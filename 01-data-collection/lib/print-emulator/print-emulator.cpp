#include <stdarg.h>
#include <stdio.h>

#include "print-emulator.h"

// Function to wrap writing to the console.
// Taken from the Edge Impulse C++ SDK.
#if defined (__MINGW32__) || \
      defined (__unix__) || \
      (defined (__APPLE__) && defined (__MACH__))
void ei_printf(const char *format, ...) {
    va_list myargs;
    va_start(myargs, format);
    vprintf(format, myargs);
    va_end(myargs);
}
#else
  #error ERROR: console or serial printing is not supported on this platform
#endif
