#include <fstream>

#include "log.hpp"

int main() {
  // Basic logging examples
  log_trace("This is a trace message");
  log_debug("This is a debug message");
  log_info("This is an info message");
  log_warn("This is a warning message");
  log_error("This is an error message");

  // Set log level to only show warnings and above
  log_set_level(LOG_WARN);
  log_info("This info message won't be shown");
  log_warn("This warning will be shown");

  // Log to a file
  FILE* file = fopen("example.log", "w");
  if (file) {
    log_add_fp(file, LOG_INFO);
  }
  log_info("This will go to both console and file");

  // Set quiet mode to suppress console output
  log_set_quiet(true);
  log_fatal("This is a fatal error message");
  log_set_quiet(false);
  log_fatal("This is a fatal error message");

  // Show console on Windows (demonstrates the feature)
  show_console();

  return 0;
}