#include "tinylog.h"

#include <stdio.h>
#include <stdarg.h>

#include "tinyprintf.h"


static LOG_LEVEL_EN sg_enCurrentLogLevel = LOG_LEVEL_INFO;


void LOG_setLogLevel(LOG_LEVEL_EN enLogLevel)
{
  sg_enCurrentLogLevel = enLogLevel;
}

void LOG(LOG_LEVEL_EN enLogLevel, const char* fmt, ...)
{
  if(sg_enCurrentLogLevel > enLogLevel)
    return;

  va_list va;
  va_start(va, fmt);
  tinyvprintf(fmt, va);
  va_end(va);
}
