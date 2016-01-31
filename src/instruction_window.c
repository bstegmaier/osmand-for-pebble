#include <pebble.h>
#include "instruction_window.h"

static Window *s_window;
static Layer *s_canvas_layer;
static TextLayer *s_distance_layer;
static TextLayer *s_street_desc_layer;
static GFont s_res_gothic_28;
static GFont s_res_bitham_34_medium_numbers;
static uint32_t graphics_resource_id;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GDrawCommandImage *s_instruction_image = gdraw_command_image_create_with_resource(graphics_resource_id);
  gdraw_command_image_draw(ctx, s_instruction_image, GPoint(0, 0));
}

static void set_street(const char *street_desc) {
  text_layer_set_text(s_street_desc_layer, street_desc);
}

static void set_distance(uint16_t distance) {
  static char buf[4];
  snprintf(buf, sizeof(buf), "%d", distance);
  text_layer_set_text(s_distance_layer, buf);
}

static void set_graphics(uint32_t resource_id) {
  graphics_resource_id = resource_id;
}

static void initialise_ui() {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif

  s_res_bitham_34_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);

  GRect bounds = layer_get_bounds((Layer *)s_window);

  // s_distance_layer
  GRect distance_bounds = grect_inset(
    GRect(bounds.size.w/2, (bounds.size.h*2)/6, bounds.size.w/2, (bounds.size.h*2)/6),
    GEdgeInsets(0, 2, 0, 0)
  );
  s_distance_layer = text_layer_create(distance_bounds);
  text_layer_set_text(s_distance_layer, "");
  text_layer_set_font(s_distance_layer, s_res_bitham_34_medium_numbers);
  text_layer_set_text_alignment(s_distance_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_distance_layer);

  // s_street_desc_layer
  GRect street_desc_bounds = grect_inset(
    GRect(0, (bounds.size.h*2)/3, bounds.size.w, bounds.size.h/3),
    GEdgeInsets(0, 2, 0, 2)
  );
  s_street_desc_layer = text_layer_create(street_desc_bounds);
  text_layer_set_text(s_street_desc_layer, "");
  text_layer_set_font(s_street_desc_layer, s_res_gothic_28);
  text_layer_set_text_alignment(s_street_desc_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_street_desc_layer);

  // s_canvas_layer
  s_canvas_layer = layer_create(GRect(0, 0, bounds.size.w/2, (bounds.size.h*2)/3));
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_canvas_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_street_desc_layer);
  layer_destroy(s_canvas_layer);
  text_layer_destroy(s_distance_layer);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_instruction_window(uint32_t graphics_resource_id, uint16_t distance, const char *street_desc) {
  initialise_ui();
  set_street(street_desc);
  set_distance(distance);
  set_graphics(graphics_resource_id);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, false);
}

void hide_instruction_window(void) {
  window_stack_remove(s_window, false);
}
