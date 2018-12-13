#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            D2

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 300; // delay for half a second
// Uncomment a BoardConfiguration below to match the microcontroller configuration you are using.
String DrunkerBoard = "D1_Onboard"; // Use this board configuration for Wemos D1 mini with onboard LED only
// String DrunkerBoard = "D1_WS2812"; // Use this board configuration for Wemos D1 Mini with WS2812 shield
// String DrunkerBoard = "D1_OLED"; // Use this board configuration for Wemos D1 Mini with OLED shield

String result;

//WiFi Configuration (change these to your WiFi settings
#define WiFi_AP "mySSID"
#define WiFi_PW "MechaMechaMooMoo"
#define WiFi_retry 500  // How many milliseconds before trying to connect to WiFi again?

//Other DrunkerTracker Settings
#define dt_pollingRate 57000  // How many millseconds before checking the status again

//OLED Configuration
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
#define LOGO16_GLCD_HEIGHT 48
#define LOGO16_GLCD_WIDTH  64

Adafruit_SSD1306 display(OLED_RESET); // Make a display and name it "display"
HTTPClient http;  // Make an HTTP Client named "http"

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xE0, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0xFC, 0x3C, 0x1C, 0x1C, 0x1C, 0x1C,
  0x1C, 0x3C, 0x3C, 0x3C, 0x1E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x1E, 0x3C, 0x3C, 0x3C, 0x1C, 0x1C,
  0x1C, 0x1C, 0x1C, 0x3C, 0xF8, 0xF8, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFE, 0xFC, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0xFF, 0xFF, 0xFC, 0xF8, 0x38, 0x38, 0x38,
  0x38, 0x38, 0x38, 0x38, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xF8, 0xF8, 0x38, 0x38,
  0x38, 0x38, 0xF8, 0xFC, 0xFF, 0xFF, 0xFF, 0xF7, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
  0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC,
  0xFC, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xFF, 0x7F,
  0x06, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
  0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
  0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
  0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00
};

//#if (SSD1306_LCDHEIGHT != 48)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

void setup()   {
    //We will use the serial line for debugging our code
    Serial.begin(9600);
   
    //Set up a wifi connection
    //These are configure at the top of this sketch
    WiFi.begin(WiFi_AP, WiFi_PW);
    
  // Board Selection for the Setup Function
  
  Serial.println("Board Selected: " + DrunkerBoard);
  
  if (DrunkerBoard == "D1_Onboard") {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
  
  } else if (DrunkerBoard == "D1_WS2812") {
     pixels.begin(); // This initializes the NeoPixel library.
     white();
  } else if (DrunkerBoard == "D1_OLED") {
  
    // Set up the OLED display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
    // Show image buffer on the display hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will display the splashscreen.
    display.display();
    delay(2000);
    // Clear the buffer.
    display.clearDisplay();
  }
  
  
  // Do this for all boards
   
  // Connect to the WiFi
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { // loop while the wifi status is not equal to 'true' 
      delay(WiFi_retry); // wait a bit
      Serial.print("."); // make the loading bar grow
      // This loop will never break for as long as it cannot connect to the network
    }
    // Once connected, the loop will break and display the device network address
    Serial.println(); // skip a line
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
  }

char resetText() {
  //This function
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.display();
}

void loop() {
  result = getDrunkerState();
  Serial.println(" Fetch DrunkerState ");
  Serial.print("The Result is ");
  Serial.println(result);
   
    // Board Selection for the program loop
    
    if (DrunkerBoard == "D1_Onboard") {

    if (result == "true") { 
      digitalWrite(LED_BUILTIN, LOW);  // We go WAY low.  The LED_BUILTIN Pin on the WEMOS D1 is active LOW.
      }
    else if (result == "false") {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    
    } else if (DrunkerBoard == "D1_WS2812") {
    
      if (result == "true") { 
      green();
    }
    else if (result == "false") {
      red();
    }
    
    } else if (DrunkerBoard == "D1_OLED") {
      resetText();
      display.println(" Checking ");
      display.println("   ...    ");
      display.display();
    
    }
         
            resetText();  // Get the screen ready for a message
            display.println("DrunkerBox");
            display.println(" Not Live ");
            display.display();
        
      delay(dt_pollingRate);  // Wait for the polling rate before checking again
      resetText();
      display.drawBitmap(LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH, logo16_glcd_bmp, 0, 0, 1);
      display.display();
      delay(3000);
}

String getDrunkerState() {
  delay(1000);
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    http.begin("http://drunkerbot.hardwareflare.com/api/status/state");  //Check the bot API for the status
    if (http.GET() > 0) { //Check the returning code
      result = http.getString();
      Serial.print("Result returned is: ");
      Serial.println(result);
      http.end(); // End this http request so we can start a new one
      return result;
   }
  }
 }


String getStreamerName() {
  http.begin("http://drunkerbot.hardwareflare.com/api/status/host");  //Check the bot API for the streamer's name
   if (http.GET() > 0) {
     resetText();  // Get the screen ready for a message
     display.println("DrunkerBox");
     display.println("Hosted by:");
     display.println(http.getString());
     display.display();
     http.end();      }
      }


   void green() {
    pixels.setPixelColor(0, pixels.Color(0,255,0));
    pixels.show();
 
   }
   void red() {
    pixels.setPixelColor(0, pixels.Color(255,0,0));
    pixels.show();
 
   }

   void blue() {
    pixels.setPixelColor(0, pixels.Color(0,0,255));
    pixels.show();
 
   }

   void white() {
    pixels.setPixelColor(0, pixels.Color(255,255,255));
    pixels.show();
 
   }


