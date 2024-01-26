/* ****************************************************************************
Title		: IoT Project
File Name	: 'main.cpp'
Target MCU	: Espressif ESP32 (Doit DevKit Version 1)

Project Description: 
        Main Goal: interactive and collaborative fusion of art and technology 
        1. Further understand websockets and implimentation on ESP32
        2. Control mutliple RGB LEDs on LED ring from custom webpage
        3. Control Servo motor from custom webpage
Limitations:
        1. Limited to 6ish clients due to use of two websocket ports creating 2 connections per client
                and the ESP32 cannot handle more than 12ish connections

Revision History:
When			Who			    Description of change
-----------     ----------      ------------------------
03-DEC-2023	    price	    developed to project specs
04-NOV-2023	    brooks		    Start of program

****************************************************************************

Include Files
****************************************************************************
*/
#include <Arduino.h>
#include <debounce.h>          // Debouncing library
#include <WiFi.h>              // WiFi library
#include <ESP32Ping.h>         // ESP32 ping library
#include <SSD1306Wire.h>       // Include for OLED display
#include <AutoConnect.h>       // Include the AutoConnect library
#include <favicon.h>           // Include the favicon image
#include <webpage.h>           // Include the webpage html
#include <AsyncTCP.h>          // Asynchronous TCP Library
#include <ESPAsyncWebServer.h> // Asynchronous Web Server Library
#include <FastLED.h>           // Include for Addressable LEDs
#include <colors.h>            // Include color arrays for conversions
#include <ESP32Servo.h>        // Include for servo motor

// Globals
// ****************************************************************************
const char *REMOTE_HOST = "google.com";   // Host to ping
unsigned long timePrevious = millis();    // Time of last LED toggle
const uint8_t BUTTONPIN = 17;             // Pin number connected to BUTTON
bool logicLevel = HIGH;                   // Logic level of button
Debounce myButton(BUTTONPIN, logicLevel); // Instantiate debouncing object
const uint8_t LED_PIN_GREEN = 15;         // Pin number connected to LED
const uint8_t LED_PIN_RED = 4;            // Pin number connected to LED
const uint8_t LED_PIN_RGB = 12;           // Pin number connected to addressable LEDs
const uint8_t NUM_LEDS = 16;              // Number of LEDs in Chain
const uint8_t BRIGHTNESS = 100;           // Brightness of addressable LEDs
uint32_t assignedColors[6];               // Array for HEX Colors
String colorArray[6];                     // Array for String Colors
String setColor = "";                     // used for single color setting
float level = 1;                          // brightness level
uint8_t latch = 1;                        // latch for brightness direction
CRGB leds[NUM_LEDS];                      // Define LEDs as type RGB
struct Led                                // Define Led struct
{
    uint8_t pin;                          // Pin number connected to LED
    bool state;                           // LED state false=LOW, true=HIGH

    void update()
    {
        digitalWrite(pin, state ? HIGH : LOW); // Set LED state
    }
};
bool defaultState = false;                     // Default false=LOW, true=HIGH
Led ledGreen = {LED_PIN_GREEN, defaultState};  // Declare as a type of LED
Led ledRed = {LED_PIN_RED, defaultState};      // Declare as a type of LED
SSD1306Wire display(0x3c, SDA, SCL);           // Instantiate OLED display

hw_timer_t *timer0 = NULL;                        // Create timer
portMUX_TYPE mux0 = portMUX_INITIALIZER_UNLOCKED; // Create mutex
volatile bool flagTimer0 = false;                 // Flag interrupt

uint8_t numCurrentClients = 0;                 // number of current websocket clients

Servo myServo;                             // Initialize myServo as Servo class
String angle = "";                         // String variable for angle of servo motor
int intAngle;                              // Integer variable for angle of servo motor

const int HTTP_PORT = 80;   // HTTP port number
const int WS_PORT = 3000;   // Assign port 3000 for WebSocket handeling of rLED and Servo
const int WS_PORT_2 = 3001; // Assign port 3001 for WebSocket handeling of RGB LEDs

// The server instance for WebSocket uses ESPAsyncWebServer
AsyncWebServer wsServer(WS_PORT);    // Create AsyncWebServer object on port 3000
AsyncWebServer wsServer2(WS_PORT_2); // Create AsyncWebServer object on port 3001
AsyncWebSocket ws("/redled&servo");  // Create AsyncWebSocket object on /redled&servo endpoint
AsyncWebSocket ws2("/rgbleds");      // Create AsyncWebSocket object on /servo endpoint

// This server instance is responsible for serving web pages. It is the host
// that AutoConnect will live on.
WebServer server(HTTP_PORT); // Create WebServer object on port 80
AutoConnect portal(server);  // Declare AutoConnect instance

// Board Peripherals Functions
// ****************************************************************************
void updateOLED2()
{
    display.clear();                                                        // Clear OLED display
    display.setFont(ArialMT_Plain_16);                                      // Fonts http://oleddisplay.squix.ch/
    display.drawString(0, 0, "IoT Project");                                // Top line on OLED display
    display.drawString(0, 16, "Run time: " + String(timePrevious / 1000));  // Next line displaying Run time in seconds
    display.drawString(0, 32, WiFi.localIP().toString());                   // Next line displays IP address for clients to connect to
    display.setFont(ArialMT_Plain_10);                                      // Size 10 font
    display.drawString(0, 48, WiFi.macAddress());                           // Last line is MAC address of board
    display.display();                                                      // Write OLED buffer to display
}

void updateLEDs(uint8_t numClients, String colorArray[])    // function to convert input string array of colors to HEX values and display them to RGB LEDs
{
    for (int a = 0; a < numClients; a++)                    // loop through number of clients connected (number of LEDS to display)
    {
        for (int b = 0; b < arraySize(colorName2); b++)     // for each color in the color string array find the matching color in colors.h and match it to the HEX value 
        {
            if (colorName2[b] == colorArray[a])
            {
                assignedColors[a] = colorHex2[b];           // add the HEX color value to the appropriate position 
            }
        }
    }

    for (int i = 0; i <= numClients - 1; ++i)               // for each led less than the number of connected clients (minus one beacuse clients start at 1 and LEDS start at 0)
    {
        leds[i] = assignedColors[i];                        // assign the HEX color
    }
    for (int j = numClients; j < NUM_LEDS; ++j)             // if LED number over the connected clients, write black (no color)
    {
        leds[j] = CRGB::Black;
    }
    FastLED.show();                                         // display the colors to the proper LEDs
}

void pulseBrightness()  // function to pulse the brighness of the RGB LEDs up and down
{
    if (latch == 1) {level += 1;} else {level -= 1;}
    if (level >= 250) {latch = 0;}
    if (level <= 10) {latch = 1;}

    FastLED.setBrightness(level);
}

void stringtoArray(String colorString)  // function for converting colorString received from websocket to a string array of colors
{
    colorString += ",";     // add comma to end of string so function parses out the last term
    int r = 0, t = 0;

    for (int i = 0; i < colorString.length(); i++)
    {
        if (colorString[i] == ',')
        {
            if (i - r > 1)
            {
                colorArray[t] = colorString.substring(r, i);    // pull value from between the last comma to the next comma and assign it to place in colorArray string array
                t++;
            }
            r = (i + 1);    // increment place by 1
        }
    }
}

//  WebSocket Functions
// ****************************************************************************
void notifyClients()
{
    ws.textAll(ledRed.state ? "ON" : "OFF"); // Notify all websocket clients of Red LED status
}

void notifyClients2()
{
    ws.textAll(angle); // Notify all websocket clients of servo angle
}

void notifyClients3()
{
    ws2.textAll(setColor); // Notify all websocket clients of string of colors RGB LEDs are set to
}

void notifyClients4()
{
    ws2.textAll(String(numCurrentClients)); // Notify all websocket clients of the current number of clients
}

void handleSocket3000Message(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = '\0';                        // Null-terminate the data
        if (strcmp((char *)data, "toggle") == 0) // Compare data to "toggle"
        {
            ledRed.state = !ledRed.state; // Toggle LED on/off
            ledRed.update();              // Update LED
            notifyClients();              // Notify websocket clients
        }
        else
        {
            angle = (char *)data;       // if not toggle then its a numeric value for servo position
            intAngle = angle.toInt();   // convert number in string to an integer value
            notifyClients2();           // Notify websocket clients
        }
    }
}

void handleSocket3001Message(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    { 
        data[len] = '\0';               // Null-terminate the data
        setColor = (char *)data;        // assign data to string variable
        stringtoArray(String(setColor));// convert string to array of color strings
        notifyClients3();               // Notify websocket clients
    }
}

// Event handlers for AsyncWebSocket correspond to message reception events.
void onrLEDServoEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                      void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleSocket3000Message(arg, data, len);    // handle Red LED and Servo messages from Port 3000
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void onColorSetEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                     void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        numCurrentClients++;    // on connect add one to current number of clients
        notifyClients4();       // notify all clients the number of connected clients
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        numCurrentClients--;    // on disconnect subtract one from current number of clients
        notifyClients4();       // notify all clients the number of connected clients
        break;
    case WS_EVT_DATA:
        handleSocket3001Message(arg, data, len);    // handle RGB LED string message from port 3001
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

String processor(const String &var)
{
    return String(var == "STATE" && ledRed.state ? "ON" : "OFF");
}

// Interrupt Service Routine (ISR)
// ****************************************************************************
void IRAM_ATTR timerISR0()
{
    portENTER_CRITICAL_ISR(&mux0); // Enter critical section
    flagTimer0 = true;             // Set flag variable
    portEXIT_CRITICAL_ISR(&mux0);  // Exit critical section
}

// Begin Code
// ****************************************************************************
void setup()
{
    delay(3000);                   // Wait for serial monitor to open on pc
    Serial.begin(115200);          // Start serial monitor baud rate=115200
    Serial.println();              // Blank line for readability
    pinMode(ledGreen.pin, OUTPUT); // Set pin digital output
    pinMode(ledRed.pin, OUTPUT);   // Set pin digital output
    pinMode(BUTTONPIN, INPUT);     // Set pin digital input
    ledRed.state = HIGH;           // Set LED state
    ledRed.update();               // Update LED

    // Web Server Root URL
    server.on("/", []()
              {
        String html = index_html; // HTML, CSS, & Javascript in webpage.h
        html.replace("%FAVICON%", FAVICON);
        html.replace("%AUTOCONNECT_MENU%", AUTOCONNECT_LINK(BAR_32));
        html.replace("%STATE%", processor("STATE"));
        html.replace("%SERVOPOS%", angle);              // PAUL ADD
        server.send(200, "text/html", html); });

    server.onNotFound([]()
                      { server.send(404, "text/plain", "File not found"); });

    // Register an event handler for WebSocket and start ESPAsyncWebServer
    ws.onEvent(onrLEDServoEvent);
    ws2.onEvent(onColorSetEvent);
    wsServer.addHandler(&ws);
    wsServer2.addHandler(&ws2);

    // Start AsyncWebServer only when WiFi is connected
    portal.onConnect([](IPAddress ip)
                     { 
        wsServer.begin();
        wsServer2.begin(); });

    AutoConnectConfig config;        // Create config object
    config.ota = AC_OTA_BUILTIN;     // Enable OTA in AutoConnect
    config.ticker = true;            // Enable LED for connection state
    config.tickerPort = BUILTIN_LED; // Select LED to use for ticker
    config.tickerOn = LOW;           // Logic level for LED ticker
    config.title = "IoT Project";    // Default AutoConnect Menu Title
    config.autoReconnect = true;     // Reconnect to previously saved SSID
    config.reconnectInterval = 1;    // Reconnection interval in seconds
    portal.config(config);           // Update changes to AutoConnect

    if (portal.begin())
    {
        Serial.println();
        Serial.println("WiFi Connected: " + WiFi.SSID());
        Serial.println("IP address: " + WiFi.localIP().toString());
        Serial.println("Station MAC: " + WiFi.macAddress());
        Serial.println("Autoconnect Menu http://" + WiFi.localIP().toString() + "/_ac");
        Serial.println("Pinging host: " + (String)REMOTE_HOST);
        if (Ping.ping(REMOTE_HOST))
        {
            Serial.print("Success!! - ");
            float avg_time_ms = Ping.averageTime();
            Serial.println("Internet access " + (String)avg_time_ms + "ms");
        }
        else
        {
            Serial.println("Error :( ");
        }
    }
    display.init();                            // Init OLED display
    display.displayOn();                       // Turn on OLED display
    display.clear();                           // Clear OLD display
    display.setTextAlignment(TEXT_ALIGN_LEFT); // Set text alignment

    FastLED.addLeds<WS2812, LED_PIN_RGB, GRB>(leds, NUM_LEDS);  // add LEDs of type WS2812, on Pin 12, of color structure GRB
    FastLED.setBrightness(BRIGHTNESS);                          // initialize LEDs at BRIGHTNESS
    for (int i = 0; i < NUM_LEDS; ++i)                          // loop through all LEDs
    {
        leds[i] = CRGB::Black;                                  // set current LED to Black, NO COLOR
    }
    FastLED.show();                                             // show all programmed leds at set brightness and color

    myServo.attach(14);     // define myServo on pin 14
    myServo.write(0);       // write and angle of 0 degrees
    delay(1000);            // delay 

    timer0 = timerBegin(0, 80, true);               // 80MHz/80 prescaler=1 MHz
    timerAttachInterrupt(timer0, &timerISR0, true); // Attach ISR
    timerAlarmWrite(timer0, 1000, true);            // Interrupt time 1ms
    timerAlarmEnable(timer0);                       // Enable interrupt
}

// Main program
// ****************************************************************************
void loop()
{
    portal.handleClient(); // Handle AutoConnect portal
    ws.cleanupClients();   // Clean up disconnected clients
    ws2.cleanupClients();  // Clean up disconnected clients
    updateLEDs(numCurrentClients, colorArray);  // Update color of RGB LEDs availble as a result of the number of current clients
    pulseBrightness();  // pulse RGB LEDs brightness up and down

    if (millis() - timePrevious >= 1000) // Repeats every 1sec
    {
        timePrevious = millis();          // Reset time
        ledGreen.state = !ledGreen.state; // Toggle LED on/off
        ledGreen.update();                // Update LED
        updateOLED2();                    // Update OLED display

        myServo.write(intAngle);          // Write angle to servo
    }

    if (flagTimer0) // Check timer interrupt
    {
        flagTimer0 = false; // Reset flag variable
        myButton.update();  // Check button every 1ms
    }

    if (myButton.isPressed()) // Check if button was pushed
    {
        ledRed.state = !ledRed.state; // Toggle LED on/off
        ledRed.update();              // Update LED
        notifyClients();              // Notify websocket clients
    }
}
