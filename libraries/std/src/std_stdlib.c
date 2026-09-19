#include <nitro.h>
#include <limits.h>

int STD_ConvertAsciiToInt(const char *s) {
  BOOL isPlus = TRUE;
  unsigned int val = 0;

  while (*s == ' ') {
    s++;
  }

  if (*s == '-') {
    isPlus = FALSE;
    s++;
  } else if (*s == '+') {
    s++;
  }

  while ('0' <= *s && *s <= '9') {
    val = (val * 10) + (*s - '0');

    if (val > INT_MAX) {
      return (isPlus) ? INT_MAX : INT_MIN;
    }
    s++;
  }
  return (isPlus) ? (int)val : (int)-val;
}

long int STD_ConvertAsciiToLong(const char *s) {
  BOOL isPlus = TRUE;
  unsigned long int val = 0;

  while (*s == ' ') {
    s++;
  }

  if (*s == '-') {
    isPlus = FALSE;
    s++;
  } else if (*s == '+') {
    s++;
  }

  while ('0' <= *s && *s <= '9') {
    val = (val * 10) + (*s - '0');

    if (val > LONG_MAX) {
      return (isPlus) ? LONG_MAX : LONG_MIN;
    }
    s++;
  }
  return (isPlus) ? (long int)val : (long int)-val;
}
