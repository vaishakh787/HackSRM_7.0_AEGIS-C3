#pragma once
#include <Arduino.h>
void alertsInit();
void alertsShowStatus(const String& msg);
void alertsTrigger(const String& ip, const String& eventType);