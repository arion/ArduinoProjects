// ########################################
// Begin Pins
// ########################################

// Nokia 5110 screen pins
#define PIN_SCLK 8
#define PIN_SDIN 9
#define PIN_DC 10
#define PIN_RESET 12
#define PIN_SCE 11

// DS18B20 sensor pins
#define PIN_DS_DATA 13

// China digital encoder pins
#define PIN_DT_A 4
#define PIN_CLC_B 5
#define PIN_SW_C 3

// ########################################
// End Pins
// ########################################

float sensorTemp = 0;
int defineTemp = 0;

#define MODE_HOT 0
#define MODE_COLD 1
#define DEVIATION 1.0 // deveation from define temperature

int mode = MODE_HOT; // 0 - hot, 1 - cold
bool enable = false;
bool screenUpdate = true;

unsigned long currentTime;
unsigned long loopTime;

void setup(void)
{
	currentTime = millis();
	loopTime = currentTime;

	systemInit();
	screenInit();
	encoderInit();
}

void loop(void)
{
	currentTime = millis();
	if(currentTime >= (loopTime + 5))   // tick every 5ms (200 Hz)
	{
		encoderRead();
		sensorRead();

		if (screenUpdate)
		{
			systemUpdate();
			screenRender();
		}
		screenUpdate = false;

		loopTime = currentTime;
	}
}

// ########################################
// Begin screen logic
// ########################################

#include <LCD5110_Basic.h>

// Data which pins use Nokia5110
LCD5110 myGLCD(PIN_SCLK, PIN_SDIN, PIN_DC, PIN_RESET, PIN_SCE);

// Init fonts for display
extern uint8_t SmallFont[];
extern uint8_t BigNumbers[];
extern uint8_t MediumNumbers[];

void screenInit()
{
	Serial.begin(9600);

	// Start Nokia 5110 display with contrast
	myGLCD.InitLCD(63);
}

void screenRender()
{
	myGLCD.clrScr();

	myGLCD.setFont(MediumNumbers);
	myGLCD.printNumI(defineTemp, 0, 0);

	myGLCD.setFont(BigNumbers);
	myGLCD.printNumF(sensorTemp, 1, 0, 25);

	myGLCD.setFont(SmallFont);

	if (defineTemp >= 100)
	{
		myGLCD.print("C", 39, 15);
	}
	else if (defineTemp < 10)
	{
		myGLCD.print("C", 13, 15);
	}
	else
	{
		myGLCD.print("C", 26, 15);
	}

	if (sensorTemp >= 100)
	{
		myGLCD.print("C", 75, 45);
	}
	else if (sensorTemp < 10)
	{
		myGLCD.print("C", 45, 45);
	}
	else
	{
		myGLCD.print("C", 60, 45);
	}

	if (mode == MODE_HOT)
	{
		myGLCD.print("hot", 60, 0);
	}
	else
	{
		myGLCD.print("cold", 60, 0);
	}

	if (enable)
	{
		myGLCD.print("on", 60, 15);
	}
	else
	{
		myGLCD.print("off", 60, 15);
	}
}

// ########################################
// Begin screen logic
// ########################################

// ########################################
// Begin temperature sensor logic
// ########################################

#include <OneWire.h>

// Data wire is plugged ds18b20 into pin 13 on the Arduino
OneWire ds(PIN_DS_DATA);

int sensorIntervel = 200; // interval in ticks (5ms * interval)
int sensorCurrentInreval = 0;

float lastTemp = 0;

void sensorRead()
{
	byte addr[8];

	if ((sensorCurrentInreval != 0) && (sensorCurrentInreval != sensorIntervel))
	{
		// just plus if this is not first or last tick
		sensorCurrentInreval += 1;
		return;
	}

	if (!sensorValid(addr)) // find sensor address
	{
		return;
	}

	if (sensorCurrentInreval == 0)   // if first tick then send request
	{
		sensorRequest(addr);
	}

	if (sensorCurrentInreval == sensorIntervel)   // if last tick then read response
	{
		sensorTemp = sensorResponse(addr);

		if (lastTemp != sensorTemp)
		{
			screenUpdate = true;
			lastTemp = sensorTemp;
		}

		if (defineTemp == 0)
		{
			defineTemp = sensorTemp;
		}

		Serial.print("Temperature: ");
		Serial.print(sensorTemp);
		Serial.println();
	}

	sensorCurrentInreval += 1;

	if (sensorCurrentInreval >= sensorIntervel)   // if last tick then drop counter
	{
		sensorCurrentInreval = 0;
	}
}

bool sensorValid(byte addr[8])
{
	if (!ds.search(addr))
	{
		Serial.println("No more addresses.\n");
		ds.reset_search();
		return false;
	}

	Serial.print("R=");
	for(int i = 0; i < 8; i++)
	{
		Serial.print(addr[i], HEX);
		Serial.print(" ");
	}
	Serial.println();

	if (OneWire::crc8( addr, 7) != addr[7])
	{
		Serial.println("CRC is not valid!\n");
		return false;
	}

	return true;
}

bool sensorRequest(byte addr[8])
{
	ds.reset();
	ds.select(addr);
	ds.write(0x44, 1);
}

float sensorResponse(byte addr[8])
{
	byte data[12];

	ds.reset();
	ds.select(addr);
	ds.write(0xBE);

	for (int i = 0; i < 9; i++)
	{
		data[i] = ds.read();
	}

	int raw = (data[1] << 8) + data[0];

	if (data[7] == 0x10)
	{
		raw = (raw & 0xFFF0) + 12 - data[6];
	}

	return raw / 16.0;
}

// ########################################
// End temperature sensor logic
// ########################################

// ########################################
// Begin encoders logic
// ########################################

// Data for encoder pins
int encoderA = PIN_DT_A; // DT Pin
int encoderB = PIN_CLC_B; // CLC Pin
int encoderC = PIN_SW_C; // SW Pin

int encoderAVal;
int encoderBVal;
int encoderAPrev = 0;
int encoderCVal = 0;
int encoderCPrev = 0;

void encoderInit()
{
	// Start Encoder
	pinMode(encoderA, INPUT);
	pinMode(encoderB, INPUT);
	pinMode(encoderC, INPUT);
}

bool encoderRead()
{
	// read encoder pins
	encoderAVal = digitalRead(encoderA);
	encoderBVal = digitalRead(encoderB);
	encoderCVal = digitalRead(encoderC);

	if((!encoderAVal) && (encoderAPrev)) // if roll state canged
	{
		if(encoderBVal)
		{
			Serial.println("Roll to right");
			defineTemp += 1;
		}
		else
		{
			Serial.println("Roll to left");
			defineTemp -= 1;
		}

		if (defineTemp < 0)
		{
			defineTemp = 0;
		}

		if (defineTemp > 110)
		{
			defineTemp = 110;
		}
		screenUpdate = true;
	}
	encoderAPrev = encoderAVal;  // save new value

	if ((encoderCVal != encoderCPrev) && (encoderCVal == LOW)) // if button click
	{
		if (mode == MODE_HOT)
		{
			Serial.println("Switch mode to cold");
			mode = MODE_COLD;
		}
		else
		{
			Serial.println("Switch mode to hot");
			mode = MODE_HOT;
		}
		screenUpdate = true;
	}
	encoderCPrev = encoderCVal; // save new value
}

// ########################################
// End encoders logic
// ########################################

// ########################################
// Begin system logic
// ########################################

int relayPin = 2;

void systemInit()
{
	pinMode(relayPin, OUTPUT);
}

void systemUpdate()
{
	enable = false;
	if ((mode == MODE_HOT) && ((defineTemp - DEVIATION) > sensorTemp))
	{
		enable = true;
	}
	if ((mode == MODE_COLD) && ((defineTemp + DEVIATION) < sensorTemp))
	{
		enable = true;
	}

	if (enable)
	{
		digitalWrite(relayPin, HIGH);

	}
	else
	{
		digitalWrite(relayPin, LOW);
	}
}

// ########################################
// End system logic
// ########################################
