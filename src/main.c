#include <pebble.h>
#include "alert_window.h"
#include "directions_window.h"
#include "navigation_info_window.h"

static Window *s_main_window;
static StatusBarLayer *s_statusbar_layer;
static TextLayer *s_status_layer;

static void notes() {
  // backlight
  light_enable_interaction();
  
  // vibration
  // https://developer.getpebble.com/docs/c/User_Interface/Vibes/
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_alert_window(RESOURCE_ID_ALERT_GENERIC, "GENERIC DEMO WARNING MESSAGE");  
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_directions_window(RESOURCE_ID_TURN_SLIGHTLY_RIGHT, 200, "Via della Valéta");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_navigation_info_window("Via della Valéta", 142, time(NULL));
}

static void main_window_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void main_window_load(Window *window) {
  s_statusbar_layer = status_bar_layer_create();
  status_bar_layer_set_colors(s_statusbar_layer, GColorChromeYellow, GColorBlack);
  layer_add_child(window_get_root_layer(window), (Layer *)s_statusbar_layer);
  
  s_status_layer = text_layer_create(GRect(10, 60, 120, 20));
  text_layer_set_text(s_status_layer, "Waiting...");
  layer_add_child(window_get_root_layer(window), (Layer *)s_status_layer);
  
  window_set_click_config_provider(s_main_window, main_window_click_config_provider);
}

static void main_window_unload(Window *window) {
  status_bar_layer_destroy(s_statusbar_layer);
}

static void handle_init(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
