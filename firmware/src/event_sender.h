#pragma once
#include <Arduino.h>
void eventSenderSend(const String& ip,
                     const String& eventType,
                     const String& detail);