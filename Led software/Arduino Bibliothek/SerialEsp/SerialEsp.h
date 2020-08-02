#define SerialEsp_h
#include "Arduino.h"
class SerialEsp
{
public:
	SerialEsp();
	int Buffer[10];
	void lesewertsub();
	void LeseZahlen();
	int Errechnewert();
	void Bufferreset();
	int Errechneterwert ;
	int Buchstabe;
	
	
private: int Signallange;

	
};
