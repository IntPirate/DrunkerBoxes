#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// Load Local fies:
#include "Adafruit_SSD1306.h" //using the local one because we are working on doing some changes
#include "OLED_Animation.h"
#include "WIFI_Config.h" // Please modify the "WIFI_Config_Example.h" file before uploading

#define WiFi_retry 500  // How many milliseconds before trying to connect to WiFi again?

//Other DrunkerTracker Settings
#define dt_pollingRate 57000  // How many seconds before checking the status again

//OLED Configuration
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
#define LOGO16_GLCD_HEIGHT 48
#define LOGO16_GLCD_WIDTH  64

//const unsigned char* Frame_Name [LOGO16_GLCD_HEIGHT*LOGO16_GLCD_WIDTH];

Adafruit_SSD1306 display(OLED_RESET); // Make a display and name it "display"
HTTPClient http;  // Make an HTTP Client named "http"

//#if (SSD1306_LCDHEIGHT != 48)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

void setup()   {
	// Set up Serial communication from the board to the computer
	Serial.begin(115200);

	//Set up a wifi connection
	//These are configure at the top of this sketch
	WiFi.begin(WiFi_AP, WiFi_PW);

	// Set up the OLED display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)

	// Show image buffer on the display hardware.
	// Since the buffer is intialized with an Adafruit splashscreen
	// internally, this will display the splashscreen.
	display.display();
	delay(1000);

	// Clear the buffer.
	display.clearDisplay();

	// Connect to the WiFi
	Serial.print("Connecting");
	while (WiFi.status() != WL_CONNECTED) // loop while the wifi status is not equal to 'true'
	{
		delay(WiFi_retry); // wait a bit
		Serial.print("."); // make the loading bar grow
		// This loop will never break for as long as it cannot connect to the network
	}
	// Once connected, the loop will break and display the device network address
	Serial.println(); // skip a line
	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
}

void resetText() {
	//This function
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.clearDisplay();
	display.display();
}

void Check_Drunkerbox_Status() {
	display.clearDisplay();
	display.println(" Checking ");
	display.println("   ...    ");
	display.display();
	if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
		http.begin("http://drunkerbot.hardwareflare.com/api/status/state");  //Check the bot API for the status
		if (http.GET() > 0) { //Check the returning code
			if (http.getString() == "true") { //Get the body of the reply and show it on the OLED
				http.end(); // End this http request so we can start a new one
				http.begin("http://drunkerbot.hardwareflare.com/api/status/host");  //Check the bot API for the streamer's name
				if (http.GET() > 0) {
					resetText();  // Get the screen ready for a message
					display.println("DrunkerBox");
					display.println("Hosted by:");
					display.println(http.getString());
					display.display();
				}
				http.end();
			}
			else {
				resetText();  // Get the screen ready for a message
				display.println("DrunkerBox");
				display.println(" Not Live ");
				display.display();
			}
		}
		else {
			http.end();   //Close connection
		}
	}
	delay(dt_pollingRate);  // Wait for the polling rate before checking again
}

void Display_Frame(const unsigned char* Frame_Name) {
	display.drawBitmap(0, 0, Frame_Name, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, 1);
	display.display();
	delay(animation_delay);
	display.clearDisplay();
}

void Fill_beer_Animation(){
	Display_Frame(empty);
	Display_Frame(bottle_full_and_glass_empty);
	Display_Frame(fill_up_1);
	Display_Frame(fill_up_2);
	Display_Frame(fill_up_3);
	Display_Frame(filling_up_10);
	Display_Frame(filling_up_20);
	Display_Frame(filling_up_35);
	Display_Frame(filling_up_50);
	Display_Frame(filling_up_75);
	Display_Frame(filling_up_90);
	Display_Frame(filling_up_100);
}


void loop() {
	Check_Drunkerbox_Status();
	Fill_beer_Animation();
}
