# log.hpp

A minimal, thread-safe, header-only logging library in modern C++, modified from [rxi/log.c](https://github.com/rxi/log.c). 

## Basic Usage

### Include the Header

```cpp
#include "log.hpp"
```

### Basic Logging

```cpp
log_trace("This is a TRACE message");
log_debug("Debug info: %d", value);
log_info("Application started");
log_warn("Something might be wrong...");
log_error("An error occurred: %s", error_msg);
log_fatal("Critical failure, exiting!");
```

### Set Log Level

```cpp
log_set_level(LOG_WARN); // Only WARN, ERROR, FATAL logs are printed
```

### Enable/Disable Output

```cpp
log_set_quiet(true);  // Disable output to stderr
log_set_quiet(false); // Re-enable
```

### Add File Output

```cpp
FILE* fp = fopen("app.log", "w");
log_add_fp(fp, LOG_DEBUG);
```

### Custom Callback

```cpp
void my_callback(log_Event* ev) {
    // Handle log event manually
    std::cerr << "[Custom] " << ev->fmt << std::endl;
}

log_add_callback(my_callback, nullptr, LOG_WARN);
```

### Thread Safety

```cpp
std::mutex mtx;

log_set_lock([](bool lock, void* udata) {
    std::mutex* m = static_cast<std::mutex*>(udata);
    if (lock)
        m->lock();
    else
        m->unlock();
}, &mtx);
```

## LICENSE

This project is licensed under the MIT License. Ported and extended from [rxi/log.c](https://github.com/rxi/log.c), which is also MIT Licensed.
