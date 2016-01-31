#include <pebble.h>
#include "alert_window.h"
#include "directions_window.h"
#include "navigation_info_window.h"
#include "constants.h"

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
  show_directions_window(RESOURCE_ID_TURN_SLIGHTLY_RIGHT_INSTRUCTION, 200, "Via della Valéta");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_navigation_info_window("Via della Valéta", 142, time(NULL));
}

static void main_window_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void alert_message_handler(DictionaryIterator *iterator, void *context) {
  Tuple *alert_text = dict_find(iterator, ALERT_TEXT);
  Tuple *alert_type = dict_find(iterator, ALERT_TYPE);
  // pushing an existing window again with modified content would lead to a segfault upon deinit
  hide_alert_window();
  show_alert_window(alert_type->value->int16, alert_text->value->cstring);
}

static void navigation_info_message_handler(DictionaryIterator *iterator, void *context) {
  Tuple *destination = dict_find(iterator, NAVIGATION_DESTINATION);
  Tuple *distance = dict_find(iterator, NAVIGATION_DIST_TO_DEST);
  Tuple *eta = dict_find(iterator, NAVIGATION_ETA);

  /*
  if (destination) navigation_info_window_update_destination(destination->value->cstring);
  if (distance)    navigation_info_window_update_distance(distance->value->XXX);
  if (eta)         navigation_info_window_update_eta(eta->value->XXX);
  */

  show_navigation_info_window("Via della Valéta", 142, time(NULL));
}

static void instruction_message_handler(DictionaryIterator *iterator, void *context) {
  Tuple *instruction_type = dict_find(iterator, INSTRUCTION_TYPE);
  Tuple *distance = dict_find(iterator, INSTRUCTION_DISTANCE);
  Tuple *text = dict_find(iterator, INSTRUCTION_TEXT);

  uint32_t instruction_canvas_resource = RESOURCE_ID_GO_STRAIGHT_INSTRUCTION;
  switch(instruction_type->value->uint16) {
    case GO_STRAIGHT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_GO_STRAIGHT_INSTRUCTION; break;
    case KEEP_LEFT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_KEEP_LEFT_INSTRUCTION; break;
    case KEEP_RIGHT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_KEEP_RIGHT_INSTRUCTION; break;
    case TURN_LEFT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_TURN_LEFT_INSTRUCTION; break;
    case TURN_RIGHT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_TURN_RIGHT_INSTRUCTION; break;
    case TURN_SHARPLY_LEFT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_TURN_SHARPLY_LEFT_INSTRUCTION; break;
    case TURN_SHARPLY_RIGHT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_TURN_SHARPLY_RIGHT_INSTRUCTION; break;
    case TURN_SLIGHTLY_LEFT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_TURN_SLIGHTLY_LEFT_INSTRUCTION; break;
    case TURN_SLIGHTLY_RIGHT_INSTRUCTION:
      instruction_canvas_resource = RESOURCE_ID_TURN_SLIGHTLY_RIGHT_INSTRUCTION; break;
  }
  hide_directions_window();
  show_directions_window(instruction_canvas_resource, distance->value->uint16, text->value->cstring);
}

static void message_received_handler(DictionaryIterator *iterator, void *context) {
  Tuple *message_type = dict_find(iterator, MESSAGE_TYPE);
  if(!message_type) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "No MESSAGE_TYPE received.");
    return;
  }

  switch(message_type->value->int16) {
    case ALERT_MESSAGE:           alert_message_handler(iterator, context); break;
    case NAVIGATION_INFO_MESSAGE: navigation_info_message_handler(iterator, context); break;
    case INSTRUCTION_MESSAGE:     instruction_message_handler(iterator, context); break;
    default:
      APP_LOG(APP_LOG_LEVEL_WARNING, "Unknown message type received.");
  }
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

  app_message_register_inbox_received(message_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
