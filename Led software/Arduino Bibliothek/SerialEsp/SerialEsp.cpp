
#include "SerialEsp.h"
#include "Arduino.h"
int Buffer[10];
int Signallange = 0;
int Errechneterwert = 0;
int Buchstabe = 0;


SerialEsp::SerialEsp()
{
	
	Serial.println("Programm gestartet");
}
void SerialEsp::lesewertsub()
{
	delay(10);
	int Signal = Serial.read() - 48;
//	Serial.print("Ankommende zahlen:");Serial.println(Signal);
	delay(10);
	
	if (Signal == Serial.available())
	{
		Buchstabe = Serial.read();
		//Serial.print("Buchstabe: "); Serial.println(Buchstabe);
		LeseZahlen();
		Errechneterwert = Errechnewert();
		Bufferreset();
	}
	else
	{
		while (Serial.available() != 0)
		{
			Serial.println(Serial.read());
		}
		delay(1);
		Serial.print("~");
		delay(1);
	}
}
void SerialEsp::LeseZahlen()
{

	Signallange = 0;int num1 = 0;
	while (Serial.available() != 0)
	{
		num1 = Serial.read();	//	Serial.print("Lesezahl:"); Serial.println(num1);
//		Serial.print("Signallaenge: "); Serial.println(Signallange);
		Buffer[Signallange] = num1;
	//	Serial.print("Gespeichert in Buffer:");Serial.println(Buffer[Signallange]);
		
		Signallange = Signallange +1;
	}
	
}
int SerialEsp::Errechnewert()
{
	int	Wert = 0;
	int multi = 1;
	while (Signallange != 0)
	{
		if ((Buffer[Signallange - 1] - 48) != 0)
		{
			Wert += (Buffer[Signallange - 1] - 48) * multi;
		}
		Signallange = Signallange-1;
		multi = multi * 10;
	}

//	Serial.print("Wert: "); Serial.println(Wert);
	return Wert;
}
void SerialEsp::Bufferreset()
{
	for (int i = 0; i < 10; i++)
	{
	//	Serial.print("Bufferreset:");
		//Serial.println(Buffer[i]);
		Buffer[i] = 0;
		
	}
}
