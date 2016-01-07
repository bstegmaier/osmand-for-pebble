#include <pebble.h>

char *itoa(int value) {
  static char buf[] = "00000000000";
  snprintf(buf, sizeof(buf), "%d", value);
  return buf;
}