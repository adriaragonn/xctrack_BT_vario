#include <BMP280_DEV.h>
#include <Device.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>

#define DELAYMSEC 100

#define DEBUGMODE   1

/////////////////////////////
#define RX 11 // not used (TX module bluetooth)
#define TX 12 // MISO on ISCP (RX module bluetooth)
SoftwareSerial BTserial(RX, TX); // RX not connected
#define USB_SPEED 115200
#define BT_SPEED 9600
#define FREQUENCY 10
#define USB_MODE
#define BT_MODE
#define USE_LK8000
////////////////////////

bool bBMP280init = false;
BMP280_DEV bmp280;

const char compile_date[] = "XCTRACK VARIO " __DATE__;
const char mess_check[] = "checking BMP280 sensor...";
const char mess_error[] = "Error connecting BMP280...";
const char mess_reset[] = "Reset in 1s...";

//////////////////////////

void setup() {
  wdt_disable();
  delay(10);
#ifdef USB_MODE
  Serial.begin(USB_SPEED);
  Serial.setTimeout(DELAYMSEC);
#endif
#ifdef BT_MODE
  BTserial.begin(BT_SPEED);
#endif
  wdt_enable(WDTO_1S);
  int iRet = bmp280.begin(BMP280_I2C_ALT_ADDR);

  delay(200);

  if (iRet == 0) {
#ifdef USB_MODE
    Serial.println(mess_error);
    Serial.println(mess_reset);
#endif
#ifdef BT_mode
    BTserial.println(mess_error);
    BTserial.println(mess_reset);
#endif
    delay(1200);
  } else {
    bBMP280init = true;
    bmp280.setTimeStandby(TIME_STANDBY_125MS);
    bmp280.startNormalConversion();

  }
}

uint32_t get_time = millis();
uint32_t sum = 0;
uint8_t n = 0;
float pressure_1;
unsigned int checksum_end;
float pressure;
unsigned int CheckSum (String);


void loop() {
  unsigned int checksum_end;

  if (bBMP280init)
  {
      if (bmp280.getPressure(pressure))    // Check if the measurement is complete
      {
          // formatto la stringa con protocollo LNMEA LK8EX1
          String str_out = String("LK8EX1,")+String((unsigned long)(pressure*100))+String(",99999,9999,99,999,");
          
          checksum_end = CheckSum ( str_out );

          // send the NMEA string, including checksum and CRLF
          #ifdef USB_MODE
             Serial.print("$"+str_out+"*");       
             Serial.println(checksum_end, HEX);
          #endif
          #ifdef BT_MODE
             BTserial.print("$"+str_out+"*");
             BTserial.println(checksum_end, HEX);
          #endif
      }
   }   

}

unsigned int CheckSum (String csString)
{
  unsigned int checksum_end, ai, bi;

  for (checksum_end = 0, ai = 0; ai < csString.length(); ai++)
  {
    bi = (unsigned char)csString[ai];
    checksum_end ^= bi;
  }
  return checksum_end;
}
