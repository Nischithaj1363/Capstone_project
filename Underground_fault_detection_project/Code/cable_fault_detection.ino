  #include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 2        // Pin connected to DHT11
#define DHTTYPE DHT11   // DHT 11 sensor
#define Buzzer 5
const int voltageSensor = A0;
#define FAULT_2KM_PIN 10 // Pin for detecting 2km cable fault
//#define FAULT_4KM_PIN 9 // Pin for detecting 4km cable fault
#define SHORT_CIRCUIT_PIN 7 // Pin for detecting short circuit
#define LINE_TO_GROUND_PIN 8 // Pin for detecting line-to-ground fault
#define LINE_TO_LINE_PIN 4   // Pin for detecting line-to-line fault
float vOUT = 0.0;
float vIN = 0.0;
float R1 = 3000.0;
float R2 = 750.0;
int val = 0;
// Set up the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27 for a 16x2 LCD

// Set up the DHT sensor
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(9600);      // Serial monitor for debugging and GPS

  lcd.begin(16, 2);        // Initialize LCD
  lcd.init();
  lcd.backlight();         // Turn on LCD backlight
  
  dht.begin();             // Initialize DHT sensor
  
  pinMode(FAULT_2KM_PIN, INPUT_PULLUP); // Enable pull-up resistor

  pinMode(SHORT_CIRCUIT_PIN, INPUT_PULLUP); // Enable pull-up resistor
  pinMode(LINE_TO_GROUND_PIN, INPUT_PULLUP); // Enable pull-up resistor for line-to-ground fault
  pinMode(LINE_TO_LINE_PIN, INPUT_PULLUP);   // Enable pull-up resistor for line-to-line fault
  
  pinMode(voltageSensor, INPUT);  // Set voltage pin as input
  pinMode(Buzzer, OUTPUT);
}

void loop() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.println(temperature);
  val = analogRead(voltageSensor);
  vOUT = (val * 2.9) / 1024.0;
  vIN = vOUT / (R2/(R1+R2));
  Serial.print("Input = ");
  Serial.println(vIN);



  // Check for faults
  if (digitalRead(FAULT_2KM_PIN) == LOW) {
    // Fault at 2km
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fault at 2KM");
    digitalWrite(Buzzer,HIGH);

  }

  else if (digitalRead(LINE_TO_GROUND_PIN) == LOW) {
    // Line-to-ground fault
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Line to Ground Fault");
    digitalWrite(Buzzer,HIGH);

  }
  else if (digitalRead(LINE_TO_LINE_PIN) == LOW) {
    // Line-to-line fault
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Line to Line Fault");
    digitalWrite(Buzzer,HIGH);

  }
  // Check for short circuit
  else if (digitalRead(SHORT_CIRCUIT_PIN) == HIGH) {
    // Short circuit
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Short Circuit");
    digitalWrite(Buzzer,HIGH);

  }
  else if (vIN < 3.0) {
    // Voltage is less than 2V
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Low Voltage");
    digitalWrite(Buzzer,HIGH);

  }
  else if (temperature >= 35.0) {
    // Temperature is greater than 35°C
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("High Temp (>35C)");

  }
  else {
    // No faults detected, display the sensor data
    lcd.setCursor(0, 0);  // First line
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print("C ");
    lcd.print("H:");
    lcd.print(humidity);
    lcd.print("%");

    lcd.setCursor(0, 1);  // Second line
    lcd.print("V:");
    lcd.print(vIN, 2); // Show voltage with 2 decimal places
    lcd.print("V");

    digitalWrite(Buzzer,LOW);
  }

  delay(2000); // Wait for 2 seconds before next update
}
