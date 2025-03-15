#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MovingAvg.h"

#define SCREEN_ADDRESS 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define OLED_RESET -1 // 1 if sharing Arduino reset pin
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);

MovingAvg<int, 50, long> analogValues;

#define DECAY_RATE 0.01
// or use const float DECAY_RATE = 0.01;

#define LED_SLOW 3
#define LED_FAST 4
#define BUZZER 5
#define SWITCH_PIN 6

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); // SWITCHCAPAVCC tells Arduino to switch on OLED internal voltage
  display.clearDisplay(); // clear buffer
  display.display(); // update the display with the buffer's content

  pinMode(LED_SLOW, OUTPUT);
  pinMode(LED_FAST, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(SWITCH_PIN, INPUT_PULLUP);
}

void displayRate(float cprRate) {
  display.clearDisplay();
  display.setTextSize(2); // Text size is 2
  display.setTextColor(1); // Text color is white
  display.setCursor(0,0); // Start at top-left cursor
  display.println(F("CPR\n Rate"));
  display.setTextSize(2);
  display.println("");
  display.println(cprRate);
  display.display();
}

void loop() {
  int  analogValue = analogRead(A0);
  analogValues.push(analogRead(A0));
  float smoothedValue = analogValues.getMovingAvg();
  Serial.println(smoothedValue); // current moving averages

  // keeping track of max and min
  static float analogValueMax = 0;
  static float analogValueMin = 1023;
  analogValueMax = (smoothedValue > analogValueMax) ? smoothedValue : analogValueMax;
  analogValueMin = (smoothedValue < analogValueMin) ? smoothedValue : analogValueMin;

  // elastic envelope
  float stepSize = (analogValueMax - analogValueMin) * DECAY_RATE;
  analogValueMax -= stepSize;

  // define two thresholds for hysteresis
  float upperThreshold = analogValueMax * 0.7 + analogValueMin * 0.3;
  float lowerThreshold = analogValueMax * 0.3 + analogValueMin * 0.7;

  // calculate CPR rate
  static bool wasAboveUpperThreshold = false;
  static long lastRisingEdgeTime = micros(); // in microseconds
  static float cprRate = 0; // in Hz
  if (smoothedValue > upperThreshold && (analogValueMax - analogValueMin) > 50) {
    if (!wasAboveUpperThreshold) {
      long currTime = micros();
      long timeDiff = currTime - lastRisingEdgeTime;
      lastRisingEdgeTime = currTime;
      cprRate = float(1e6) / timeDiff;
    }
    wasAboveUpperThreshold = true;
  } else if (smoothedValue < lowerThreshold) {
    wasAboveUpperThreshold = false;
  }

  int cprBPM = cprRate * 60;

  bool isAudioMode = (digitalRead(SWITCH_PIN) == LOW);

  if (isAudioMode) {
    if (cprBPM < 100) {
      digitalWrite(BUZZER, HIGH);
      digitalWrite(LED_SLOW, LOW);
      digitalWrite(LED_FAST, LOW);
    } else if (cprBPM > 140) {
      digitalWrite(BUZZER, HIGH);
      digitalWrite(LED_SLOW, LOW);
      digitalWrite(LED_FAST, LOW);
    } else {
      digitalWrite(BUZZER, LOW);
    }
  } else {
    if (cprBPM < 100) {
      digitalWrite(LED_SLOW, HIGH);
      digitalWrite(LED_FAST, LOW);
      digitalWrite(BUZZER, LOW);
    } else if (cprBPM > 140) {
      digitalWrite(LED_SLOW, LOW);
      digitalWrite(LED_FAST, HIGH);
      digitalWrite(BUZZER, LOW);
    } else {
      digitalWrite(LED_SLOW, LOW);
      digitalWrite(LED_FAST, LOW);
      digitalWrite(BUZZER, LOW);
    }
  }

  Serial.print("CPR BPM: ");
  Serial.println(cprBPM);

  // Calculate CPR depth
  float cprDepth = 0.7 * (analogValueMax - analogValueMin) / (cprRate * cprRate); // in cm

  //Serial.println(cprRate);
  
  displayRate(cprRate);
}