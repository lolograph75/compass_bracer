#include <Wire.h>
#include <DFRobot_QMC5883.h>
#include <Adafruit_NeoPixel.h>

#define DATAPIN 6
#define NUMLEDS 12

DFRobot_QMC5883 compass;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUMLEDS, DATAPIN, NEO_GRB + NEO_KHZ800);


uint16_t x, y, z;

void setup() {
  Serial.begin(115200);
  leds.begin();
  leds.setBrightness(64);
  for (uint8_t i = 0; i < NUMLEDS; i++) {
    leds.setPixelColor(i, 0, 0, 0);
  }
  leds.show();
  while (!compass.begin())
  {
    Serial.println("Could not find a valid QMC5883 sensor, check wiring!");
    delay(500);
  }

  if (compass.isHMC()) {
    Serial.println("Initialize HMC5883");
    compass.setRange(HMC5883L_RANGE_1_3GA);
    compass.setMeasurementMode(HMC5883L_CONTINOUS);
    compass.setDataRate(HMC5883L_DATARATE_15HZ);
    compass.setSamples(HMC5883L_SAMPLES_8);
  }
  else if (compass.isQMC()) {
    Serial.println("Initialize QMC5883");
    compass.setRange(QMC5883_RANGE_2GA);
    compass.setMeasurementMode(QMC5883_CONTINOUS);
    compass.setDataRate(QMC5883_DATARATE_50HZ);
    compass.setSamples(QMC5883_SAMPLES_8);
  }
}



void loop() {
  Vector norm = compass.readNormalize();

  // Calculate heading
  float heading = atan2(norm.YAxis, norm.XAxis);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  //for Salviac declination is 0° 39E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / PI);
  //float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / PI);
  float declinationAngle = (0.0 + (39.0 / 60.0)) / (180 / PI);
  heading += declinationAngle;

  // Correct for heading < 0deg and heading > 360deg
  if (heading < 0) {
    heading += 2 * PI;
  }

  if (heading > 2 * PI) {
    heading -= 2 * PI;
  }

  // Convert to degrees
  float headingDegrees = heading * 180 / PI;

  // Output
  Serial.print(" Heading = ");
  Serial.print(heading);
  Serial.print(" Degress = ");
  Serial.print(headingDegrees);
  Serial.println();

  int ledtoheading = map(headingDegrees, 0, 359, 11, 0); //map and reverse for the right direction

  for (uint8_t i = 0; i < NUMLEDS; i++) {
    leds.setPixelColor(i, 0, 0, 0);
  }
  leds.show();

  if (ledtoheading == 0) {
    leds.setPixelColor(0, 0, 255, 0);
    leds.setPixelColor(11, 255, 0, 0);
    leds.setPixelColor(10, 0, 255, 0);
  }  else {
    if (ledtoheading == 11) {
      leds.setPixelColor(1, 0, 255, 0);
      leds.setPixelColor(0, 255, 0, 0);
      leds.setPixelColor(11, 0, 255, 0);
    }  else {
      leds.setPixelColor(ledtoheading + 1, 0, 255, 0);
      leds.setPixelColor(ledtoheading, 255, 0, 0);
      leds.setPixelColor(ledtoheading - 1, 0, 255, 0);
    }

    leds.show();
    delay(200);
  }
}
