#include <pebble.h>
#include "alert_window.h"

#define WINDOW_MARGIN   10

static Window *s_window;
static GFont s_res_gothic_28_bold;
static GBitmap *s_icon_bitmap;
static BitmapLayer *s_icon_layer;
static TextLayer *s_text_layer;

static void initialise_ui(uint32_t resource_id, const char *text) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, 0);
  #endif
  window_set_background_color(s_window, PBL_IF_COLOR_ELSE(GColorYellow, GColorWhite));
  
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  
  GRect bounds = layer_get_bounds((Layer *)s_window);
  
  // s_icon_layer
  s_icon_bitmap = gbitmap_create_with_resource(resource_id);
  if (!s_icon_bitmap) {
    s_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ALERT_GENERIC);
  }
  GRect bitmap_bounds = gbitmap_get_bounds(s_icon_bitmap);
  GRect icon_bounds = GRect(
    (bounds.size.w - bitmap_bounds.size.w) / 2,
    WINDOW_MARGIN,
    bitmap_bounds.size.w,
    bitmap_bounds.size.h
  );
  s_icon_layer = bitmap_layer_create(icon_bounds);
  bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
  bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet); // for transparency
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_icon_layer);
  
  // s_text_layer
  GRect text_bounds = grect_inset(
    GRect(0, icon_bounds.origin.y + icon_bounds.size.h, bounds.size.w, bounds.size.h - (icon_bounds.origin.y + icon_bounds.size.h)),
    GEdgeInsets(WINDOW_MARGIN)
  );
  s_text_layer = text_layer_create(text_bounds);
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_text(s_text_layer, text);
  text_layer_set_font(s_text_layer, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_text_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_icon_layer);
  gbitmap_destroy(s_icon_bitmap);
  text_layer_destroy(s_text_layer);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_alert_window(uint32_t resource_id, const char *text) {
  initialise_ui(resource_id, text);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, false);
}

void hide_alert_window(void) {
  window_stack_remove(s_window, false);
}
