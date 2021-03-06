#ifndef _ESPFC_DEBUG_H_
#define _ESPFC_DEBUG_H_

#include <EspGpio.h>

#if 0
#define PIN_DEBUG(v) EspGpio::digitalWrite(D0, v)
#define PIN_DEBUG_INIT() pinMode(D0, OUTPUT)
#else
#define PIN_DEBUG(v)
#define PIN_DEBUG_INIT()
#endif

#if ESPFC_SERIAL_DEBUG
//#define LOG_SERIAL_INIT() Serial.begin(115200); Serial.flush(); Serial.println()
#define LOG_SERIAL_INIT() Serial.begin(115200); Serial.println()
#define LOG_SERIAL_DEBUG(v) Serial.print(' '); Serial.print(v)
#else
#define LOG_SERIAL_INIT()
#define LOG_SERIAL_DEBUG(v)
#endif

#if ESPFC_SERIAL_DEBUG
template <typename T>
void D(T t)
{
  Serial.println(t);
}

template<typename T, typename... Args>
void D(T t, Args... args) // recursive variadic function
{
  Serial.print(t);
  Serial.print(' ');
  D(args...);
}
#else
#define D(...)
#endif

#endif
