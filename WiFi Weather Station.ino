// Basic serial communication with ESP8266
// Uses serial monitor for communication with ESP8266
//
//  Pins
//  Arduino pin 2 (RX) to ESP8266 TX
//  Arduino pin 3 to voltage divider then to ESP8266 RX
//  Connect GND from the Arduiono to GND on the ESP8266
//  Pull ESP8266 CH_PD HIGH
//
// When a command is entered in to the serial monitor on the computer
// the Arduino will relay it to the ESP8266
//

#include <DateTime.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define ESP_TX 10
#define ESP_RX 9

#include <SoftwareSerial.h>
SoftwareSerial ESPserial(ESP_TX, ESP_RX);

#define SSID "arion" // введите ваш SSID
#define PASS "27466126" // введите ваш пароль
#define PORT_SPEED 9600 // введите ваш SSID

String LOCATION 	= "Togliatty,ru"; // id местоположения
String DST_HOST 	= "api.openweathermap.org"; //api.openweathermap.org
String DST_PORT 	= "80"; //api.openweathermap.org
String DST_API_KEY 	= "44db6a862fba0b067b1930da0d769e98"; //openweathermap APIKey
int DST_GMT			= 4;

int temp = 0;
int humidity = 0;
int pressure = 0;
int wind_speed = 0;

unsigned long loopTime;
unsigned int lastScreen = 0;
unsigned int dataUpdateTick = 0;

byte bottom[8] =
{
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b11111
};
byte top[8] =
{
	0b11111,
	0b11111,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};
byte fill[8] =
{
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};
byte leftcircle[8] =
{
	0b00000,
	0b00000,
	0b00001,
	0b00011,
	0b00011,
	0b00001,
	0b00000,
	0b00000
};
byte rightcircle[8] =
{
	0b00000,
	0b00000,
	0b10000,
	0b11000,
	0b11000,
	0b10000,
	0b00000,
	0b00000
};
byte topandbottom[8] =
{
	0b11111,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111
};
byte drop[8] =
{
	0b00100,
	0b00100,
	0b01010,
	0b01010,
	0b10001,
	0b10001,
	0b10001,
	0b01110
};
byte scale[8] =
{
	0b00100,
	0b01010,
	0b01010,
	0b01110,
	0b01110,
	0b11111,
	0b11111,
	0b01110
};

void seperator(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)3);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)4);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)3);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)4);
}

void zero(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void one(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.print(" ");
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)0);
}

void two(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)5);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)0);
}

void three(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)5);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void four(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos, 1);
	lcd.print(" ");
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.print(" ");
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void five(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)5);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void six(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)5);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void seven(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos, 1);
	lcd.print(" ");
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)1);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.print(" ");
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void eight(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)5);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void nine(int cursorpos)
{
	lcd.setCursor(cursorpos, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 1, 0);
	lcd.write((uint8_t)5);
	lcd.setCursor(cursorpos + 1, 1);
	lcd.write((uint8_t)0);
	lcd.setCursor(cursorpos + 2, 0);
	lcd.write((uint8_t)2);
	lcd.setCursor(cursorpos + 2, 1);
	lcd.write((uint8_t)2);
}

void writeNumber(int number, int cursorpos)
{
	if(number == 0)
	{
		zero(cursorpos);
	}
	if(number == 1)
	{
		one(cursorpos);
	}
	if(number == 2)
	{
		two(cursorpos);
	}
	if(number == 3)
	{
		three(cursorpos);
	}
	if(number == 4)
	{
		four(cursorpos);
	}
	if(number == 5)
	{
		five(cursorpos);
	}
	if(number == 6)
	{
		six(cursorpos);
	}
	if(number == 7)
	{
		seven(cursorpos);
	}
	if(number == 8)
	{
		eight(cursorpos);
	}
	if(number == 9)
	{
		nine(cursorpos);
	}
}

void writeTime(int hours, int minutes)
{
	lcd.clear();
	
	int hours1 = 0;
	int minutes1 = 0;
	int minutes2 = 0;
	int hours2 = 0;
	if(minutes >= 10)
	{
		minutes1 = minutes / 10;
		minutes2 = minutes % 10;
	}
	else
	{
		minutes2 = minutes;
	}
	if(hours >= 10)
	{
		hours1 = hours / 10;
		hours2 = hours % 10;
	}
	else
	{
		hours2 = hours;
	}
	writeNumber(hours1, 0);
	writeNumber(hours2, 4);
	seperator(7);
	writeNumber(minutes1, 9);
	writeNumber(minutes2, 13);
}

void setup()
{  
	// set up the LCD's number of columns and rows: 
  	lcd.begin(16, 2);
  	lcdWrite(0, "Please whait...");
	lcd.createChar(0, bottom);
	lcd.createChar(1, top);
	lcd.createChar(2, fill);
	lcd.createChar(3, leftcircle);
	lcd.createChar(4, rightcircle);
	lcd.createChar(5, topandbottom);
	lcd.createChar(6, drop);
	lcd.createChar(7, scale);
  	
	// communication with the host computer
	Serial.begin(PORT_SPEED);

	// Start the software serial for communication with the ESP8266
	ESPserial.begin(PORT_SPEED);
	
	// First sync datetime with unixtime
	// makeTime(byte sec, byte min, byte hour, byte day, byte month, int year );
	DateTime.sync(DateTime.makeTime(00, 00, 00, 01, 01, 1970));
	
  	loopTime = millis();

	// try to connect to wifi
	for(int i = 0; i < 3; i++)
	{
		if(connectWiFi()) { return; }
	}

	terminal();
}

void loop()
{
	unsigned long currentTime = millis();
	
  	if(currentTime >= (loopTime + 5000)) { // обновляем каждые 5 секунд
  		lastScreen += 1;
  		
		switch (lastScreen) {
			case 1:
				DateTime.available(); // refresh time
				writeTime(DateTime.Hour, DateTime.Minute);
				break;
			case 2:
				lcdWrite(0, String(temp) + "\337C  \006" + String(humidity) + "%");
				lcdWrite(1, "\177" + String(wind_speed) + "m/s  \007" + String(pressure) + "hP");
				lastScreen = 0;
				break;
		}
  	
    	loopTime = currentTime;
    	
    	dataUpdateTick += 1;
    	
    	if (dataUpdateTick == 1) {
    		if (!updateTimeAndWeather()) {
    			dataUpdateTick = 0; // try to update data again on next tick
    		}
    	}
    	
    	if (dataUpdateTick >= 360) { // update data each 30 minutes
    		dataUpdateTick = 0;
    	}
  	}
}

boolean connectWiFi()
{
	Serial.println("Try to connect to WiFi...");
  	lcdWrite(0, "Connecting...");
	
	Serial.println("AT+RST");
	ESPserial.println("AT+RST");

	if(ESPserial.find("ready"))
	{
		Serial.println("WiFi - is ready");
	}
	else
	{
		Serial.println("WiFI - dosn't respond.");
  		lcdWrite(0, "Not respond");
		return false;
	}

	Serial.println("AT+CWMODE=1");
	ESPserial.println("AT+CWMODE=1");

	delay(1000);
	
	String cmd = "AT+CWJAP=\"";
	cmd += SSID;
	cmd += "\",\"";
	cmd += PASS;
	cmd += "\"";
	Serial.println(cmd);
	ESPserial.println(cmd);

	if(ESPserial.find("OK"))
	{
		Serial.println("OK, Connected to WiFi.");
  		lcdWrite(0, "Connected");

		delay(1000);
	
		// turn to alone connection mode
		Serial.println("AT+CIPMUX=0");
		ESPserial.println("AT+CIPMUX=0");
		
		delay(5000);
	
    	return true;
  	} else {
    	Serial.println("Can not connect to the WiFi.");
  		lcdWrite(0, "Not connected");
    	
    	delay(5000);
    	
    	return false;
  	}
}

boolean updateTimeAndWeather()
{
	String cmd = "AT+CIPSTART=\"TCP\",\"" + DST_HOST + "\"," + DST_PORT;
	
	Serial.println(cmd);
	ESPserial.println(cmd);
	
	if (ESPserial.find("Error")) return false;
	
	cmd = "GET /data/2.5/weather?q=" + LOCATION + "&appid=" + DST_API_KEY + " HTTP/1.0\r\nHost: " + DST_HOST + "\r\n\r\n";
	
	Serial.print("AT+CIPSEND=");
	Serial.println(cmd.length());
	
	ESPserial.print("AT+CIPSEND=");
	ESPserial.println(cmd.length());
	
	if(ESPserial.find(">"))
	{
		Serial.print(">");
	}
	else
	{
		Serial.println("AT+CIPCLOSE");
		ESPserial.println("AT+CIPCLOSE");

		Serial.println("Connection timeout");
		terminal();
		return false;
	}
	
	Serial.println(cmd);
	ESPserial.print(cmd);
	
  	unsigned int i = 0; //счетчик времени
  	int n = 0; // счетчик символов
  	char date[30] = "";
  	if (ESPserial.find("Date: ")) {
	  	while (i<60000) {
	    	if(ESPserial.available()) {
	      		char c = ESPserial.read();
	      		if(c=='G') break;
	      		date[n]=c;
	      		n++;
	      		i=0;
	    	}
	    	i++;
	  	}
  	}
 
 	i = 0;
 	n = 1;
  	char weather[200] = "{";
  	if (ESPserial.find("\"main\":{")) {
	  	while (i<60000) {
	    	if(ESPserial.available()) {
	      		char c = ESPserial.read();
	      		weather[n]=c;
	      		if(c=='}' && n > 130) break;
	      		n++;
	      		i=0;
	    	}
	    	i++;
	  	}
  	}
  	
  	if(strlen(date) == 0) { return false; }
  	
  	parseWeatherData(String(weather));
  	syncDateTime(String(date));
  	
  	return true;
}

void parseWeatherData(String weather) {
	Serial.println(weather);
	
	int temp_begin = weather.indexOf("\"temp\":") + 7;
	int temp_end = weather.indexOf(".", temp_begin);
	if (temp_end == -1) {
		temp_end = weather.indexOf(",", temp_begin);
	}
	temp = weather.substring(temp_begin, temp_end).toInt() - 273; // to celsius
	
	int humidity_begin = weather.indexOf("\"humidity\":") + 11;
	int humidity_end = weather.indexOf(".", humidity_begin);
	if (humidity_end == -1) {
		humidity_end = weather.indexOf(",", humidity_begin);
	}
	humidity = weather.substring(humidity_begin, humidity_end).toInt();
	
	int pressure_begin = weather.indexOf("\"pressure\":") + 11;
	int pressure_end = weather.indexOf(".", pressure_begin);
	if (pressure_end == -1) {
		pressure_end = weather.indexOf(",", pressure_begin);
	}
	pressure = weather.substring(pressure_begin, pressure_end).toInt();
	
	int wind_speed_begin = weather.indexOf("\"speed\":") + 8;
	int wind_speed_end = weather.indexOf(".", wind_speed_begin);
	if (wind_speed_end == -1) {
		wind_speed_end = weather.indexOf(",", wind_speed_begin);
	}
	wind_speed = weather.substring(wind_speed_begin, wind_speed_end).toInt();
}

void syncDateTime(String date_time) {
	Serial.println(date_time);
	int lenght = date_time.length();
	int hour = date_time.substring(lenght-9,lenght-7).toInt() + DST_GMT;
	int minute = date_time.substring(lenght-6,-4).toInt();
	
	DateTime.sync(DateTime.makeTime(00, minute, hour, 01, 01, 1970));
}

void lcdWrite(int line, String message) {
	if (line == 0) {
		lcd.clear();
	}
	lcd.setCursor(0, line);
	
	lcd.print(message);
}

void terminal() {
	Serial.println("Start debug terminal.");
  	lcdWrite(0, "Debug terminal");
    while (1) {
		if (ESPserial.available() > 0) {
		    Serial.write(ESPserial.read());
		}
	
	
		if (Serial.available()) { // Outgoing data
		    ESPserial.write(Serial.read());
		}
    }
}
