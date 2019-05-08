// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define RELAYPIN  7

#define DHTPIN    2         // Pin which is connected to the DHT sensor.

// Uncomment the type of sensor in use:
#define DHTTYPE           DHT11     // DHT 11 
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS = 5000; // delay for 5 seconds

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define BODY_PIN            5
#define TAIL_PIN            6

// How many NeoPixels are attached to the Arduino?
#define BODY_NUMPIXELS      37
#define TAIL_NUMPIXELS      21

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel body_strip = Adafruit_NeoPixel(BODY_NUMPIXELS, BODY_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel tail_strip = Adafruit_NeoPixel(TAIL_NUMPIXELS, TAIL_PIN, NEO_GRB + NEO_KHZ800);

int pre_relay = 0;

void setup() {
  Serial.begin(9600); 
  // Initialize device.
  pinMode(RELAYPIN, OUTPUT);
  dht.begin();
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  //delayMS = sensor.min_delay / 1000;

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  body_strip.begin();
  body_strip.setBrightness(32); // 1/8 brightness
  body_strip.show(); // Initialize all pixels to 'off' 

  tail_strip.begin();
  tail_strip.setBrightness(32); // 1/8 brightness
  tail_strip.show(); // Initialize all pixels to 'off' 
}

void loop() {
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
    if (event.temperature <= 5) {
      body_colorWipe(body_strip.Color(0, 0, 255), 50); // Blue
    } else if (event.temperature > 5 && event.temperature <= 15) {
      body_colorWipe(body_strip.Color(0, 255, 255), 50); // Cyan
    } else if (event.temperature > 15 && event.temperature <= 25) {
      body_colorWipe(body_strip.Color(0, 255, 0), 50); // Green
    } else if (event.temperature > 25 && event.temperature <= 35) {
      body_colorWipe(body_strip.Color(255, 255, 0), 50); // Yellow
    } else if (event.temperature > 35) {
      body_colorWipe(body_strip.Color(255, 0, 0), 50); // Red
    }
  }
  // Delay between measurements.
  delay(delayMS);
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
    if (event.relative_humidity <= 20) {
      tail_colorWipe(tail_strip.Color(255, 0, 0), 50); // Red
    } else if (event.relative_humidity > 20 && event.relative_humidity <= 40) {
      tail_colorWipe(tail_strip.Color(255, 255, 0), 50); // Yellow
    } else if (event.relative_humidity > 40 && event.relative_humidity <= 60) {
      tail_colorWipe(tail_strip.Color(0, 255, 0), 50); // Green
    } else if (event.relative_humidity > 60 && event.relative_humidity <= 80) {
      tail_colorWipe(tail_strip.Color(0, 255, 255), 50); // Cyan
    } else if (event.relative_humidity > 80 && event.relative_humidity <= 100) {
      tail_colorWipe(tail_strip.Color(0, 0, 255), 50); // Blue
    }
    if (event.relative_humidity <= 50) {
      if (pre_relay == 0) {
        digitalWrite(RELAYPIN, HIGH);
        pre_relay == 1;
      }
    } else {
      digitalWrite(RELAYPIN, LOW);
      pre_relay = 0;
    }
  }
}

// Fill the dots one after the other with a color
void body_colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<body_strip.numPixels(); i++) {
    body_strip.setPixelColor(i, c);
    body_strip.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void tail_colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<tail_strip.numPixels(); i++) {
    tail_strip.setPixelColor(i, c);
    tail_strip.show();
    delay(wait);
  }
}
