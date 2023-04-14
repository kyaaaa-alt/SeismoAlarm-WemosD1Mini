#ifndef alarm_h
#define alarm_h

void alarm()
{
    for(int x=0;x<100;x++)
    {
      digitalWrite(alarm_led, HIGH);
      digitalWrite(buzzPin, LOW);
      delay(timeOn);
      digitalWrite(buzzPin, HIGH);
      digitalWrite(alarm_led, LOW);
      delay(timeOff);
      digitalWrite(warn_led, HIGH);
      digitalWrite(buzzPin, LOW);
      delay(timeOn);
      digitalWrite(warn_led, LOW);
      digitalWrite(buzzPin, HIGH);
      delay(timeOff);
    }
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(wifi_led, LOW);
    }
    if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(wifi_led, LOW);
      digitalWrite(warn_led, LOW);
    }
}

#endif
