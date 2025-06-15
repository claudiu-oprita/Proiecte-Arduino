#include "arduino_secrets.h"
#include "thingProperties.h"
#include <EEPROM.h>
#include "base64.h"
#include <time.h>

const int LM35_PIN = A0;
const int LED_PIN = 7;
const int HUMIDITY_PIN = 13;
const int MAX_MESSAGES = 10;

const char* smtp_server = "mail.smtp2go.com"; 
const int smtp_port = 2525; 
WiFiClient client;
bool useSchedule = false;

// Structura pentru alerta de inundatie
struct FloodAlertEvent {
  float temperature;
  bool ledState;
};

// Structura pentru actiune LED
struct LedActionEvent {
  bool ledState;
};

int floodMessageIndex = 0;
int ledMessageIndex = 0;

const int FLOOD_EVENT_SIZE = sizeof(FloodAlertEvent);
const int LED_EVENT_SIZE = sizeof(LedActionEvent);

// Adrese EEPROM
const int FLOOD_EVENTS_ADDR = 0; // Start EEPROM pentru alerte inundatie
const int LED_EVENTS_ADDR = FLOOD_EVENTS_ADDR + FLOOD_EVENT_SIZE * MAX_MESSAGES; // dupa alerte
const int FLOOD_INDEX_ADDR = LED_EVENTS_ADDR + LED_EVENT_SIZE * MAX_MESSAGES; // dupa actiuni LED
const int LED_INDEX_ADDR = FLOOD_INDEX_ADDR + 1; // 1 byte pentru index flood

void setup() {
  Serial.begin(9600);
  delay(1500);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(LED_PIN, OUTPUT);
  pinMode(HUMIDITY_PIN, INPUT);

  // Citire index din EEPROM
  floodMessageIndex = EEPROM.read(FLOOD_INDEX_ADDR);
  if (floodMessageIndex >= MAX_MESSAGES) floodMessageIndex = 0;

  ledMessageIndex = EEPROM.read(LED_INDEX_ADDR);
  if (ledMessageIndex >= MAX_MESSAGES) ledMessageIndex = 0;

  Serial.println("Ultimele 10 alerte de inundatie:");
  printFloodAlerts();

  Serial.println("Ultimele 10 actiuni ale LED-ului:");
  printLedActions();
}

void loop() {
  ArduinoCloud.update();

  int rawTemp = analogRead(LM35_PIN);
  temperature = (rawTemp * 5.0 / 1023.0) * 100.0;

  int humidityRaw = digitalRead(HUMIDITY_PIN);
  humidity = (humidityRaw == LOW) ? 100.0 : 0.0;
 // fixedSchedule();
    } 

void sendEmail() {
  if (client.connect(smtp_server, smtp_port)) {
    Serial.println("Conectat la SMTP server.");

  
    auto readSmtpResponse = []() {
    unsigned long timeout = millis() + 5000;
    while (!client.available() && millis() < timeout) delay(10);
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line.startsWith("250 ") || line.startsWith("235 ") || line.startsWith("354") || line.startsWith("221") || line.startsWith("530") || line == "") {
        break;
      }
    }
    };

    client.println("EHLO arduino");
    readSmtpResponse();

    client.println("AUTH LOGIN");
    readSmtpResponse();

    client.println(base64::encode("PS-Nanu2"));  // username base64
    readSmtpResponse();

    client.println(base64::encode("xV9uQ52ynhvVeh2D"));  // password base64
    readSmtpResponse();

    client.println("MAIL FROM:<claudiu.oprita-mandrut@student.upt.ro>");
    readSmtpResponse();

    client.println("RCPT TO:<opritaclaudiu7@gmail.com>");
    readSmtpResponse();

    client.println("DATA");
    readSmtpResponse();

    // Trimiterea continutului email-ului
    client.println("Subject: Umiditate crescuta detectata");
    client.println("From: Arduino <claudiu.oprita-mandrut@student.upt.ro>");
    client.println("To: opritaclaudiu7@gmail.com");
    client.println("Content-Type: text/plain; charset=utf-8");
    client.println("");
    client.println("ALERTA!");
    client.println("Dispozitivul a detectat un nivel de umiditate ridicat!");
    client.println("Luati masuri cat de repede posibil.");
    client.println("");
    client.println(".");

    readSmtpResponse(); 

    client.println("QUIT");
    readSmtpResponse();  

    Serial.println("Email-ul a fost trimis!");
  } else {
    Serial.println("Conectare esuata la server!.");
  }

  client.stop();
}

void onLedChange() {
  digitalWrite(LED_PIN, led);
  saveLedAction(led);
  printLedActions();
}

void onFloodAlertButtonChange() {
  if (floodAlertButton) {
    saveFloodAlert();
    printFloodAlerts();
    sendEmail();
    floodAlertButton = false;
  }
}
void onDeleteLastFloodAlertButtonChange() {
  if (deleteLastFloodAlertButton) {
    deleteLastFloodAlert();
    printFloodAlerts();  
    deleteLastFloodAlertButton = false; // resetăm butonul în cloud
  }
}

// Salvare alerta inundatie
void saveFloodAlert() {
  FloodAlertEvent event;
  event.temperature = temperature;
  event.ledState = led;

  int addr = FLOOD_EVENTS_ADDR + floodMessageIndex * FLOOD_EVENT_SIZE;
  byte* p = (byte*)(void*)&event;
  for (int i = 0; i < FLOOD_EVENT_SIZE; i++) {
    EEPROM.write(addr + i, p[i]);
  }

  floodMessageIndex = (floodMessageIndex + 1) % MAX_MESSAGES;
  EEPROM.write(FLOOD_INDEX_ADDR, floodMessageIndex);
}

// Citire alerta inundatie
void readFloodAlert(int index, FloodAlertEvent& event) {
  int addr = FLOOD_EVENTS_ADDR + index * FLOOD_EVENT_SIZE;
  byte* p = (byte*)(void*)&event;
  for (int i = 0; i < FLOOD_EVENT_SIZE; i++) {
    p[i] = EEPROM.read(addr + i);
  }
}

// Salvare actiune LED
void saveLedAction(bool ledState) {
  LedActionEvent event;
  event.ledState = ledState;

  int addr = LED_EVENTS_ADDR + ledMessageIndex * LED_EVENT_SIZE;
  byte* p = (byte*)(void*)&event;
  for (int i = 0; i < LED_EVENT_SIZE; i++) {
    EEPROM.write(addr + i, p[i]);
  }

  ledMessageIndex = (ledMessageIndex + 1) % MAX_MESSAGES;
  EEPROM.write(LED_INDEX_ADDR, ledMessageIndex);
}

// Citire actiune LED
void readLedAction(int index, LedActionEvent& event) {
  int addr = LED_EVENTS_ADDR + index * LED_EVENT_SIZE;
  byte* p = (byte*)(void*)&event;
  for (int i = 0; i < LED_EVENT_SIZE; i++) {
    p[i] = EEPROM.read(addr + i);
  }
}

// Afisare alerte inundatie
void printFloodAlerts() {
  for (int i = 0; i < MAX_MESSAGES; i++) {
    int idx = (floodMessageIndex + i) % MAX_MESSAGES;
    FloodAlertEvent event;
    readFloodAlert(idx, event);

    //Date valide
    Serial.print("- Alerta: Inundatie detectata! ");
    Serial.print("Temperatura: ");
    Serial.print(event.temperature);
    Serial.print(" C, LED-ul este ");
    Serial.println(event.ledState ? "aprins" : "stins");
  }
  Serial.println("=======================");
}

// Afisare actiuni LED
void printLedActions() {
  for (int i = 0; i < MAX_MESSAGES; i++) {
    int idx = (ledMessageIndex + i) % MAX_MESSAGES;
    LedActionEvent event;
    readLedAction(idx, event);

    Serial.print("- LED-ul este ");
    Serial.println(event.ledState ? "aprins" : "stins");
  }
  Serial.println("=======================");
}

//Respectare orar de functionare
void fixedSchedule(){
  if (schedule.isActive()) {
      digitalWrite(LED_PIN, HIGH);  // aprinde becul
      } 
  else { 
      digitalWrite(LED_PIN, LOW);   // stinge becul
  }
}

// Stergere alerte inundatie
void deleteLastFloodAlert() {
  if (floodMessageIndex == 0) {
    floodMessageIndex = MAX_MESSAGES - 1;
  } else {
    floodMessageIndex--;
  }

  int addr = FLOOD_EVENTS_ADDR + floodMessageIndex * FLOOD_EVENT_SIZE;

  for (int i = 0; i < FLOOD_EVENT_SIZE; i++) {
    EEPROM.write(addr + i, 0xFF);  // Scriem FF peste datele vechi
  }

  EEPROM.write(FLOOD_INDEX_ADDR, floodMessageIndex);

  Serial.println("Ultima alerta de inundatie a fost stearsa.");
}

void onMessageChange()  {
  // code
}

//Schimbare
void onScheduleChange()  {
  Serial.println("Orarul a fost actualizat.");
}
