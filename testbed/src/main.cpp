#include <entry.h>

void testbed() {
  log_fatal("The value of pi is %.2f", 3.14);
  log_error("The value of pi is %.2f", 3.14);
  log_warn("The value of pi is %.2f", 3.14);
  log_info("The value of pi is %.2f", 3.14);
  log_debug("The value of pi is %.2f", 3.14);
  log_trace("The value of pi is %.2f", 3.14);
}
