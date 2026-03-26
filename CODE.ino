#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ---------------- NETWORK ----------------
const char* ssid = "vivo Y21";
const char* password = "7418922390";
const char* BOTtoken = "8482210850:AAGg0cJwqTNVilV6hHbNO-J3XhToXOyDqwU";
const char* CHAT_ID = "996053949";

// ---------------- USERS ----------------
const String USER_1_UID = "85 A1 39 06";
const String USER_2_UID = "9E D7 F3 05";
const String USER_3_UID = "A3 DF 37 27";
const String USER_4_UID = "63 11 C0 27";

// ---------------- CERTIFICATES ----------------
const String CERT_UID   = "C0 D5 9E 5F";   // old
const String CERT_UID_1 = "03 B5 9E E4";   // new
const String CERT_UID_2 = "33 4F B4 E4";   // new

// ---------------- PINS ----------------
#define SS_1_PIN D0
#define SS_2_PIN D1
#define RST_PIN  D3
#define SERVO_PIN D4
#define LED_G     D2
#define LED_R     3
#define BUZZER    D8

MFRC522 rfid_door(SS_1_PIN, RST_PIN);
MFRC522 rfid_file(SS_2_PIN, RST_PIN);
Servo lockerServo;
ESP8266WebServer server(80);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);

// ---------------- STATE ----------------
String lockerStatus = "LOCKED";
String fileStatus = "Checking...";
String lastUser = "None";
String lastActionTime = "N/A";
String eventLogs = "--- SMART LOCKER LOGS ---\n";

// ------------------------------------------------
// TELEGRAM SEND
// ------------------------------------------------
void sendTelegram(String msg) {
  server.stop();
  digitalWrite(SS_1_PIN, HIGH);
  digitalWrite(SS_2_PIN, HIGH);

  client.setInsecure();
  client.setBufferSizes(512, 512);

  if (bot.sendMessage(CHAT_ID, msg, "")) {
    Serial.println("Telegram Sent");
  } else {
    Serial.println("Telegram Failed");
  }

  client.stop();
  server.begin();
}

// ------------------------------------------------
// WEB DASHBOARD
// ------------------------------------------------
void handleDownload() {
  server.sendHeader("Content-Disposition", "attachment; filename=logs.txt");
  server.send(200, "text/plain", eventLogs);
}

void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style>";
  html += "body { font-family: sans-serif; text-align: center; background: #1a1a2e; color: white; }";
  html += ".card { background: #16213e; padding: 20px; border-radius: 15px; display: inline-block; margin-top: 30px; border: 2px solid #0f3460; }";
  html += ".btn { background: #e94560; color: white; padding: 10px 20px; text-decoration: none; border-radius: 5px; display: inline-block; margin-top: 15px; }";
  html += "</style><title>Locker Dashboard</title></head><body><div class='card'><h1>Locker Admin</h1>";
  html += "<p>Door Status: <b>" + lockerStatus + "</b></p>";
  html += "<p>File Presence: <b>" + fileStatus + "</b></p>";
  html += "<hr><p>Last Action: " + lastUser + " @ " + lastActionTime + "</p>";
  html += "<a href='/download' class='btn'>DOWNLOAD LOGS</a></div></body></html>";
  server.send(200, "text/html", html);
}

// ------------------------------------------------
// SETUP
// ------------------------------------------------
void setup() {
  Serial.begin(115200);

  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(SS_1_PIN, OUTPUT);
  pinMode(SS_2_PIN, OUTPUT);
  digitalWrite(SS_1_PIN, HIGH);
  digitalWrite(SS_2_PIN, HIGH);

  lockerServo.attach(SERVO_PIN);
  lockerServo.write(0);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.on("/", handleRoot);
  server.on("/download", handleDownload);
  server.begin();

  timeClient.begin();

  SPI.begin();
  rfid_door.PCD_Init();
  rfid_file.PCD_Init();

  Serial.println("\nConnected. IP:");
  Serial.println(WiFi.localIP());
}

// ------------------------------------------------
// LOOP
// ------------------------------------------------
void loop() {
  server.handleClient();

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 4000) {
    fileStatus = isCertPresent() ? "PRESENT" : "MISSING";
    timeClient.update();
    lastCheck = millis();
  }

  digitalWrite(SS_2_PIN, HIGH);
  digitalWrite(SS_1_PIN, LOW);

  if (rfid_door.PICC_IsNewCardPresent() && rfid_door.PICC_ReadCardSerial()) {

    String scanned = getUID(rfid_door);

    if (scanned == USER_1_UID) handleTransaction("User 1");
    else if (scanned == USER_2_UID) handleTransaction("User 2");
    else if (scanned == USER_3_UID) handleTransaction("User 3");
    else if (scanned == USER_4_UID) handleTransaction("User 4");
    else denyAccess();

    rfid_door.PCD_Init();
  }
}

// ------------------------------------------------
// TRANSACTION
// ------------------------------------------------
void handleTransaction(String name) {

  bool wasInside = isCertPresent();

  lastUser = name;
  lastActionTime = timeClient.getFormattedTime();
  lockerStatus = "OPEN";

  digitalWrite(LED_G, HIGH);
  beep(1);

  lockerServo.write(90);
  delay(5000);

  lockerServo.write(0);
  lockerStatus = "LOCKED";
  digitalWrite(LED_G, LOW);
  delay(2000);

  bool isInsideNow = isCertPresent();

  String actionMsg = "";

  if (wasInside && !isInsideNow)
    actionMsg = "⚠️ " + name + " Took certificate (should be returned within 7 days).";
  else if (!wasInside && isInsideNow)
    actionMsg = "✅ " + name + " RETURNED certificate";
  else
    actionMsg = "🔓 " + name + " accessed locker";

  eventLogs += "[" + lastActionTime + "] " + actionMsg + "\n";

  sendTelegram(actionMsg + " at " + lastActionTime);
}

// ------------------------------------------------
// CERTIFICATE CHECK
// ------------------------------------------------
bool isCertPresent() {

  digitalWrite(SS_1_PIN, HIGH);
  digitalWrite(SS_2_PIN, LOW);

  rfid_file.PCD_Init();
  bool found = false;

  if (rfid_file.PICC_IsNewCardPresent() && rfid_file.PICC_ReadCardSerial()) {

    String uid = getUID(rfid_file);

    if (uid == CERT_UID ||
        uid == CERT_UID_1 ||
        uid == CERT_UID_2) {
      found = true;
    }
  }

  digitalWrite(SS_2_PIN, HIGH);
  return found;
}

// ------------------------------------------------
// UID
// ------------------------------------------------
String getUID(MFRC522 &reader) {
  String uid = "";

  for (byte i = 0; i < reader.uid.size; i++) {
    if (reader.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(reader.uid.uidByte[i], HEX);
    if (i < reader.uid.size - 1) uid += " ";
  }

  uid.toUpperCase();
  reader.PICC_HaltA();
  return uid;
}

// ------------------------------------------------
// DENY
// ------------------------------------------------
void denyAccess() {
  digitalWrite(LED_R, HIGH);
  beep(2);

  String t = timeClient.getFormattedTime();
  eventLogs += "[" + t + "] 🚫 UNAUTHORIZED SCAN\n";

  sendTelegram("🚫 Unauthorized card at " + t);

  digitalWrite(LED_R, LOW);
}

// ------------------------------------------------
// BUZZER
// ------------------------------------------------
void beep(int c) {
  for (int i = 0; i < c; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(50);
  }
}
