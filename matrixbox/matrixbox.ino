#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <iarduino_RTC.h>

#define PIN 2
iarduino_RTC time(RTC_DS1302, 8, 6, 7); // RST, CLK, DAT

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

#include <SdFat.h>

const uint8_t SD_CHIP_SELECT = 10;
SdFat  sd;

#include <Rotary.h>
Rotary r = Rotary(4, 3);

int maxDelay = 5000;
int setTimeDelay = maxDelay;

void setup() {
  Serial.begin(9600);
  if (!sd.begin(SD_CHIP_SELECT)) sd.initErrorHalt();

  Serial.println("SD Card Loaded");

  time.begin();

  matrix.begin();
  matrix.setBrightness(40);
  matrix.setTextWrap(false);
}

void loop() {
  if (setTimeDelay > 0) {
    unsigned char result = r.process();
    if (result) {
      setTimeDelay = maxDelay;
      time.gettime();
      int hour = time.Hours;
      int minute = time.minutes;
      if (result == DIR_CW) {
        if (minute == 59) {
          minute = 0;
          if (hour == 23) {
            hour = 0; 
          } else {
            hour += 1;
          }
        } else {
          minute += 1;
        }
      } else {
        if (minute == 00) {
          minute = 59;
          if (hour == 0) {
            hour = 23; 
          } else {
            hour -= 1;
          }
        } else {
          minute -= 1;
        }
      }
      time.settime(0, minute, hour);
    }
    if (setTimeDelay == maxDelay) { 
      drawTime();
    }
    setTimeDelay -= 1;
    delay(1);
  } else {
    loadImages();
  }
}

void loadImages() {
  sd.vwd()->rewind();
  SdFile file;

  while (file.openNext(sd.vwd(), O_READ)) {
    file.printName(&Serial);
    if (file.isDir() || file.fileSize() == 4096) {
      file.close();
      continue;
    }
    int frameSize = drawFile(file);
    if (frameSize < 5) {
      file.rewind();
      drawFile(file);
      file.rewind();
      drawFile(file);
      file.close();
    } else {
      file.close();
    }
    drawTime();
    delay(1000);
  }
}

int drawFile(SdFile file) {
  int delayValue = 200;
  char buffer[64];
  int frameSize = 0;

  while (file.fgets(buffer, sizeof(buffer)) > 0) {
    // start parse line
    if (buffer[0] == 'd') {
      // delay line
      strtok(buffer,":");
      delayValue = atoi(strtok (NULL,"x")) * 10;
      // Serial.print("Delay: "); Serial.println(delayValue, DEC);
    } else if (buffer[0] == '#') {
      // next frame
      // Serial.println("Next frame");
      frameSize += 1;
      matrix.show();
      // Serial.print("Delay: "); Serial.println(delayValue, DEC);
      delay(delayValue);
    } else {
      int x = atoi(strtok (buffer,","));
      int y = atoi(strtok (NULL,":"));
      strtok(NULL,"#");
      int r, g, b;
      sscanf(strtok(NULL," "), "%02x%02x%02x", &r, &g, &b);
      matrix.drawPixel(x, y, matrix.Color(r, g, b)); 
    }
    // end parse line
  }
  return frameSize;
}

const uint16_t colors[] = { matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void drawTime() {
  matrix.fillScreen(0);
  matrix.setTextColor(colors[random(0, 3)]);
  matrix.setCursor(1, 1);
  matrix.print(time.gettime("H"));
  matrix.setTextColor(colors[random(0, 2)]);
  matrix.setCursor(4, 9);
  matrix.print(time.gettime("i"));
  matrix.show();
}

