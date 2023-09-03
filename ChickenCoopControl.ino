#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebSrv.h"
#include <ESP32Time.h>
#include <Stepper.h>
#include "esp_task_wdt.h"

ESP32Time rtc(3600);  // GMT Offset

// Access Point;
DNSServer dnsServer;
AsyncWebServer server(80);
String password = "pw";

// Stepper;
#define STEPS 2049    // Anzahl der Schritte pro Umdrehung
#define MOTOR_RPM 10  // Motorgeschwindigkeit in RPM
// ULN2003 Motor Driver Pins
#define IN1 26
#define IN2 25
#define IN3 33
#define IN4 32
Stepper stepper(STEPS, IN1, IN3, IN2, IN4);

// Button
#define ButtonPin 35

// Time Variables
int user_year;
int user_month;
int user_day;
int user_hour;
int user_minute;
bool time_received = false;

// Door Variable
int user_distance;
bool distance_received = false;
int stepsToGo = 0;
bool stepsToGo_recieved = false;
bool isOpen = true;

// Time Arrays
#include "fillArrays.h"

// Captive AP-Websites
#include "APwebsites.h"

void addInfoToAP(AsyncWebServerRequest *request, int month);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", pwInput.c_str());
  }
};

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (time_received && distance_received) {
      addInfoToAP(request, rtc.getMonth());
    } else {
      request->send_P(200, "text/html", index_html);
    }
    Serial.println("Client Connected");
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    if (request->hasParam("password")){
      inputMessage = request->getParam("password")->value();
      inputParam = "password";
      if(inputMessage == password) addInfoToAP(request, 0);
      else {
        Serial.println("Wrong PWD!");
static const char PROGMEM wrongPwMessage[] = "<script>\n"
                                                      "var infoContainer = document.getElementById('infoContainer');\n"
                                                      "infoContainer.innerHTML = 'Wrong Password. :(';\n"
                                                      "</script>\n";
        request->send(200, "text/html", pwInput + String(wrongPwMessage));
      }
    }

    if (request->hasParam("datetime")) {
        inputMessage = request->getParam("datetime")->value();
        inputParam = "datetime";
        // Parse the inputMessage to separate year, month, day, hour, and minute
        sscanf(inputMessage.c_str(), "%d-%d-%dT%d:%d", &user_year, &user_month, &user_day, &user_hour, &user_minute);
        Serial.println(inputMessage);
        time_received = true;
      
    }
    if (request->hasParam("distance")) {
        inputMessage = request->getParam("distance")->value();
        inputParam = "distance";
        user_distance = inputMessage.toInt();
        Serial.println(inputMessage);
        distance_received = true;
      
    }
    if (time_received && distance_received) {
      // handle time
        rtc.setTime(0, user_minute, user_hour - 1, user_day, user_month, user_year);
        Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));  // returns time with specified format
        struct tm timeinfo = rtc.getTimeStruct();
        //time_received = false;
        // handle distance
        stepsToGo = (user_distance / 3) * STEPS;
        Serial.println(user_distance);
        //distance_received = false;
        addInfoToAP(request, rtc.getMonth());
    }
    if (request->hasParam("action")) {
      String action = request->getParam("action")->value();
      moveStepper("Manual via AP");
      addInfoToAP(request, rtc.getMonth());
    }
    handleAdvancedOptions(request, rtc.getMonth());
  });
}

void handleAdvancedOptions(AsyncWebServerRequest *request, int month) {
  if (request->hasParam("sunsetHour") && request->getParam("sunsetHour")->value() != "") {
    int newSunsetHour = request->getParam("sunsetHour")->value().toInt();
    sunsetHour[month] = newSunsetHour;
    addInfoToAP(request, rtc.getMonth());
  }

  if (request->hasParam("sunsetMinute") && request->getParam("sunsetMinute")->value() != "") {
    int newSunsetMinute = request->getParam("sunsetMinute")->value().toInt();
    sunsetMinute[month] = newSunsetMinute;
    addInfoToAP(request, rtc.getMonth());
  }

  if (request->hasParam("sunriseHour") && request->getParam("sunriseHour")->value() != "") {
    int newSunriseHour = request->getParam("sunriseHour")->value().toInt();
    sunriseHour[month] = newSunriseHour;
    addInfoToAP(request, rtc.getMonth());
  }

  if (request->hasParam("sunriseMinute") && request->getParam("sunriseMinute")->value() != "") {
    int newSunriseMinute = request->getParam("sunriseMinute")->value().toInt();
    sunriseMinute[month] = newSunriseMinute;
    addInfoToAP(request, rtc.getMonth());
  }
}

void addInfoToAP(AsyncWebServerRequest *request, int month) {

    String sunsetTime = String(sunsetHour[month]) + ":" + String(sunsetMinute[month]);
    String sunriseTime = String(sunriseHour[month]) + ":" + String(sunriseMinute[month]);

    String dataResponse = "<script>\n";
    dataResponse += "var dataContainer = document.getElementById('dataContainer');\n";
    dataResponse += "var nextSunset = '" + sunsetTime + "';\n";
    dataResponse += "var nextSunrise = '" + sunriseTime + "';\n";
    dataResponse += "dataContainer.innerHTML = 'Received Data: Date/Time = " + rtc.getTime("%A, %B %d %Y %H:%M:%S") + ", Distance = " + String(user_distance) + "<br>Next Sunset: ' + nextSunset + '<br>Next Sunrise: ' + nextSunrise;\n";
    dataResponse += "</script>\n";

    String showButton = "<script>\n";
    showButton += "var buttonContainer = document.getElementById('button-container');\n";
    showButton += "buttonContainer.innerHTML = '<form method=\"GET\" action=\"/get\">";
    showButton += isOpen ? "<button id=\"openButton\" name=\"action\" value=\"open\" class=\"disabled-button\">&#8593; Open</button>" : "<button id=\"openButton\" name=\"action\" value=\"open\" class=\"enabled-button\">&#8593; Open</button>";
    showButton += !isOpen ? "<button id=\"closeButton\" name=\"action\" value=\"close\" class=\"disabled-button\">&#8595; Close</button>" : "<button id=\"closeButton\" name=\"action\" value=\"close\" class=\"enabled-button\">&#8595; Close</button>";
    showButton += "</form>';\n";
    showButton += "</script>\n";

    request->send(200, "text/html", index_html + dataResponse + showButton);
  
}


void setup() {

  pinMode(ButtonPin, INPUT_PULLUP);

  stepper.setSpeed(10);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ChickenCoopControl 🐔");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Setting up Async WebServer");
  setupServer();
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);  //only when requested from AP
  //more handlers...
  server.begin();
  Serial.println("All Done!");

  esp_task_wdt_init(100, true);
}

void loop() {
  dnsServer.processNextRequest();
  esp_task_wdt_reset();

  // Serielle Abfragen (Debugging)
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 't') Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));  // returns time with specified format
    if (command == 'h') Serial.println(rtc.getHour(true));                     // returns hour 0-24
    if (command == 'p') Serial.println(rtc.getMinute());                       // returns minute 0-59
    if (command == 'l') Serial.println(isOpen ? "open" : "close");             // returns door status
    if (command == 'm') Serial.println(rtc.getMonth() + 1);                    // returns month 1-12
    if (command == 's') {
      int month = rtc.getMonth();
      String sunsetTime = String(sunsetHour[month]) + " " + String(sunsetMinute[month]);
      String sunriseTime = String(sunriseHour[month]) + " " + String(sunriseMinute[month]);
      Serial.println(sunsetTime + " " + sunriseTime);
    }
  }

  checkMoveConditions();
}

void checkMoveConditions() {
  if (rtc.getHour(true) == sunsetHour[rtc.getMonth()] && rtc.getMinute() == sunsetMinute[rtc.getMonth()] && isOpen) moveStepper("Sunset. Good Night!");        // abends schließen
  if (rtc.getHour(true) == sunriseHour[rtc.getMonth()] && rtc.getMinute() == sunriseMinute[rtc.getMonth()] && !isOpen) moveStepper("Sunrise. Good Morning!");  // morgens öffnen
  //if(digitalRead(ButtonPin)==LOW) move("Button press."); // manuell bewegen
}

void moveStepper(String validation) {
  stepper.step(isOpen ? -stepsToGo : stepsToGo);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  isOpen = !isOpen;
  Serial.print(isOpen ? "Opened" : "Closed");
  Serial.println(" because " + validation);
}
