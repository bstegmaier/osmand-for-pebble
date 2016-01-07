#include <pebble.h>
#include "navigation_info_window.h"

static Window *s_window;
static TextLayer *s_destination_layer;
static TextLayer *s_distance_layer;
static TextLayer *s_distance_label_layer;
static TextLayer *s_eta_layer;
static TextLayer *s_eta_label_layer;
static StatusBarLayer *s_statusbar_layer;
static GFont s_res_gothic_28;

static void set_eta(time_t timestamp) {
  static char buf[] = "??:??";
  // TODO display in 12h format, if prefered by user
  // TODO handle timestamps >= 24h after current time
  strftime(buf, sizeof(buf), "%R", localtime(&timestamp));
  text_layer_set_text(s_eta_layer, buf);
}

static void set_destination(const char *destination) {
  text_layer_set_text(s_destination_layer, destination);
}

static void set_distance(uint32_t distance) {
  static char buf[6];
  snprintf(buf, sizeof(buf), "%ld", distance);
  text_layer_set_text(s_distance_layer, buf);
}

static void initialise_ui() {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);
   
  GRect bounds = layer_get_bounds((Layer *)s_window);
  bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
  bounds.size.h -= STATUS_BAR_LAYER_HEIGHT;
  
  // s_statusbar_layer
  s_statusbar_layer = status_bar_layer_create();
  status_bar_layer_set_colors(s_statusbar_layer, GColorDarkGreen, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_statusbar_layer);
  
  // s_destination_layer
  const GRect destination_bounds = GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h/3);
  s_destination_layer = text_layer_create(destination_bounds);
  text_layer_set_text(s_destination_layer, "?");
  text_layer_set_background_color(s_destination_layer, GColorBrightGreen);
  text_layer_set_font(s_destination_layer, s_res_gothic_28);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_destination_layer);
  
  // s_distance_layer
  const GRect distance_bounds = GRect(bounds.size.w/3, bounds.origin.y + bounds.size.h/3, (bounds.size.w*2)/3, bounds.size.h/3);
  s_distance_layer = text_layer_create(distance_bounds);
  text_layer_set_text(s_distance_layer, "?");
  text_layer_set_font(s_distance_layer, s_res_gothic_28);
  text_layer_set_text_alignment(s_distance_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_distance_layer);
  
  // s_distance_label_layer
  const GRect distance_label_bounds = GRect(0, bounds.origin.y + bounds.size.h/3, bounds.size.w/3, bounds.size.h/3);
  s_distance_label_layer = text_layer_create(distance_label_bounds);
  text_layer_set_text(s_distance_label_layer, "Dist");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_distance_label_layer);
  
  // s_eta_layer
  const GRect eta_bounds = GRect(bounds.size.w/3, bounds.origin.y + (bounds.size.h*2)/3, (bounds.size.w*2)/3, bounds.size.h/3);
  s_eta_layer = text_layer_create(eta_bounds);
  text_layer_set_text(s_eta_layer, "?");
  text_layer_set_font(s_eta_layer, s_res_gothic_28);
  text_layer_set_text_alignment(s_eta_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_eta_layer);
  
  // s_eta_label_layer
  const GRect eta_label_bounds = GRect(0, bounds.origin.y + (bounds.size.h*2)/3, bounds.size.w/3, bounds.size.h/3);
  s_eta_label_layer = text_layer_create(eta_label_bounds);
  text_layer_set_text(s_eta_label_layer, "ETA");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_eta_label_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_destination_layer);
  text_layer_destroy(s_distance_layer);
  text_layer_destroy(s_distance_label_layer);
  text_layer_destroy(s_eta_layer);
  text_layer_destroy(s_eta_label_layer);
  status_bar_layer_destroy(s_statusbar_layer);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_navigation_info_window(const char *destination, uint32_t distance, time_t eta_timestamp) {
  initialise_ui();
  set_destination(destination);
  set_distance(distance);
  set_eta(eta_timestamp);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_navigation_info_window(void) {
  window_stack_remove(s_window, true);
}
