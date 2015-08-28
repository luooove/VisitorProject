#include <avr/sleep.h>
#include <AT24CXXX.h>
#include <Wire.h>
AT24CXXX ROM_512;
#include "DS3231.h"
DS3231 RTC; //Create the DS3231 object
const int theDeviceAddress = 0x57;
#include "OLED.h"

char Director1 = 0;
int VisitorsIn = 0;
int VisitorsOut = 0;
int count = 0;
char flag = 0;

void setup()
{
  Wire.begin();        //启动I2C通信

  LEDPIN_Init();
  LED_Init();
  LED_P6x8Str(20, 0, "Vistor_Flow_Rate:");
  LED_P6x8Str(0, 2, "PeopleIn:");
  LED_P6x8Str(0, 4, "PeopleOut:");

  ROM_512.Memory_Size(512);
  Serial.begin(9600);

  attachInterrupt(0, Detect1, FALLING  );//下降沿说明有
  attachInterrupt(1, Detect2, FALLING );//下降沿说明人
}

void Detect1()
{
	if (Director1 == 0) //means people is go in and not yet pass
	{
		Director1 = 1;
		Serial.println("detect1_0");
	}
  if (Director1 == 2) //means people is going out
  {
  	Director1 = 0;
  	VisitorsOut ++;
  	count++;
  	flag = 1;
  }
}
void Detect2()
{
	if (Director1 == 0) //means people is go out and not yet pass
	{
		Director1 = 2;
		Serial.println("detect2_0");
	}
	if (Director1 == 1)
	{
		Director1 = 0;
		VisitorsIn ++;
		count++;
		flag = 2;
	}
}



void SaveData()
{
	byte buffer[10];
  DateTime now = RTC.now(); //get the current date-time
  buffer[0] = now.year() - 2000;
  buffer[1] = now.month();
  buffer[2] = now.date();
  buffer[3] = now.hour();
  buffer[4] = now.minute();
  buffer[5] = now.second();
  buffer[6] = VisitorsIn % 255;
  buffer[7] = (VisitorsIn >> 8) % 255; //

  buffer[8] = VisitorsOut % 255;
  buffer[9] = (VisitorsOut >> 8) % 255;
  //save count
  ROM_512.WriteByte(0x57 , 0 , (byte)count % 255);
  ROM_512.WriteByte(0x57 , 1 , (byte)(count >> 8) % 255);

  ROM_512.WriteByte(0x57 , 2 + count * 10 , buffer[0]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 1 , buffer[1]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 2 , buffer[2]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 3 , buffer[3]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 4 , buffer[4]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 5 , buffer[5]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 6 , buffer[6]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 7 , buffer[7]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 8 , buffer[8]);
  ROM_512.WriteByte(0x57 , 2 + count * 10 + 9 , buffer[9]);



  LED_PrintValueI(60, 2, VisitorsIn);
  LED_PrintValueI(60, 4, VisitorsOut);
}


void printSaveData()
{	byte buffer[10];

	buffer[0] = ROM_512.ReadByte(0x57 ,2 + count * 10);
	buffer[1] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 1);
	buffer[2] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 2);
	buffer[3] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 3);
	buffer[4] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 4);
	buffer[5] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 5);

	buffer[6] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 6);
	buffer[7] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 7);

	buffer[8] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 8);
	buffer[9] = ROM_512.ReadByte(0x57 ,2 + count * 10 + 9);

	Serial.println((ROM_512.ReadByte(0x57 ,0))+((ROM_512.ReadByte(0x57 ,1))<<8),DEC);

    Serial.print(2000 + buffer[0], DEC);  //year
    Serial.print('-');
    Serial.print(buffer[1], DEC);//month
    Serial.print('-');
    Serial.print(buffer[2], DEC);//day
    Serial.print('/');

    Serial.print(buffer[3], DEC);//hour
    Serial.print(':');
    Serial.print(buffer[4], DEC);//min
    Serial.print(':');
    Serial.print(buffer[5], DEC);//sec
    Serial.print('#');

    int temp = buffer[6] + buffer[7] * 255;
    Serial.print(temp, DEC);
    Serial.print('/');
    int temp2 = buffer[8] + buffer[9] * 255;
    Serial.print(temp2, DEC);
    Serial.println();

}
void loop()
{

	switch (flag) {
		case 1:

      // do something
      SaveData();
      printSaveData();
      flag = 0;
      break;
      case 2:
      // do something
      SaveData();
      printSaveData();
      flag = 0;
      break;
      default:;
      // do something
  }
}