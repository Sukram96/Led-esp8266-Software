
#include <SerialEsp.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "SPIFFS.h"
#include "time.h"

bool Accespointmodus = true; //Wenn true, dann startet der Esp sein eigenes Netzwerk
const char* ssidAP = "Fast & Furious"; //Name von Accespoint
const char* passwordAP = "987654321A";  // Passwort für den Accespoint
const char* ssid = "--"; //Heimnetzwerk
const char* password = "--";
WiFiServer server(80);
const char* ntpServer = "pool.ntp.org"; //Internetadresse woher die Internetzeit ermittelt wird
String IPAdresse = ""; //Wird gesetzt, wenn man mit dem Heimnetzwerk verbunden ist
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

String Versionnummer = "8.5.1 ESP32";
#define PIN         5
#define BassPIN		34
#define HardwarePINPlusModus	23
#define HardwarePINMinusModus	22
#define FORMAT_SPIFFS_IF_FAILED false

int Tastenmodus = 0; //Speichert den aktuellen tastenmodus auf einer Website
int Temporaererwert = 0;
int pixelposition = 0; //dient zum einstellen der einzelnen Pixel auf der Website
String sResponse, sHeader;
bool sliderbool = false;
int NUMPIXELS = 197; //Anzahl der Led Adressen
short Hardwaremodus = 0; //Beinhalt den Modus, welcher durch die digitale Pins verändert werden kann
int FARBEROT = 30;
int FARBEBLAU = 30;
int FARBEGRUEN = 10;
int Modus = 1;
int Warte = 0;
int Vor = 1;
int Hinten = 0;
int Bassfilter = 2;
int Randomwert = 10;
short  Farbanzahl = 5;
int Bass = 1;
int Analogaufloesung = 10;
uint Schlafdauer = 10; //Gibt an wie lange der ESP in Minuten in Deepsleep schlafen gehen soll
//zeigt mehr daten auf der serrielen schnittstelle an
bool Debugmodus = false;
bool Debugmodus_Internet = false;
bool Startecounter = false;
//Ersetzt den analogen wert durch die Variable Bass
bool Ersetzebass = false;
// Modus = "schnell" es wird nicht kontrolliert, ob bei der übertragung zeichen fehlen
// Modus = "sicher" es wird die Serial.Readstring methode verwerned, dauert dafür ewig um daten einzulesen
// Modus = "kontrolle" die erste zahl gibt an, wie viel ziffern folgen, wenn diese zahlen nicht gleich sind, ist dies nicht der fall, wird ein ~ gesendet(Nötig Bei Atmega chips!)
String LeseSerialModus = "sub";
bool submodus = false;

//Led startanfang 
short Startanfang = 0;
short Zwischenspeicher[200];
double zwischenrot = 0;
double zwischenblau = 0;
double zwischengruen = 0;

//arrays f�r die ledwerte, welche mit der methode Arraylicht() auf die Led geschrieben wird
short einzelnrot[200];
short einzelnblau[200];
short einzelngruen[200];

short arraygruen[200];
short arrayrot[200];
short arrayblau[200];
double GenauePosition[200];
short Position[200];
//wird für zwischenwerte ben�tigt
short Bufferlocal[200];
double Geschwindigkeit[200];

//Laufzeit von Esp
short Einschlafzeit = 23;
short Aufwachzeit = 7;
unsigned long Zeit = 0;
unsigned short Sekunden = 0;
unsigned short Minuten = 0;
unsigned short Stunden = 0;
unsigned long Schlafenszeit = 3600000 * 5; // 5 Stunde, wann soll der ESP einschlafen, abhängig von der laufzeit
unsigned long Tastendruck = 0;
unsigned long Zeitabrufen = 0; // jede Stunde soll die Zeit aktuelisiert werden
//zeit, wann des n�chste programm starten soll
unsigned long Programmzeit = 1000;

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_BRG + NEO_KHZ800);  //ws2812
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400); //WS2811
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);
SerialEsp Daten = SerialEsp();
TaskHandle_t task_loop1;

void esploop1(void* pvParameters)
{
	setup1();
	for (;;)
		loop1();
}

void setup()
{
	xTaskCreatePinnedToCore( //Starte den zweiten Core
		esploop1,
		"loop1",
		10000,
		NULL,
		1,
		&task_loop1,
		!CONFIG_ARDUINO_RUNNING_CORE);
	//Ladeparameter();
	pixels.begin();
	if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
	{
		Serial.println("SPIFFS Mount Failed");
	}
	pinMode(HardwarePINPlusModus, INPUT_PULLUP);
	pinMode(HardwarePINMinusModus, INPUT_PULLUP);
	attachInterrupt(HardwarePINPlusModus, InterruptModuserhoehen, FALLING);
	attachInterrupt(HardwarePINMinusModus, InterruptModusverkleinern, FALLING);
	analogReadResolution(Analogaufloesung);
	Serial.begin(9600);
	Position[0] = 0;
	Bufferlocal[0] = 0;
	arraygruen[0] = 250;
	Position[1] = NUMPIXELS / 2;
	Bufferlocal[1] = 0;
	arrayblau[1] = 250;
	Einschlagberechnen();
	while (arraygruen[0] != 0)
	{
		Einschlagberechnen();

	}
}
void setup1()
{
	if (!Accespointmodus)
	{
		Serial.print("Connecting to ");
		Serial.println(ssid);
		WiFi.begin(ssid, password);
		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.print(".");
		}
		Serial.println("");
		Serial.println("WiFi connected.");
		IPAdresse = WiFi.localIP().toString();
		Serial.println(IPAdresse);
		configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
		printLocalTime();
		server.begin();
	}
	if (Accespointmodus)
	{
		WiFi.mode(WIFI_AP);
		WiFi.softAP(ssidAP, passwordAP);
		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		IPAdresse = myIP.toString();
		Serial.println(IPAdresse);
		server.begin();
	}
}
void loop1()
{
	delay(5);
	internet();
	if (millis() > Schlafenszeit)
	{
		Controllerausschalten(Schlafdauer);
	}
	if (millis() > Zeitabrufen)
	{
		printLocalTime();
		Zeitabrufen = 3600000 * 1 + millis(); //Jede Stunde soll die Zeit aktuelisiert werden
		Berechneschlafzeit();
	}
}
void loop()
{
	delay(1);
	if (Serial.available() != 0) { aktualisierewerte(); }
	switch (Modus)
	{
	case 0: Modus0(); break;
	case 1: Modus1(); break;
	case 2: Modus2(); break;
	case 3: Modus3(); break;
	case 4: Modus4(); break;
	case 5: Modus5(); break;
	case 6: Modus6(); break;
	case 7: Modus7(); break;
	case 8: Modus8(); break;
	case 9: Modus9(); break;
	case 10: Modus10(); break;
	case 11: Modus11(); break;
	case 12: Modus12(); break;
	case 13: Modus13(); break;
	case 14: Modus14(); break;
	case 15: Modus15(); break;
	case 16: Modus16(); break;
	case 17: Modus17(); break;
	case 18: Modus18(); break;
	case 19: Modus19(); break;
	case 20: Modus20(); break;
	case 21: Modus21(); break;
	case 22: Modus22(); break;
	case 23: Modus23(); break;
	case 24: Modus24(); break;
	case 25: Modus25(); break;
	case 26: Modus26(); break;
	case 27: Modus27(); break;
	case 28: Modus28(); break;
	case 29: Modus29(); break;
	case 30: Modus30(); break;
	case 31: Modus31(); break;
	case 32: Modus32(); break;
	case 33: Modus33(); break;
	case 34: Modus34(); break;
	case 35: Modus35(); break;
	case 36: Modus36(); break;
	case 37: Modus37(); break;
	case 38: Modus38(); break;
	case 39: Modus39(); break;
	case 40: Modus40(); break;
	case 41: Modus41(); break;
	case 42: Modus42(); break;
	case 43: Modus43(); break;
	case 44: Modus44(); break;
	case 45: Modus45(); break;
	case 46: Modus46(); break;
	case 100: Modus100(); break;
	case 101: LedKetteMinus(); Arraylicht(); delay(30); break; //verdunkelt die led langsam
	case 102: Modus102(); break;
	case 103: Modus103(); break;
	case 200: LedArrayauflisten(); break;
	case 201: LEDARRAYReset(); break;
	case 202: Arrayfarbenauflisten();
	case 203: Ledarrayausbuffer(); Modus = 100; break;
	case 204:Controllerausschalten(5); break;
	case 205: Zeigezwischenwerte();
	case 1000:
		Serial.println("D22: " + String(digitalRead(22))) + ";";//funktioniert links, normal ist der wert 1 
		Serial.println("D23: " + String(digitalRead(23))) + ";"; //funktioniert Rechts, normal ist der Wert 1
		delay(1000);
	default:
		Serial.println("Wartemodus");
		delay(1000);
		break;
	}
}
void Modus0()
{
	Lichtersofort(FARBEROT, FARBEBLAU, FARBEGRUEN);
}
void Modus1()
{
	Lichter(FARBEROT, FARBEBLAU, FARBEGRUEN, Warte, 0, 200);
}
void Modus2()
{
	Lichter(FARBEROT, FARBEBLAU, FARBEGRUEN, Warte, 0, 200);
	Lichter(0, 0, 0, Warte, 0, 200);
}
void Modus3()
{
	while (Modus == 3)
	{
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
		Lichtersofort(FARBEROT, FARBEBLAU, FARBEGRUEN);
		delay(Warte);
		Lichtersofort(0, 0, 0);
		delay(Warte);
	}
}
void Modus4()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 100);
	for (int i = 0; i < 100; i++)
	{
		Lichtersofort(zwischenrot * i, zwischenblau * i, zwischengruen * i);
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
	for (int i = 100; i != 1; i--)
	{
		Lichtersofort(zwischenrot * i, zwischenblau * i, zwischengruen * i);
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus5()
{
	Lichthinzufuegen(0, FARBEROT, FARBEBLAU, FARBEGRUEN);
	Streifen(true);
}
void Modus6()
{
	for (int a = 1; a < Farbanzahl + 1; a++)
	{
		Position[a] = (NUMPIXELS / Farbanzahl) * a;
	}
	for (int i = 0; i < Farbanzahl; i++)
	{
		Zwischenspeicher[i] = random(NUMPIXELS);
	}
	for (int i = 0; i < Farbanzahl; i++)
	{
		einzelnrot[i] = random(Randomwert);
		einzelnblau[i] = random(Randomwert);
		einzelngruen[i] = random(Randomwert);
	}
	while (Modus == 6)
	{
		for (int i = 0; i < NUMPIXELS; i++)
		{
			for (int e = 0; e < Farbanzahl; e++)
			{
				if (Zwischenspeicher[e] == i)
				{
					Zwischenspeicher[e] = random(NUMPIXELS);
					for (int b = 0; b < Farbanzahl; b++)
					{
						einzelnrot[b] = random(Randomwert);
						einzelnblau[b] = random(Randomwert);
						einzelngruen[b] = random(Randomwert);
					}
				}
			}
			for (int a = 0; a != Farbanzahl; a++)
			{
				pixels.setPixelColor(Position[a] + Startanfang, pixels.Color(einzelnrot[a], einzelnblau[a], einzelngruen[a]));
			}
			pixels.show();
			delay(Warte);
			for (int e = 0; e < Farbanzahl; e++)
			{
				Position[e]++;
				if (Position[e] == NUMPIXELS)
				{
					Position[e] = 0;
				}
			}
			if (Serial.available() != 0)
			{
				aktualisierewerte();
			}
		}
	}
}
void Modus7()
{
	while (Modus == 7)
	{
		Hinten = random(Randomwert);
		Vor = random(Randomwert);
		int a = random(NUMPIXELS);
		einzelnrot[a] = random(Randomwert);
		einzelnblau[a] = random(Randomwert);
		einzelngruen[a] = random(Randomwert);
		Streifen(true);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			if (Modus != 7)
			{
				break;
			}
		}
	}
}
void Modus8()
{
	while (Modus == 8)
	{
		Bass = Lesenalogpin(BassPIN);
		if (Bass >= 255)
		{
			Lichtersofort(0, 0, 250);
		}
		if (Bass > 400)
		{
			Lichtersofort(250, 0, Bass);
		}
		if (Bass > 600)
		{
			Lichtersofort(0, 250, 0);
		}
		if (Bass < 250)
		{
			Lichtersofort(Bass, Bass, Bass);
		}
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
		yield();
	}
}
void Modus9()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
	while (Modus == 9)
	{
		Bass = Lesenalogpin(BassPIN);
		Lichtersofort(Bass * zwischenrot, Bass * zwischenblau, Bass * zwischengruen);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
		}
		yield();
	}
}
void Modus10()
{
	while (Modus == 10)
	{
		for (int i = 0; i < NUMPIXELS; i++)
		{
			if (i < NUMPIXELS / 2)
			{
				pixels.setPixelColor(i + Startanfang, pixels.Color(250, 0, 0));
			}
			else
			{
				pixels.setPixelColor(i + Startanfang, pixels.Color(0, 250, 0));
			}
		}
		pixels.show();
		delay(Warte);
		for (int i = 0; i < NUMPIXELS; i++)
		{
			if (i < NUMPIXELS / 2)
				pixels.setPixelColor(i + Startanfang, pixels.Color(0, 250, 0));
			else
				pixels.setPixelColor(i + Startanfang, pixels.Color(250, 0, 0));
		}
		pixels.show();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus11()
{
	zwischenblau = 0; zwischengruen = 0; zwischenrot = 0;
	while (Modus == 11)
	{
		if (random(2))
		{
			for (int i = 0; i < random(255); i++)
			{
				switch (random(3))
				{
				case 0: zwischenblau++; if (zwischenblau > 250) zwischenblau = 250; break;
				case 1: zwischenrot++; if (zwischenrot > 250) zwischenrot = 250; break;
				case 2: zwischengruen++; if (zwischengruen > 250) zwischengruen = 250; break;
				default:break;
				}
				//delay(Warte);
				Lichthinzufuegen(1, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);
				if (Serial.available() != 0)
				{
					aktualisierewerte();
				}
			}
		}
		else
		{
			for (int i = 0; i < random(255); i++)
			{
				switch (random(3))
				{
				case 0: zwischenblau--; if (zwischenblau < 0) zwischenblau = 0; break;
				case 1: zwischenrot--; if (zwischenrot < 0) zwischenrot = 0; break;
				case 2: zwischengruen--; if (zwischengruen < 0) zwischengruen = 0; break;
				default:break;
				}
				//	delay(Warte);
				Lichthinzufuegen(1, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);
				if (Serial.available() != 0)
				{
					aktualisierewerte();
				}
			}
		}
	}
}
void Modus12()
{
	while (Modus == 12)
	{
		Bass = Lesenalogpin(BassPIN);
		for (int a = 0; a != NUMPIXELS; a++)
		{
			if (Bass > a)
			{
				pixels.setPixelColor(a + Startanfang, pixels.Color(FARBEROT, FARBEBLAU, FARBEGRUEN));
			}
			else
			{
				pixels.setPixelColor(a + Startanfang, pixels.Color(0, 0, 0));
			}
		}
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
		yield();
		pixels.show();
	}
}
void Modus13()
{
	while (Modus == 13)
	{
		Bass = analogRead(BassPIN);
		zwischengruen = 200;
		zwischenrot = 0;
		for (int a = 0; a != NUMPIXELS; a++)
		{
			if (Bass > a)
			{
				if (a < NUMPIXELS / 2)
				{
					zwischenrot = zwischenrot + 2;
				}
				else
				{
					zwischengruen = zwischengruen - 2;
				}
				pixels.setPixelColor(a + Startanfang, pixels.Color(zwischenrot, 0, zwischengruen));
			}
			else
			{
				pixels.setPixelColor(a + Startanfang, pixels.Color(0, 0, 0));
			}
		}
		pixels.show();
		yield();
		if (Debugmodus) Serial.println(Bass);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus14()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, NUMPIXELS);
	for (int i = 0; i < NUMPIXELS; i++)
	{
		Lichthinzufuegen(i, zwischenrot * i, zwischenblau * i, zwischengruen * i);
	}
	while (Modus == 14)
	{
		Streifen(true);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			LEDARRAYReset();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, NUMPIXELS);
			for (int i = 0; i < NUMPIXELS; i++)
			{
				Lichthinzufuegen(i, zwischenrot * i, zwischenblau * i, zwischengruen * i);
			}
		}
	}
}

void Modus15()
{

	int Abstand = NUMPIXELS / 3;
	if (Debugmodus)
		Serial.println("Abstand: " + (String)Abstand);
	int rest = NUMPIXELS % 3;
	Errechnezwischenwerte(250, 250, 250, Abstand);
	for (int i = 0; i < Abstand; i++)
	{
		Lichthinzufuegen(i, 250 - (zwischenrot * i), 0, 0);
		Lichthinzufuegen(i + Abstand, 0, 250 - (zwischenblau * i), 0);
		Lichthinzufuegen(i + Abstand * 2, 0, 0, 250 - (zwischengruen * i));

	}

	for (int i = 0; i < Abstand && i != NUMPIXELS; i++)
	{
		einzelngruen[i + Abstand] = zwischengruen * i;
		einzelnrot[i + Abstand * 2] = zwischenrot * i;
		einzelnblau[i] = zwischenblau * i;
		Arraylicht();
		delay(30);
	}

	while (Modus == 15)
	{
		Streifen(true);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus16()
{
	LEDARRAYReset();
	zwischenblau = 0; zwischengruen = 0; zwischenrot = 0;
	if (Randomwert <= 1) Randomwert = 2;
	while (Modus == 16)
	{
		//bestimmt ob die Farbe dunkler oder heller wird
		if (random(2))
		{
			for (int i = 0; i < random(Randomwert); i++)
			{
				if (Serial.available() != 0)
				{
					aktualisierewerte();
				}
				switch (random(3))
				{
				case 0: zwischenblau++; if (zwischenblau > FARBEBLAU)  zwischenblau = FARBEBLAU; break;
				case 1: zwischenrot++; if (zwischenrot > FARBEROT) zwischenrot = FARBEROT; break;
				case 2: zwischengruen++; if (zwischengruen > FARBEGRUEN) zwischengruen = FARBEGRUEN; break;
				default:	break;
				}
				Lichthinzufuegen(0, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);
			}
		}
		else
		{
			for (int i = 0; i < random(Randomwert); i++)
			{
				if (Serial.available() != 0)
				{
					aktualisierewerte();
					if (Randomwert <= 1) Randomwert = 2;
				}
				switch (random(3))
				{
				case 0: zwischenblau--; if (zwischenblau < 0) zwischenblau = 0; break;
				case 1: zwischenrot--; if (zwischenrot < 0) zwischenrot = 0; break;
				case 2: zwischengruen--; if (zwischengruen < 0) zwischengruen = 0; break;
				default:
					break;
				}
				delay(Warte);
				Lichthinzufuegen(0, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);
			}
		}
	}
}
void Modus17()
{
	if (Warte == 0)Warte = 1;
	for (int i = 0; i < Farbanzahl; i++)
	{
		Zwischenspeicher[i] = 0;
	}
	int Abstand = NUMPIXELS / Farbanzahl;
	while (Modus == 17)
	{
		for (int e = 0; e < Farbanzahl; e++)
		{
			Zwischenspeicher[random(Farbanzahl)] = 1;
			Zwischenspeicher[random(Farbanzahl)] = 0;
		}
		for (int i = 0; i < Farbanzahl; i++)
		{
			if (Zwischenspeicher[i] == 1)
				for (int a = 0; a < Abstand; a++)
				{
					Lichthinzufuegen(a + i * Abstand, FARBEROT, FARBEBLAU, FARBEGRUEN);
				}
			else
				for (int a = 0; a < Abstand; a++)
				{
					Lichthinzufuegen(a + i * Abstand, 0, 0, 0);
				}
		}
		Arraylicht();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Abstand = NUMPIXELS / Farbanzahl;
			if (Warte == 0)Warte = 1;
		}
	}
}

void Einschlagberechnen()
{
	for (int a = 0; a != Farbanzahl; a++)
	{
		if (arrayrot[a] > 0)
		{
			if (NUMPIXELS > Position[a] + Bufferlocal[a])
			{
				if (arrayrot[a] > einzelnrot[Position[a] + Bufferlocal[a]])
				{
					einzelnrot[Position[a] + Bufferlocal[a]] = arrayrot[a];
				}
			}
			if (0 < Position[a] - Bufferlocal[a])
			{
				if (arrayrot[a] > einzelnrot[Position[a] - Bufferlocal[a]])
				{
					einzelnrot[Position[a] - Bufferlocal[a]] = arrayrot[a];
				}
			}
			arrayrot[a]--;
		}


		if (arraygruen[a] > 0)
		{
			if (NUMPIXELS > Position[a] + Bufferlocal[a])
			{
				if (arraygruen[a] > einzelngruen[Position[a] + Bufferlocal[a]])
				{
					einzelngruen[Position[a] + Bufferlocal[a]] = arraygruen[a];
				}
			}
			if (0 < Position[a] - Bufferlocal[a])
			{
				if (arraygruen[a] > einzelngruen[Position[a] - Bufferlocal[a]])
				{
					einzelngruen[Position[a] - Bufferlocal[a]] = arraygruen[a];
				}
			}

			arraygruen[a]--;
		}

		if (arrayblau[a] > 0)
		{
			if (NUMPIXELS > Position[a] + Bufferlocal[a])
			{
				if (arrayblau[a] > einzelnblau[Position[a] + Bufferlocal[a]])
				{
					einzelnblau[Position[a] + Bufferlocal[a]] = arrayblau[a];
				}
			}
			if (0 < Position[a] - Bufferlocal[a])
			{
				if (arrayblau[a] > einzelnblau[Position[a] - Bufferlocal[a]])
				{
					einzelnblau[Position[a] - Bufferlocal[a]] = arrayblau[a];
				}

			}

			arrayblau[a]--;
		}

		{
			Bufferlocal[a]++;
			if (Bufferlocal[a] > 200)
			{
				Bufferlocal[a] = 0;
			}
		}
	}

	LedKetteMinus();
	Arraylicht();
}
void Einschlagberechneneinmalig(int Berrechnearray)
{
	int a = Berrechnearray;
	if (arrayrot[a] > 0)
	{
		if (NUMPIXELS > Position[a] + Bufferlocal[a])
		{
			if (arrayrot[a] > einzelnrot[Position[a] + Bufferlocal[a]])
			{
				einzelnrot[Position[a] + Bufferlocal[a]] = arrayrot[a];
			}
		}
		if (0 < Position[a] - Bufferlocal[a])
		{
			if (arrayrot[a] > einzelnrot[Position[a] - Bufferlocal[a]])
			{
				einzelnrot[Position[a] - Bufferlocal[a]] = arrayrot[a];
			}
		}
		arrayrot[a]--;
	}


	if (arraygruen[a] > 0)
	{
		if (NUMPIXELS > Position[a] + Bufferlocal[a])
		{
			if (arraygruen[a] > einzelngruen[Position[a] + Bufferlocal[a]])
			{
				einzelngruen[Position[a] + Bufferlocal[a]] = arraygruen[a];
			}
		}
		if (0 < Position[a] - Bufferlocal[a])
		{
			if (arraygruen[a] > einzelngruen[Position[a] - Bufferlocal[a]])
			{
				einzelngruen[Position[a] - Bufferlocal[a]] = arraygruen[a];
			}
		}

		arraygruen[a]--;
	}

	if (arrayblau[a] > 0)
	{
		if (NUMPIXELS > Position[a] + Bufferlocal[a])
		{
			if (arrayblau[a] > einzelnblau[Position[a] + Bufferlocal[a]])
			{
				einzelnblau[Position[a] + Bufferlocal[a]] = arrayblau[a];
			}
		}
		if (0 < Position[a] - Bufferlocal[a])
		{
			if (arrayblau[a] > einzelnblau[Position[a] - Bufferlocal[a]])
			{
				einzelnblau[Position[a] - Bufferlocal[a]] = arrayblau[a];
			}

		}

		arrayblau[a]--;
	}


	if (Geschwindigkeit[a] == 0) Bufferlocal[a]++;
	if (arrayblau[a] == 0 && arrayrot[a] == 0 && arraygruen[a] == 0)
	{
		Bufferlocal[a] = 0;
	}

}
void Modus18()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
	while (Modus == 18)
	{
		Einschlagberechnen();
		delay(Warte);
		for (int a = 0; a != Farbanzahl; a++)
		{
			if (arrayblau[a] <= 0 && arraygruen[a] <= 0 && arrayrot[a] <= 0)
			{
				Bufferlocal[a] = 0;
				Position[a] = random(0, NUMPIXELS);
				switch (random(0, 3))
				{
				case 0: arrayrot[a] = random(0, Lesenalogpin(BassPIN)) * zwischenrot; break;
				case 1: arraygruen[a] = random(0, Lesenalogpin(BassPIN)) * zwischengruen; break;
				case 2: arrayblau[a] = random(0, Lesenalogpin(BassPIN)) * zwischenblau; break;
				default:; break;

				}
			}
		}
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
		}
	}
}
void Modus19()
{
	//LEDARRAYReset();
	Lichthinzufuegen(0, 0, 250, 250);
	Lichthinzufuegen(100, 250, 250, 0);
	unsigned long zeit = millis();
	unsigned long zeitseitstart;
	while (Modus == 19)
	{
		zeitseitstart = millis();
		LedKetteVor(true, NUMPIXELS);
		if (Serial.available() != 0)
		{
			aktualisierewerte();

		}
		if (zeitseitstart > zeit + Warte)
		{
			zeit = millis();
			Arraylicht();
		}
		yield();
		delayMicroseconds(3000);
	}

}
void Modus20()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	short lange = 0;
	int zahler = 0;
	while (Modus == 20)
	{
		lange = random(Randomwert);
		for (int a = random(NUMPIXELS); a < NUMPIXELS; a++)
		{
			if (einzelnrot[a] <= 1 && einzelngruen[a] <= 1 && einzelnblau[a] <= 1)
			{
				int lichtstarke = random(Lesenalogpin(BassPIN));
				Lichthinzufuegen(a, zwischenrot * lichtstarke, zwischenblau * lichtstarke, zwischengruen * lichtstarke);
			}
			zahler++;
			if (zahler > lange) break;
		}
		zahler = 0;
		LedKetteMinus();
		Arraylicht();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
		}
		delay(Warte);
	}
}
void Modus21()
{

	if (Vor == 0)
	{
		Lichthinzufuegen(NUMPIXELS - 1, FARBEROT, FARBEBLAU, FARBEGRUEN);
		Streifen(false);
		Lichthinzufuegen(NUMPIXELS - 1, 0, 0, 0);
		Streifen(false);
	}
	else
	{
		Lichthinzufuegen(0, FARBEROT, FARBEBLAU, FARBEGRUEN);
		Streifen(false);
		Lichthinzufuegen(0, 0, 0, 0);
		Streifen(false);
	}
}
void Modus22()
{
	short Bass = Lesenalogpin(BassPIN);
	Lichthinzufuegen(0, Bass, Bass, Bass);
	Streifen(false);
}
void Modus23()
{
	short Bass = Lesenalogpin(BassPIN);
	Lichthinzufuegen(0, Bass, Bass, Bass);
	for (int i = 0; i < Vor; i++)
	{
		LedKetteplusverkleinern(false);
		Arraylicht();
		delay(Warte);
	}
}
void Modus24()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, NUMPIXELS);
	short Bass = 0;
	while (Modus == 24)
	{
		Bass = Lesenalogpin(BassPIN);
		Lichthinzufuegen(0, Bass * zwischenrot, Bass * zwischenblau, Bass * zwischengruen);
		Streifen(false);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, NUMPIXELS);
		}
	}
}
void Modus25()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, NUMPIXELS);
	short Bass = 0;
	while (Modus == 25)
	{
		Bass = Lesenalogpin(BassPIN);
		Lichthinzufuegen(0, Bass * zwischenrot, Bass * zwischenblau, Bass * zwischengruen);
		LedKetteplusverkleinern(false);
		Arraylicht();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, NUMPIXELS);
		}
	}
}
void Modus26()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	short Randomwert = 0;
	while (Modus == 26)
	{
		for (int a = 0; a < NUMPIXELS; a++)
		{
			if (einzelnrot[a] <= 1 && einzelngruen[a] <= 1 && einzelnblau[a] <= 1)
			{
				Randomwert = random(Lesenalogpin(BassPIN));
				Lichthinzufuegen(a, zwischenrot * Randomwert, zwischenblau * Randomwert, zwischengruen * Randomwert);
			}
		}
		LedKetteMinus();
		Arraylicht();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
		}
		delay(Warte);
	}
}
void Modus27()
{
	Position[0] = 0;
	short Randomwert = 0;
	short Abstaende = NUMPIXELS / Farbanzahl;

	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	while (Modus == 27)
	{
		Randomwert = random(Lesenalogpin(BassPIN));

		for (int a = 0; a < Farbanzahl; a++)
		{
			if (zwischenblau * Randomwert > einzelnblau[0])
			{
				Lichthinzufuegen(Position[0], (int)zwischenrot * Randomwert, (int)zwischenblau * Randomwert, (int)zwischengruen * Randomwert);
			}
			Position[0] = Position[0] + Abstaende;
			if (Position[0] >= NUMPIXELS)
			{
				Position[0] = Position[0] - NUMPIXELS;
			}

		}
		Position[0] = Position[0] + Vor;
		if (Position[0] >= NUMPIXELS)
		{
			Position[0] = Position[0] - NUMPIXELS;
		}
		Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);

		LedKetteplusverkleinern(true);
		Arraylicht();

		delay(Warte);
		if (Farbanzahl == 1) Abstaende = 0;
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
			Abstaende = NUMPIXELS / Farbanzahl;
			if (Farbanzahl == 1) Abstaende = 0;
		}
	}
}

void Modus28()
{
	short Random = 0;
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	short Abstand = NUMPIXELS / Farbanzahl;
	while (Modus == 28)
	{


		for (int i = 0; i < Farbanzahl; i++)
		{
			Random = random(Lesenalogpin(BassPIN));
			if (Random != 0)
			{
				Lichthinzufuegen(Abstand * i, zwischenrot * Random, zwischenblau * Random, zwischengruen * Random);
			}
		}
		LedKetteplusverkleinern(false);
		Arraylicht();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
			Abstand = NUMPIXELS / Farbanzahl;

		}
	}
}
void Modus29()
{
	int bass;
	for (int i = 0; i < Farbanzahl; i++)
	{
		GenauePosition[i] = random(NUMPIXELS);
		Geschwindigkeit[i] = random(-3, 3);
		if (Geschwindigkeit[i] == 0)
		{
			if (random(0, 2) == 1)Geschwindigkeit[i] = 1;
			else Geschwindigkeit[i] = -1;
		}
		arrayrot[i] = random(250);
		arraygruen[i] = random(250);
		arrayblau[i] = random(250);
	}
	arrayrot[0] = 250;
	arraygruen[0] = 0;
	arrayblau[0] = 0;
	arrayrot[1] = 0;
	arraygruen[1] = 250;
	arrayblau[1] = 0;
	arrayrot[2] = 0;
	arraygruen[2] = 0;
	arrayblau[2] = 250;
	while (Modus == 29)
	{
		for (int i = 0; i < Farbanzahl; i++)
		{
			einzelnblau[Position[i]] = 0;
			einzelngruen[Position[i]] = 0;
			einzelnrot[Position[i]] = 0;
		}
		bass = Lesenalogpin(BassPIN);
		for (int i = 0; i < Farbanzahl; i++)
		{
			Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;
			GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
			if (bass == 0)
			{

			}
			else
			{
				if (Geschwindigkeit[i] < 0)
				{
					if (-(Geschwindigkeit[i]) < bass * 0.002)
						Geschwindigkeit[i] = -((double)(random(80, 120) / 50) * (double)(bass * 0.002));
				}
				else
				{
					if (Geschwindigkeit[i] < bass * 0.002)
						Geschwindigkeit[i] = ((double)random(80, 120) / 50) * (double)(bass * 0.002);
				}
			}
			Position[i] = (int)GenauePosition[i];
			if (einzelnblau[Position[i]] <= arrayblau[i])
			{
				einzelnblau[Position[i]] = arrayblau[i];
			}
			if (einzelngruen[Position[i]] <= arraygruen[i])
			{
				einzelngruen[Position[i]] = arraygruen[i];
			}
			if (einzelnrot[Position[i]] <= arrayrot[i])
			{
				einzelnrot[Position[i]] = arrayrot[i];
			}
			if (Position[i] >= NUMPIXELS)
			{
				Position[i] = NUMPIXELS - 1; Geschwindigkeit[i] = -(Geschwindigkeit[i]);
				einzelnblau[0] = 0;
				einzelngruen[0] = 0;
				einzelnrot[0] = 0;
			}
			if (Position[i] <= 0)
			{
				Position[i] = 1; Geschwindigkeit[i] = -(Geschwindigkeit[i]);
				einzelnblau[0] = 0;
				einzelngruen[0] = 0;
				einzelnrot[0] = 0;
			}
		}
		delay(Warte);
		Arraylicht();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			for (int i = 0; i < Farbanzahl; i++)
			{
				if (Daten.Buchstabe == 110)
				{
					GenauePosition[i] = random(NUMPIXELS);
					Geschwindigkeit[i] = random(-3, 3);
					if (Geschwindigkeit[i] == 0)
					{
						if (random(0, 2) == 1)Geschwindigkeit[i] = 1;
						else Geschwindigkeit[i] = -1;
					}
					arrayrot[i] = random(250);
					arraygruen[i] = random(250);
					arrayblau[i] = random(250);
				}
			}
		}
	}
	Serial.println("Modus beeendet");

}
void Modus30()
{
	LEDARRAYReset();
	int bass;
	for (int i = 0; i < Farbanzahl; i++)
	{
		//	GenauePosition[i] = random(NUMPIXELS);
			//Geschwindigkeit[i] = random(-3, 3);
			//if (Geschwindigkeit[i] == 0)
		{
			//if (random(0, 2) == 1)Geschwindigkeit[i] = 1;
			//else Geschwindigkeit[i] = -1;
		}
		arrayrot[i] = random(250);
		arraygruen[i] = random(250);
		arrayblau[i] = random(250);
		while (Modus == 30)
		{
			LedKetteMinus();
			bass = Lesenalogpin(BassPIN);
			for (int i = 0; i < Farbanzahl; i++)
			{
				Position[i] = (int)GenauePosition[i];

				GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
				if (bass == 0)
				{
					Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;
				}
				else
				{
					if (Geschwindigkeit[i] < 0)
					{
						if (-(Geschwindigkeit[i]) < bass * 0.003)
						{
							Geschwindigkeit[i] = -((random(80, 120) / 50) * (bass * 0.003));
							if (Geschwindigkeit[i] > 1) Geschwindigkeit[i] = 1;
						}
					}
					else
					{
						if (Geschwindigkeit[i] < bass * 0.003)
						{
							Geschwindigkeit[i] = (random(80, 120) / 50) * (bass * 0.003);
							if (Geschwindigkeit[i] < -1) Geschwindigkeit[i] = -1;
						}

					}
				}
				if (arraygruen[i] == 0 && arrayrot[i] == 0 && arrayblau[i] == 0 && bass >= 5)
				{
					switch (random(3))
					{
					case 0: arrayrot[i] = random(250); break;
					case 1: arraygruen[i] = random(250); break;
					case 2: arrayblau[i] = random(250); break;


					default:
						break;
					}


					GenauePosition[i] = random(NUMPIXELS);
				}
				ArrayFarbeMinus(i);
				Position[i] = (int)GenauePosition[i];
				if (einzelnblau[Position[i]] == 0)
				{
					einzelnblau[Position[i]] = arrayblau[i];
				}
				else if (einzelnblau[Position[i]] < 2000)
				{
					einzelnblau[Position[i]] = (arrayblau[i] + einzelnblau[Position[i]]);
				}
				if (einzelngruen[Position[i]] == 0)
				{
					einzelngruen[Position[i]] = arraygruen[i];
				}
				else if (einzelngruen[Position[i]] < 2000)
				{
					einzelngruen[Position[i]] = (arraygruen[i] + einzelngruen[Position[i]]);
				}
				if (einzelnrot[Position[i]] == 0)
				{
					einzelnrot[Position[i]] = arrayrot[i];
				}
				else if (einzelnrot[Position[i]] < 2000)
				{
					einzelnrot[Position[i]] = (arrayrot[i] + einzelnrot[Position[i]]);
				}
				if (Position[i] == NUMPIXELS)
				{
					Position[i] = NUMPIXELS - 1; Geschwindigkeit[i] = -(Geschwindigkeit[i]);
				}

				if (Position[i] == 0)
				{
					Position[i] = 1; Geschwindigkeit[i] = -(Geschwindigkeit[i]);
				}
			}
			delay(Warte);
			Arraylicht();
			if (Serial.available() != 0)
			{
				aktualisierewerte();
			}
		}
	}
}
void Modus31()
{
	int bass;
	for (int i = 0; i < Farbanzahl; i++)
	{
		//GenauePosition[i] = random(NUMPIXELS);
		//Geschwindigkeit[i] = random(-3, 3);
		//if (Geschwindigkeit[i] == 0)
		{
			//if (random(0, 2) == 1)Geschwindigkeit[i] = 1;
			//else Geschwindigkeit[i] = -1;
		}
		arrayrot[i] = random(250);
		arraygruen[i] = random(250);
		arrayblau[i] = random(250);
		while (Modus == 31)
		{
			//Programm(5000);
			LedKetteMinus();
			bass = Lesenalogpin(BassPIN);
			for (int i = 0; i < Farbanzahl; i++)
			{
				Position[i] = (int)GenauePosition[i];

				GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
				if (bass == 0)
				{
					Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;
				}
				else
				{
					if (Geschwindigkeit[i] < 0)
					{
						if (-(Geschwindigkeit[i]) < bass * 0.003)
							Geschwindigkeit[i] = -((random(80, 120) / 50) * (bass * 0.003));
					}
					else
					{
						if (Geschwindigkeit[i] < bass * 0.003)
							Geschwindigkeit[i] = (random(80, 120) / 50) * (bass * 0.003);
					}
				}
				if (arraygruen[i] == 0 && arrayrot[i] == 0 && arrayblau[i] == 0 && bass >= 5)
				{
					switch (random(3))
					{
					case 0: arrayrot[i] = random(250); break;
					case 1: arraygruen[i] = random(250);	break;
					case 2: arrayblau[i] = random(250); break;


					default:
						break;
					}

					//ArrayFarbeMinus(i);
					GenauePosition[i] = random(NUMPIXELS);
				}
				ArrayFarbeMinus(i);

				Position[i] = (int)GenauePosition[i];
				if (einzelnblau[Position[i]] == 0)
				{
					einzelnblau[Position[i]] = arrayblau[i];
				}
				else if (einzelnblau[Position[i]] < 250)
				{
					einzelnblau[Position[i]] = (arrayblau[i] + einzelnblau[Position[i]]);
				}
				if (einzelngruen[Position[i]] == 0)
				{
					einzelngruen[Position[i]] = arraygruen[i];
				}
				else if (einzelngruen[Position[i]] < 250)
				{
					einzelngruen[Position[i]] = (arraygruen[i] + einzelngruen[Position[i]]);
				}
				if (einzelnrot[Position[i]] == 0)
				{
					einzelnrot[Position[i]] = arrayrot[i];
				}
				else if (einzelnrot[Position[i]] < 250)
				{
					einzelnrot[Position[i]] = (arrayrot[i] + einzelnrot[Position[i]]);
				}
				if (Position[i] == NUMPIXELS)
				{
					Position[i] = NUMPIXELS - 1; Geschwindigkeit[i] = -(Geschwindigkeit[i]);
				}

				if (Position[i] == 0)
				{
					Position[i] = 1; Geschwindigkeit[i] = -(Geschwindigkeit[i]);
				}
			}
			delay(Warte);
			Arraylicht();
			if (Serial.available() != 0)
			{
				aktualisierewerte();
			}
		}
	}
}
void Modus32()
{
	ARRAYFARBENResest();
	Position[0] = 0;
	short Randomwert = 0;
	short Abstaende = NUMPIXELS / Farbanzahl;
	for (int i = 0; i < Farbanzahl; i++)
	{
		switch (random(3))
		{
		case 0: arrayrot[i] = 1; break;
		case 1: arraygruen[i] = 1; break;
		case 2: arrayblau[i] = 1; break;
		default:
			break;
		}
	}
	while (Modus == 32)
	{
		Randomwert = random(Lesenalogpin(BassPIN));
		for (int a = 0; a < Farbanzahl; a++)
		{
			if (einzelnrot[a] == 0 && einzelngruen[a] == 0 && einzelnblau[a] == 0)
			{
				Lichthinzufuegen(Position[0], (arrayrot[a] * Randomwert), arrayblau[a] * Randomwert, arraygruen[a] * Randomwert);
			}
			Position[0] = Position[0] + Abstaende;
			if (Position[0] >= NUMPIXELS)
			{
				Position[0] = Position[0] - NUMPIXELS;
			}
			if (Position[0] > 196) Position[0] = 0;
		}
		Position[0] = Position[0] + Vor;
		LedKetteplusverkleinern(true);
		Arraylicht();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Abstaende = NUMPIXELS / Farbanzahl;
			if (Farbanzahl == 1) Abstaende = 0;
		}
	}
}
void Modus33()
{
	ARRAYFARBENResest();
	while (Modus == 33)
	{
		while (true)
		{
			for (int i = 0; i < Farbanzahl; i++)
			{
				GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
				Position[i] = (int)GenauePosition[i];
				Arraylicht();
				for (int a = 0; a < Farbanzahl; a++)
				{
					if (Geschwindigkeit[a] != 0)
						pixels.setPixelColor(Position[a] + Startanfang, arrayrot[a], arrayblau[a], arraygruen[a]);
				}
				Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;
				if (Geschwindigkeit[i] == 0)
				{
					Einschlagberechneneinmalig(i);
					int bass = Lesenalogpin(BassPIN);
					if (Bufferlocal[i] == 0 && bass > 50)
					{
						if (millis() > Zeit)
						{
							Zeit = millis() + 150;
							Position[i] = 0;
							Geschwindigkeit[i] = (random(80, 120) / 50) * (bass * 0.02);
							arraygruen[i] = random(240);
							arrayrot[i] = random(240);
							arrayblau[i] = random(240);
							GenauePosition[i] = 0;
						}
					}
				}
				if (Geschwindigkeit[i] <= 0.05 && Bufferlocal[i] == 0)
				{
					Bufferlocal[i] = 1;
					Geschwindigkeit[i] = 0;
				}
			}
			for (int a = 0; a != NUMPIXELS; a++)
			{
				if (einzelnrot[a] >= 1)
				{
					einzelnrot[a] = einzelnrot[a] - 1;
				}
				if (einzelnblau[a] >= 1)
				{
					einzelnblau[a] = einzelnblau[a] - 1;
				}
				if (einzelngruen[a] >= 1)
				{
					einzelngruen[a] = einzelngruen[a] - 1;
				}
			}
			delay(Warte);
			pixels.show();
			if (Serial.available() != 0)
			{
				aktualisierewerte();
				break;
			}
			delay(0);
		}
	}
}
void Modus34()
{
	int zeit2 = millis();
	ARRAYFARBENResest();
	while (Modus == 34)
	{

		for (int i = 0; i < Farbanzahl; i++)
		{
			GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
			Position[i] = (int)GenauePosition[i];
			Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;


			if (arraygruen[i] == 0 && arrayrot[i] == 0 && arrayblau[i] == 0)
			{
				Bass = Lesenalogpin(BassPIN);
				if (millis() > zeit2 && Bass > 0)
				{

					zeit2 = millis() + 100;
					arraygruen[i] = random(250);
					arrayrot[i] = random(250);
					arrayblau[i] = random(250);
					//	int ges= random(0,Bass);
				//		Geschwindigkeit[i] = Geschwindigkeit[i] = (double)ges / 50;
					//	Geschwindigkeit[i] = (double) Bass/40;
					Geschwindigkeit[i] = (double)Bass / 80;
					GenauePosition[i] = 0;
				}
			}
			else
			{
				Lichthinzufuegen(Position[i], arrayrot[i], arrayblau[i], arraygruen[i]);
			}
			if (Geschwindigkeit[i] < 0.05)
			{
				//	Serial.print("i:"); Serial.println(i);

				ArrayFarbeMinus(i);
			}
		}

		Arraylicht();
		LEDARRAYReset();
		delay(Warte);

		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus35()
{
	if (Warte == 0)Warte = 1;
	for (int i = 0; i < Farbanzahl; i++)
	{
		Zwischenspeicher[i] = 0;
	}
	double Abstand = NUMPIXELS / Farbanzahl;
	while (Modus == 35)
	{
		for (int e = 0; e < Farbanzahl; e++)
		{
			Zwischenspeicher[random(Farbanzahl)] = 1;
			Zwischenspeicher[random(Farbanzahl)] = 0;
		}
		Bass = Lesenalogpin(BassPIN);
		if (Bass > 5)
		{
			for (int i = 0; i < Farbanzahl; i++)
			{
				if (Zwischenspeicher[i] == 1)
					for (int a = 0; a < Abstand; a++)
					{

						Lichthinzufuegen(a + i * Abstand, FARBEROT, FARBEBLAU, FARBEGRUEN);
					}
				else
					for (int a = 0; a < Abstand; a++)
					{
						Lichthinzufuegen(a + i * Abstand, 0, 0, 0);
					}
			}
		}
		Arraylicht();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Abstand = NUMPIXELS / Farbanzahl;
			if (Warte == 0)Warte = 1;
		}
	}
}
void Modus36()
{
	int Welle = 0;
	double wellehoehe = 0;
	Welle = random(250);
	wellehoehe = Welle;
	while (Modus == 36)
	{
		Bass = Lesenalogpin(BassPIN);
		wellehoehe = wellehoehe * 0.70;
		if (Welle < Bass)
		{
			Welle = Bass;
			wellehoehe = Bass;
		}
		Lichthinzufuegen(0, wellehoehe, wellehoehe, wellehoehe);
		LedKetteVor(false, NUMPIXELS);
		Arraylicht();
		delay(Warte);
		if (wellehoehe <= 1)
		{
			Welle = Welle * 0.70;
			while (wellehoehe < Welle)
			{
				wellehoehe = wellehoehe * 1.25;
				Lichthinzufuegen(0, wellehoehe, wellehoehe, wellehoehe);
				LedKetteVor(false, NUMPIXELS);
				Arraylicht();
				delay(Warte);
			}
		}
		//	if(Welle <= 0) Welle = random(250);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus37() //led wird ringförmig gefärbt
{
	Position[0] = 0;
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
	{
		while (Modus == 37)
		{
			Bass = Lesenalogpin(BassPIN);
			pixels.setPixelColor(Position[0], zwischenrot * Bass, zwischengruen * Bass, zwischenblau * Bass);
			Position[0]++;
			if (Position[0] == NUMPIXELS + 1) Position[0] = 0;
			if (Serial.available() != 0)
			{
				aktualisierewerte();
				Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
			}
			delay(Warte);
			pixels.show();
		}
	}
}
void Modus38()  //leds kreisen um zwei punkte
{
	double Differenz = 0;
	int positionsumme = 0;
	for (int i = 2; i < Farbanzahl + 2; i++)
	{
		GenauePosition[i] = random(0, NUMPIXELS);
		Position[i] = (int)GenauePosition[i];
		switch (random(3))
		{
		case 0: arrayblau[i] = 50; arraygruen[i] = 0; arrayrot[i] = 0; break;
		case 1: arrayblau[i] = 0; arraygruen[i] = 50; arrayrot[i] = 0; break;
		case 2: arrayblau[i] = 0; arraygruen[i] = 0; arrayrot[i] = 50; break;
		default:
			break;
		}
	}
	Position[0] = 100;
	Position[1] = 130;
	while (Modus == 38)
	{
		LEDARRAYReset();
		positionsumme = 0;
		for (int f = 0; f < Farbanzahl + 2; f++)
		{
			positionsumme += Position[f];
		}
		for (int i = 2; i < Farbanzahl + 2; i++)
		{





			Differenz = Position[i] - (double)positionsumme / (Farbanzahl + 2);
			//Differenz = Position[i] - Position[0] + Position[i] - Position[1];


			if (Differenz != 0)
			{
				Geschwindigkeit[i] = Geschwindigkeit[i] - ((double)(0.8) / (double)(Differenz)) / (double)(2);
			}
			GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
			if (Geschwindigkeit[i] > 2) Geschwindigkeit[i] = 2;
			if (Geschwindigkeit[i] < -2) Geschwindigkeit[i] = -2;
			Position[i] = (int)GenauePosition[i];
			Lichtstacken(Position[i], arrayrot[i], arrayblau[i], arraygruen[i]);
		}
		Lichthinzufuegen(Position[0], 250, 0, 250);
		Lichthinzufuegen(Position[1], 0, 250, 250);
		Arraylicht();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			for (int i = 2; i < Farbanzahl + 2; i++)
			{
				GenauePosition[i] = random(0, NUMPIXELS);
				Position[i] = (int)GenauePosition[i];
				switch (random(3))
				{
				case 0: arrayblau[i] = 50; arraygruen[i] = 0; arrayrot[i] = 0; break;
				case 1: arrayblau[i] = 0; arraygruen[i] = 50; arrayrot[i] = 0; break;
				case 2: arrayblau[i] = 0; arraygruen[i] = 0; arrayrot[i] = 50; break;
				default:
					break;
				}
			}
		}
	}
}
void Modus39()
{
	int Welle = 0;
	double wellehoehe = 0;
	Welle = random(250);
	wellehoehe = Welle;
	while (Modus == 39)
	{
		Bass = Lesenalogpin(BassPIN);
		wellehoehe = wellehoehe * 0.70;
		if (Welle < Bass)
		{
			Welle = Bass;
			wellehoehe = Bass;
		}
		Lichthinzufuegen(0, wellehoehe, wellehoehe, wellehoehe);
		LedKetteVor(false, NUMPIXELS);
		Arraylicht();
		delay(Warte);
		if (wellehoehe <= 1)
		{
			Welle = Welle * 0.70;
			while (wellehoehe < Welle)
			{
				wellehoehe = wellehoehe * 1.25;
				Lichthinzufuegen(0, wellehoehe, wellehoehe, wellehoehe);
				LedKetteVor(false, NUMPIXELS);
				Arraylicht();
				delay(Warte);
			}
		}
		//	if(Welle <= 0) Welle = random(250);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus40() //led wird blockmässig durch bass an/ausgeschaltet
{
	int farben[3] = { 0,0,0 };
	//Serial.println("Modus40");
	if (Warte == 0)Warte = 1;
	for (int i = 0; i < Farbanzahl; i++)
	{
		Zwischenspeicher[i] = 0;
	}
	double Abstand = NUMPIXELS / Farbanzahl;
	while (Modus == 40)
	{
		Bass = Lesenalogpin(BassPIN);
		if (Bass > 5)
		{
			Zwischenspeicher[random(Farbanzahl)] = 2;
			Zwischenspeicher[random(Farbanzahl)] = 0;
			for (int i = 0; i < Farbanzahl; i++)
			{
				if (Zwischenspeicher[i] == 2)
				{
					farben[0] = Generiereaehnlichefarbe(Randomwert, FARBEROT);
					farben[1] = Generiereaehnlichefarbe(Randomwert, FARBEBLAU);
					farben[2] = Generiereaehnlichefarbe(Randomwert, FARBEGRUEN);
					for (int a = 0; a < Abstand; a++)
					{
						Lichthinzufuegen(a + i * Abstand, farben[0], farben[1], farben[2]);
						Zwischenspeicher[i] == 1;
					}
				}
				if (Zwischenspeicher[i] == 0)
				{
					for (int a = 0; a < Abstand; a++)
					{
						Lichthinzufuegen(a + i * Abstand, 0, 0, 0);
					}
				}
			}
		}
		Arraylicht();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Abstand = NUMPIXELS / Farbanzahl;
			if (Warte == 0)Warte = 1;
		}
	}
}
void Modus41()
{

	double steilheit = ((double)random(100, 300) / 2000);//=x
	int maxwert = 1;
	double mutiplikator = 0;
	int farben[3] = { 0,0,0 };
	int nullpunkt = 0;
	nullpunkt = (sqrt(maxwert) / steilheit); //sqrt ist die wurzel
	int position = random(NUMPIXELS - 2 * nullpunkt);
	for (int i = 0; i < NUMPIXELS; i++)
	{
		mutiplikator = -sq(steilheit * (i - nullpunkt)) + maxwert; //sq ist ²
		farben[0] = (mutiplikator * Generiereaehnlichefarbe(Randomwert, FARBEROT)) / maxwert; if (farben[0] < 0) farben[0] = 0;  if (farben[0] > 250) farben[0] = 250;
		farben[1] = (mutiplikator * Generiereaehnlichefarbe(Randomwert, FARBEBLAU)) / maxwert; if (farben[1] < 0) farben[1] = 0;  if (farben[1] > 250) farben[1] = 250;
		farben[2] = (mutiplikator * Generiereaehnlichefarbe(Randomwert, FARBEGRUEN)) / maxwert; if (farben[2] < 0) farben[2] = 0;  if (farben[2] > 250) farben[0] = 250;
		Lichthinzufuegen(i + position, farben[0], farben[1], farben[2]);
		Arraylicht();
		LedKetteMinus();
		//Serial.println("x: " + (String) i+ ("y:")+ (String)mutiplikator);
		//Serial.println("nullpunkt: " + (String)nullpunkt);
		if (mutiplikator < 0) break;
		delay(Warte);
	}
	//for (int i = 0; i < 250; i++)
	{

		//delay(Warte);
		//Arraylicht();
	}
}
void Modus42()
{
	int aktuellerplatz = 0;
	int mitte = NUMPIXELS / 2;
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
	while (Modus == 42)
	{
		Bass = Lesenalogpin(BassPIN);
		for (int i = 0; i < 2; i++)
		{
			Position[aktuellerplatz] = mitte;
			arrayrot[aktuellerplatz] = zwischenrot * Bass;
			arrayblau[aktuellerplatz] = zwischenblau * Bass;
			arraygruen[aktuellerplatz] = zwischengruen * Bass;
			if (aktuellerplatz % 2 == 0)
			{
				Geschwindigkeit[aktuellerplatz] = 1;
			}
			else Geschwindigkeit[aktuellerplatz] = -1;
			aktuellerplatz++;
			if (aktuellerplatz >= NUMPIXELS) aktuellerplatz = 0;
		}

		for (int i = 0; i < NUMPIXELS; i++)
		{
			if (Position[i] != 0 && Position[i] != NUMPIXELS)
			{
				Lichthinzufuegen(Position[i], arrayrot[i], arrayblau[i], arraygruen[i]);
			}Position[i] = ErrechneGenaueFarbposition(Position[i], Geschwindigkeit[i]);
		}
		delay(Warte);
		Arraylicht();
		LEDARRAYReset();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
		}
	}
}
void Modus43()
{
	while (Modus == 43)
	{
		for (int i = 0; i < NUMPIXELS; i++)
		{
			if (i < NUMPIXELS / 2)
			{
				pixels.setPixelColor(i + Startanfang, pixels.Color(einzelnrot[0], einzelngruen[0], einzelnblau[0]));
			}
			else
			{
				pixels.setPixelColor(i + Startanfang, pixels.Color(einzelnrot[1], einzelngruen[1], einzelnblau[1]));
			}
		}
		pixels.show();
		delay(Warte);
		for (int i = 0; i < NUMPIXELS; i++)
		{
			if (i < NUMPIXELS / 2)
				pixels.setPixelColor(i + Startanfang, pixels.Color(einzelnrot[1], einzelngruen[1], einzelnblau[1]));
			else
				pixels.setPixelColor(i + Startanfang, pixels.Color(einzelnrot[0], einzelngruen[0], einzelnblau[0]));
		}
		pixels.show();
		delay(Warte);
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
}
void Modus44()
{
	pixels.clear();
	int Arrayposition = 0;
	unsigned long zeitarray[200];
	unsigned long zeitstempel = millis();
	for (short i = 0; i < 200; i++)
	{
		arrayblau[i] = random(FARBEBLAU);
		arrayrot[i] = random(FARBEROT);
		arraygruen[i] = random(FARBEGRUEN);
		Position[i] = 245;
	}
	while (Modus == 44)
	{
		zeitstempel = millis();
		for (short i = 0; i < Farbanzahl; i++)
		{
			if ((int)Position[i] == 245)
			{
				Position[i] = random(NUMPIXELS);
				pixels.setPixelColor(Position[i], arrayrot[Arrayposition], arraygruen[Arrayposition], arrayblau[Arrayposition]);
				Arrayposition++;
				if (Arrayposition > 200) Arrayposition = 0;
				zeitarray[i] = zeitstempel + Warte;
			}
			if (zeitarray[i] < millis())
			{
				pixels.setPixelColor(Position[i], 0, 0, 0);
				Position[i] = 245; //Platz für neue Farben freigeben
			}
		}
		pixels.show();
		yield();

		if (Serial.available() != 0)
		{
			int farbanzahl = Farbanzahl;
			aktualisierewerte();
			for (short i = 0; i < 200; i++)
			{
				arrayblau[i] = random(FARBEBLAU);
				arrayrot[i] = random(FARBEROT);
				arraygruen[i] = random(FARBEGRUEN);

			}
		}
	}

}
void Modus45()
{

	while (Modus == 45)
	{
		Ledarrayausbuffer(); //Speichert die aktuell angezeigte leds in die einzel arrays auf
		for (int Schritte = 1; Schritte < 101; Schritte++)
		{
			for (int i = 0; i < NUMPIXELS; i++)
			{
				if (Debugmodus)
				{
					Serial.println("i: " + (String)i);
					Serial.println("Blau: " + (String)(einzelnblau[i] + ((FARBEBLAU - einzelnblau[i]) * Schritte / 100)));
					Serial.println("Gruen: " + (String)(einzelngruen[i] + ((FARBEGRUEN - einzelngruen[i]) * Schritte / 100)));
					Serial.println("Rot: " + (String)(einzelnrot[i] + ((FARBEROT - einzelnrot[i]) * Schritte / 100)));
					pixels.show();
					delay(Warte);
				}

				//pixels.setPixelColor(i, einzelnrot[i] + ((FARBEROT - einzelnrot[i]) * Schritte / 100), einzelngruen[i] + ((FARBEGRUEN - einzelngruen[i]) * Schritte / 100), einzelnblau[i] + ((FARBEBLAU - einzelnblau[i]) * Schritte / 100));
				pixels.setPixelColor(i, einzelnrot[i] + ((FARBEROT - einzelnrot[i]) * Schritte / 100), einzelnblau[i] + ((FARBEBLAU - einzelnblau[i]) * Schritte / 100), einzelngruen[i] + ((FARBEGRUEN - einzelngruen[i]) * Schritte / 100));
			}
			pixels.show();
			delay(Warte);
			if (Serial.available() != 0)
			{

				aktualisierewerte();
				Ledarrayausbuffer();
				Schritte = 1;
			}
		}

	}


}
void Modus46()
{

	while (Modus = 46)
	{
		int wartezeit = 0;
		wartezeit = Lesenalogpin(BassPIN);
		Lichtersofort(FARBEROT, FARBEBLAU, FARBEGRUEN);
		delay(wartezeit);
		Lichtersofort(0, 0, 0);
		delay(wartezeit);
		if (Serial.available() != 0)
		{
			aktualisierewerte();

		}
	}
}
void Modus100()
{
	Ledarrayausbuffer();
	while (Modus == 100)
	{
		if (Serial.available() != 0)
		{

			aktualisierewerte();


		}
		Arraylicht(); delay(0);
	}

}
void Modus102()
{
	LedKetteVor(true, NUMPIXELS);
	Modus = 100;
}
void Modus103()
{
	LedKetteHinten(true, NUMPIXELS);
	Modus = 100;
}
void LedArrayauflisten()
{
	for (int a = 0; a != NUMPIXELS; a++)
	{
		if (einzelnrot[a] != 0 || einzelngruen[a] != 0 || einzelnrot[a] != 0)
		{
			Serial.print("Aktueller Platz: ");
			Serial.println(a);
			Serial.print("Rot: ");
			Serial.println(einzelnrot[a]);
			Serial.print("Gruen: ");
			Serial.println(einzelngruen[a]);
			Serial.print("Blau: ");
			Serial.println(einzelnblau[a]);
			delay(0);
		}
	}
	Modus = 100;
}
void ARRAYFARBENResest()
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		arrayblau[i] = 0;
		arrayrot[i] = 0;
		arraygruen[i] = 0;
	}
}
void LEDARRAYReset()
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		einzelnrot[i] = 0;
		einzelnblau[i] = 0;
		einzelngruen[i] = 0;
	}
}
void Ledarrayausbuffer()  //speichert die angezeigte farbwerte in ein array
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		int ledwert = pixels.getPixelColor(i);
		if (ledwert >= 65536) { einzelnrot[i] = ledwert / 65536; ledwert = ledwert - (65536 * einzelnrot[i]); }
		else einzelnrot[i] = 0;
		if (ledwert >= 256) { einzelnblau[i] = ledwert / 256; ledwert = ledwert - (256 * einzelnblau[i]); }
		else einzelnblau[i] = 0;
		if (ledwert >= 1) { einzelngruen[i] = ledwert; }
		else einzelngruen[i] = 0;
	}
}
void Zeigezwischenwerte()
{
	Serial.println("Grun: " + (String)zwischengruen);
	Serial.println("Rot: " + (String)zwischenrot);
	Serial.println("Blau: " + (String)zwischenblau);
}
void Arrayfarbenauflisten()
{
	for (int a = 0; a != NUMPIXELS; a++)
	{
		if (arrayrot[a] != 0 || arrayblau[a] != 0 || arraygruen[a] != 0)
		{
			Serial.print("Aktueller Platz: ");
			Serial.println(a);
			Serial.print("Rot: ");
			Serial.println(arrayrot[a]);
			Serial.print("Gruen: ");
			Serial.println(arraygruen[a]);
			Serial.print("Blau: ");
			Serial.println(arrayblau[a]);
			delay(0);
		}
	}
	Modus = 100;
}
void Manager()
{
	if (Serial.available() != 0)
	{
		aktualisierewerte();
	}

}
void aktualisierewerte()
{
	//checkt ob ankommende nachrichten mit der ersten gesendeten ziffer übereinstimmt, wenn nicht, sendert er ein ~
	if (submodus)
	{
		Daten.lesewertsub(Debugmodus);
		Wertezuordnen(Daten.Buchstabe, Daten.errechneterwert);
	}
	else
	{
		//checkt ob eine Nachricht noch verarbeitet werden soll
		delay(10);
		if (Daten.zweiterbuchstabe)
		{
			Daten.Signalzuordnen();
			Wertezuordnen(Daten.Buchstabe, Daten.Errechnewert());
			return;
		}
		else
		{
			//Lese die serriele schnittstelle aus
			Daten.lesewert(Debugmodus);
			//Wenn der erst char ein Buchstabe ist, dann soll er die ziffern danach auslesen
			if (Daten.Signal >= 58)
			{
				Daten.Signalzuordnen();
				Wertezuordnen(Daten.Buchstabe, Daten.Errechnewert());
			}
		}

	}
}

int Lesenalogpin(int ANALOGPIN)
{
	int wert = 0, kleinsterwert = 1024;
	if (!Ersetzebass)
	{
		for (int i = 0; i < Bassfilter; i++)
		{
			wert = analogRead(ANALOGPIN);
			if (wert <= kleinsterwert)
			{
				kleinsterwert = wert;
			}
		}

		wert = kleinsterwert - 14;
		if (wert <= 0) wert = 0;
	}
	else
	{
		wert = Bass;
	}
	return wert;
}


void Wertezuordnen(int Buchstabe, int Wert)
{
	if (Debugmodus) { Serial.print("Buchstabe: "); Serial.println(Buchstabe); Serial.print("wert: "); Serial.println(Wert); }
	switch (Buchstabe)
	{
	case 83: Synchron(); break;
	case 98: FARBEBLAU = Wert; break;
	case 99: CreativMode(Wert); break;
	case 102: Bassfilter = Wert; break;
	case 104: Hinten = Wert; break;
	case 103: FARBEGRUEN = Wert; break;
	case 109: Modus = Wert; break;
	case 110: Farbanzahl = Wert; break;
	case 112: Randomwert = Wert; break;
	case 114: FARBEROT = Wert; break;
	case 116: Bass = Wert; break;
	case 118: Vor = Wert; break;
	case 119: Warte = Wert; break;
	default:
		break;
	}
}

void Synchron()
{
	Serial.print("BLAU"); Serial.print(FARBEBLAU);
	Serial.print("G"); Serial.print(FARBEGRUEN);
	Serial.print("R"); Serial.print(FARBEROT);
	Serial.print("M"); Serial.print(Modus);
	Serial.print("W"); Serial.print(Warte);
	Serial.print("V"); Serial.print(Vor);
	Serial.print("H"); Serial.print(Hinten);
	Serial.print("A"); Serial.print(Farbanzahl);
	Serial.print("F"); Serial.print(Bassfilter);
	Serial.print("B"); Serial.print(Bass);
}
void Lichter(int Rot, int Blau, int Gruen, int Wartev, int Start, int Stop)
{
	unsigned long zeit = millis();
	unsigned long zeitseitstart;
	for (int i = Start; i < Stop; i++)
	{
		pixels.setPixelColor(i + Startanfang, pixels.Color(Rot, Blau, Gruen));
		zeitseitstart = millis();
		if (zeitseitstart > zeit + Warte)
		{
			zeit = millis();
			pixels.show();
		}
		delayMicroseconds(1000);
		if (Serial.available() != 0)
		{
			int Temp_Modus = Modus;
			aktualisierewerte();
			Blau = FARBEBLAU;
			Gruen = FARBEGRUEN;
			Rot = FARBEROT;
			if (Temp_Modus != Modus)
			{
				break;
			}
		}
	}
}

void Lichtersofort(int Rot, int Blau, int Gruen)
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		pixels.setPixelColor(i + Startanfang, pixels.Color(Rot, Blau, Gruen));
	}
	pixels.show();
}
//Überträgt die Werte auf die Led Kette und zeigt sie an (Ausgenommen modus 33)
void Arraylicht()
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		pixels.setPixelColor(i + Startanfang, einzelnrot[i], einzelnblau[i], einzelngruen[i]);
	}
	if (Modus != 33)pixels.show();

}
void Lichthinzufuegen(int Position, int Rot, int Blau, int Gruen)
{
	einzelnrot[Position] = Rot;
	einzelnblau[Position] = Blau;
	einzelngruen[Position] = Gruen;
}
void Lichtstacken(int Position, int Rot, int Blau, int Gruen) //adiert ein farbe zur led
{
	einzelnrot[Position] += Rot; if (einzelnrot[Position] > 250) einzelnrot[Position] = 250;
	einzelnblau[Position] += Blau; if (einzelnblau[Position] > 250) einzelnblau[Position] = 250;
	einzelngruen[Position] += Gruen; if (einzelngruen[Position] > 250) einzelngruen[Position] = 250;
}
//ledsfarben werden nach rechts verschoben
void LedKetteHinten(bool Schleife, int bereich)
{
	if (Schleife)
	{
		zwischenrot = einzelnrot[0];
		zwischenblau = einzelnblau[0];
		zwischengruen = einzelngruen[0];
	}
	for (int i = 1; i < bereich; i++)
	{
		yield();
		einzelnrot[i - 1] = einzelnrot[i];
		einzelnblau[i - 1] = einzelnblau[i];
		einzelngruen[i - 1] = einzelngruen[i];
	}
	if (Schleife)
	{
		einzelnrot[bereich - 1] = zwischenrot;
		einzelnblau[bereich - 1] = zwischenblau;
		einzelngruen[bereich - 1] = zwischengruen;
	}
}
void VerschiebeLedlinks(bool schleife, int start, int ende)
{
	int farben[] = { 0,0,0 };
	if (schleife)
	{
		farben[0] = einzelnrot[ende];
		farben[1] = einzelnblau[ende];
		farben[2] = einzelngruen[ende];
	}
	for (int i = start; i < ende - 1; i++)
	{
		einzelnrot[i + 1] = einzelnrot[i];
		einzelngruen[i + 1] = einzelngruen[i];
		einzelnblau[i + 1] = einzelnblau[i];
	}
	if (schleife)
	{
		einzelnrot[start] = farben[0];
		einzelnblau[start] = farben[1];
		einzelngruen[start] = farben[2];
	}
}
//ledsfarben werden nach links verschoben
void LedKetteVor(bool Schleife, int bereich)
{
	int farben[] = { 0,0,0 };
	if (Schleife)
	{
		farben[0] = einzelnrot[bereich - 1];
		farben[1] = einzelnblau[bereich - 1];
		farben[2] = einzelngruen[bereich - 1];
	}
	for (int i = NUMPIXELS - 1; i != 0; i--)
	{
		yield();
		einzelnrot[i] = einzelnrot[i - 1];
		einzelnblau[i] = einzelnblau[i - 1];
		einzelngruen[i] = einzelngruen[i - 1];
	}
	if (Schleife)
	{
		einzelnrot[0] = farben[0];
		einzelnblau[0] = farben[1];
		einzelngruen[0] = farben[2];
	}
	if (!Schleife)
	{
		einzelnrot[0] = 0;
		einzelnblau[0] = 0;
		einzelngruen[0] = 0;
	}
}
void LedKetteMinus() //Verdunkelt die farbearray um 1
{
	for (int i = 0; i != NUMPIXELS; i++)
	{
		if (einzelnrot[i] > 0) einzelnrot[i]--;
		if (einzelnblau[i] > 0) einzelnblau[i]--;
		if (einzelngruen[i] > 0) einzelngruen[i]--;
	}
}
//verdunkelt einzelne leds
void ArrayFarbeMinus(int i)
{
	if (arraygruen[i] != 0) arraygruen[i]--;
	if (arrayrot[i] != 0) arrayrot[i]--;
	if (arrayblau[i] != 0) arrayblau[i]--;
}
//verschiebt die Led nach links und verdunkelt sie
void LedKetteplusverkleinern(bool Schleife)
{
	int farben[] = { 0,0,0 };
	if (Schleife)
	{
		farben[0] = einzelnrot[NUMPIXELS - 1];
		farben[1] = einzelnblau[NUMPIXELS - 1];
		farben[2] = einzelngruen[NUMPIXELS - 1];
	}
	for (int i = NUMPIXELS - 1; i != 0; i--)
	{
		if (einzelnrot[i] > 0) einzelnrot[i]--;
		if (einzelnblau[i] > 0) einzelnblau[i]--;
		if (einzelngruen[i] > 0) einzelngruen[i]--;
	}
	LedKetteVor(Schleife, NUMPIXELS);
	if (Schleife)
	{
		einzelnrot[0] = farben[0];
		einzelnblau[0] = farben[1];
		einzelngruen[0] = farben[2];
	}
}
void Streifen(bool Schleife)
{
	for (int i = 0; i < Vor; i++)
	{
		Arraylicht();
		LedKetteVor(Schleife, NUMPIXELS);
		delay(Warte);
	}
	for (int i = 0; i < Hinten; i++)
	{
		Arraylicht();
		LedKetteHinten(Schleife, NUMPIXELS);
		delay(Warte);
	}
}
int ErrechneFarbpositionen(int Position, int Geschwindigkeit)
{
	Position = Position + Geschwindigkeit;
	if (Position > NUMPIXELS) Position = NUMPIXELS;
	if (Position < 0) Position = 0;
	return Position;
}
int Generiereaehnlichefarbe(int Prozent, int Farbe)
{
	Prozent = random(Prozent);
	if (random(2) == 0)
	{
		Farbe += ((double)Prozent / 100) * (double)Farbe;
		if (Farbe >= 255) Farbe = 254;
	}
	else
	{
		Farbe -= ((double)Prozent / 100) * (double)Farbe;
		if (Farbe < 0) Farbe = 0;
	}
	return Farbe;
}
double ErrechneGenaueFarbposition(double Position, double Geschwindigkeit)
{
	Position = Position + Geschwindigkeit;
	if (Position > NUMPIXELS) Position = NUMPIXELS;
	if (Position < 0) Position = 0;
	return Position;
}
double ErrechneGeschwindigkeit(double Geschwindigkeit, double Beschleunigung)
{
	return Geschwindigkeit * Beschleunigung;
}
void Errechnezwischenwerte(double Rot, double Blau, double Gruen, double Schritte)
{
	zwischenrot = Rot / Schritte;
	zwischenblau = Blau / Schritte;
	zwischengruen = Gruen / Schritte;
}
void CreativMode(int Wert)
{
	switch (Wert)
	{
	case 0:
		UebertragePixel();
		break;
	case 1: if (Debugmodus) { Debugmodus = false; Serial.println("debug aus"); }
		  else {
		Debugmodus = true; Serial.println("debug an");
	}break;
	case 2: if (Startecounter) Startecounter = false; else Startecounter = true;	Zeit = millis(); break;
	case 3: if (Ersetzebass) Ersetzebass = false; else Ersetzebass = true; break;
	case 4: Speicherparameter(); break;
	case 5: Ladeparameter();
		pixels.updateLength(NUMPIXELS);
		break;
	case 6: Serial.println(Versionnummer); break;
	case 7: Aenderpixelanzahl(); break;
	case 8: if (Debugmodus_Internet) { Debugmodus_Internet = false; Serial.println("debug_internet aus"); }
		  else {
		Debugmodus = true; Serial.println("debug_internet an");
	}break;
	case 9: AenderAnalogAufloesung(); break;
	default:
		break;
	}
}
void Aenderpixelanzahl()
{
	if (Daten.zweiterbuchstabe)
	{
		Daten.Signalzuordnen();
		if (Daten.Buchstabe == 97) //a
		{
			NUMPIXELS = Daten.Errechnewert();
			pixels.updateLength(NUMPIXELS);
		}
	}
}
void AenderAnalogAufloesung()
{
	if (Daten.zweiterbuchstabe)
	{
		Daten.Signalzuordnen();
		if (Daten.Buchstabe == 97) //a
		{
			int Aufloesung = Daten.Errechnewert();
			if (Aufloesung < 9) Aufloesung = 9;
			if (Aufloesung > 12) Aufloesung = 12;
			analogReadResolution(Aufloesung);
		}
	}
}
void UebertragePixel()
{
	int position = 0;
	while (Serial.available() != 0) //checkt ob eine Nachricht noch verarbeitet werden soll
	{
		delay(1);
		if (Daten.zweiterbuchstabe)
		{
			Daten.Signalzuordnen();
			if (Daten.Buchstabe == 112) //p
			{
				position = Daten.Errechnewert();
			}
			else
			{
				Pixelzuordnen(position, Daten.Buchstabe, Daten.Errechnewert());
			}
		}
		else
		{
			//Lese die serriele schnittstelle aus
			Daten.lesewert(Debugmodus);
			//Wenn der erst char ein Buchstabe ist, dann soll er die ziffern danach auslesen
			if (Daten.Signal >= 58)
			{
				Daten.Signalzuordnen();
				if (Daten.Buchstabe == 112)
				{
					position = Daten.Errechnewert();
					if (position > NUMPIXELS) position = NUMPIXELS;
				}
				else
				{
					Pixelzuordnen(position, Daten.Buchstabe, Daten.Errechnewert());
				}
			}
		}
	}
	Arraylicht();
}
void Pixelzuordnen(int position, int buchstabe, int wert)
{
	switch (buchstabe)
	{
	case 114: einzelnrot[position] = wert; break;
	case 103: einzelngruen[position] = wert; break;
	case 98: einzelnblau[position] = wert; break;
	default:
		break;
	}
}
void Controllerausschalten(int ZeitinMinuten)
{
	if (Debugmodus) Serial.println("Shutdown für " + (String)ZeitinMinuten + " Minuten");
	esp_sleep_enable_timer_wakeup(uint((uint)ZeitinMinuten * (uint)60000000));
	delay(1000);
	esp_deep_sleep_start();
}
//speichert werte, maximaler bereich: 255
void Speicherparameter()
{
	EEPROM.begin(512);
	EEPROM.write(0, FARBEBLAU);
	EEPROM.write(1, FARBEGRUEN);
	EEPROM.write(2, FARBEROT);
	EEPROM.write(3, Modus);
	EEPROM.write(4, Warte);
	EEPROM.write(5, Vor);
	EEPROM.write(6, Hinten);
	EEPROM.write(7, Bassfilter);
	EEPROM.write(8, Randomwert);
	EEPROM.write(9, Farbanzahl);
	EEPROM.write(10, Bass);
	EEPROM.write(11, NUMPIXELS);
	EEPROM.write(12, Accespointmodus);
	EEPROM.write(13, Einschlafzeit);
	EEPROM.write(14, Aufwachzeit);
	EEPROM.commit();
	EEPROM.end();
}
void Ladeparameter()
{
	EEPROM.begin(512);
	FARBEBLAU = int(EEPROM.read(0));
	FARBEGRUEN = int(EEPROM.read(1));
	FARBEROT = int(EEPROM.read(2));
	Modus = int(EEPROM.read(3));
	Warte = int(EEPROM.read(4));
	Vor = int(EEPROM.read(5));
	Hinten = int(EEPROM.read(6));
	Bassfilter = int(EEPROM.read(7));
	Randomwert = int(EEPROM.read(8));
	Farbanzahl = int(EEPROM.read(9));
	Bass = int(EEPROM.read(10));
	//NUMPIXELS = int(EEPROM.read(11));
	Accespointmodus = EEPROM.read(12);
	Einschlafzeit = EEPROM.read(13);
	Aufwachzeit = EEPROM.read(14);
	EEPROM.commit();
	EEPROM.end();
	//pixels.updateLength(NUMPIXELS);
}
void InterruptModuserhoehen()
{
	if (Debugmodus)
	{
		Serial.println("Interrupt+");
	}
	if (millis() > Tastendruck + 200)
	{
		Tastendruck = millis();
		if (Hardwaremodus < 14)	Hardwaremodus++;
		Funktionstasten(Hardwaremodus);
		if (Debugmodus)
		{
			Serial.println("Modus erhöht: " + String(Hardwaremodus));
		}
	}
}
void InterruptModusverkleinern()
{
	if (Debugmodus)
	{
		Serial.println("Interrupt-");
	}
	if (millis() > Tastendruck + 200)
	{
		Tastendruck = millis();
		if (Hardwaremodus > 1)	Hardwaremodus--;
		Funktionstasten(Hardwaremodus);
		if (Debugmodus)
		{
			Serial.println("Modus verkleinert: " + String(Hardwaremodus));
		}
	}
}
void Funktionstasten(int Modusfunktion)
{
	switch (Modusfunktion)
	{
	case 1: FARBEBLAU = 255; FARBEGRUEN = 0; FARBEROT = 0; break;
	case 2: FARBEBLAU = 0; FARBEGRUEN = 255; FARBEROT = 0; break;
	case 3: Modus = 1; FARBEBLAU = 0; FARBEGRUEN = 0; FARBEROT = 255; break;
	case 4: Modus = 101; break;
	case 5: Modus = 21; break;
	case 6: Modus = 10; Warte = 0; break;
	case 7: Modus = 11; Warte = 30; Vor = 1;  break;
	case 8: Modus = 15; break;
	case 9: Modus = 18; Bass = 50; Warte = 10, Farbanzahl = 2; break;
	case 10: Modus = 26; break;
	case 11: Modus = 29; Farbanzahl = 7; break;
	case 12: Modus = 43; einzelngruen[0] = 250; einzelnblau[1] = 250; Warte = 0; break;
	case 13: Modus = 13; break;
	case 14: Modus = 9; break;
	default:
		break;
	}
}
void internet()
{
	WiFiClient client = server.available();  //prüft ob etwas neues zu verarbeiten ist
	if (!client)
	{
		delay(1);
		return;
	}
	sliderbool = false;
	unsigned long ultimeout = millis() + 500; // Wait until the client sends some data
	while (!client.available() && (millis() < ultimeout))
	{
		delay(1);
	}
	if (millis() > ultimeout)
	{
		if (Debugmodus)Serial.println("client connection time-out!");
		return;
	}
	//String Nachricht = client.readStringUntil('\r');  // Liest die erste zeile 
	String Nachricht = client.readString();
	if (Debugmodus_Internet)
	{
		Serial.print("Empfangene Nachricht: "); Serial.println(Nachricht);
	}

	if (Nachricht == "")
	{
		if (Debugmodus) Serial.println("empty request! - stopping client");
		client.stop();
		return;
	}
	client.flush(); //wartet bis alle daten gesendet wurden
	 // get path; end of path is either space or ?
	String sPath = "", sParam = "", sCmd = "";
	int CutStart, iEndSpace, iEndQuest;
	if (Nachricht.indexOf("GET ") >= 0)
	{
		CutStart += 4;
		iEndSpace = Nachricht.indexOf(" ", CutStart);
		iEndQuest = Nachricht.indexOf("?", CutStart);
		if (Debugmodus)Serial.println("Getstart: " + (String)CutStart);
	}
	if (Nachricht.indexOf("PUT ") >= 0)
	{
		CutStart = 4;
		iEndSpace = Nachricht.indexOf(" ", CutStart);
		iEndQuest = Nachricht.indexOf("?", CutStart);
		if (Debugmodus)Serial.println("Putstart: " + (String)CutStart);
	}
	if (Debugmodus)Serial.println("iEndSpace: " + (String)iEndSpace);
	if (Debugmodus)Serial.println("iEndQuest : " + (String)iEndQuest);
	if (iEndSpace > 0)		// are there parameters?
	{
		if (iEndQuest > 0) //wenn ein parameter vorhanden ist
		{
			if (iEndQuest > iEndSpace)
			{
				sPath = Nachricht.substring(CutStart, iEndSpace);
			}
			else
			{
				sPath = Nachricht.substring(CutStart, iEndQuest);
			}
			sParam = Nachricht.substring(iEndQuest, iEndSpace);
			if (Debugmodus)
			{
				Serial.print("sparam: ");
				Serial.println(sParam);
			}
		}
		else
		{
			sPath = Nachricht.substring(CutStart, iEndSpace);
		}
		if (Debugmodus) { Serial.print("sPath: "); Serial.println(sPath); Serial.println(" "); }
	}
	//verarbeitet die daten
	if (sParam.length() > 0)
	{
		int iEqu = sParam.indexOf("=");
		int slider = sParam.indexOf("Slider");
		if (slider == -1) //wenn kein slider in der empfangene nachricht vorhanden ist
		{
			sliderbool = false;
			if (iEqu >= 0)
			{
				sCmd = sParam.substring(iEqu + 1, sParam.length());
			}
		}
		else   //slider erkannt
		{
			if (Debugmodus)Serial.println("slider erkannt: "),
				sCmd = sParam.substring(iEqu + 1, sParam.length());
			if (sParam.indexOf("Slider1") > -1)
			{
				sliderbool = true;
			}
			if (sParam.indexOf("Slider2") > -1)
			{
				if (Debugmodus) Serial.println("Slider2");
				Tastenmodus = 50;
				einzelngruen[pixelposition] = sCmd.toInt();
			}
			if (sParam.indexOf("Slider3") > -1)
			{
				Tastenmodus = 50;
				einzelnrot[pixelposition] = sCmd.toInt();
			}
			if (sParam.indexOf("Slider4") > -1)
			{
				Tastenmodus = 50;
				einzelnblau[pixelposition] = sCmd.toInt();
			}
		}
		if (sParam.indexOf("input1") > -1)
		{
			String Konsole = sCmd;
			if (Debugmodus) Serial.println("Konsole: " + Konsole);
			VerarbeiteBefehl(Konsole);
			sPath = "/optionen/konsole";
		}
		if (sParam.indexOf("input2") > -1)
		{
			String Konsole = sCmd;
			if (Debugmodus) Serial.println("schreibe datei: " + Konsole);
			char charbuf[30];
			ErgaenzeTxtFile(SPIFFS, "/hello.txt", Konsole);
			sPath = "/optionen/explorer";
		}
		if (sParam.indexOf("input") > -1)
		{
			int ParameterWertStart = 0, ParameterWertEnde = 0;
			int VariablennameStart = 0, VariablennameEnde = 0;
			String NameVariable = "";
			while (ParameterWertEnde < sParam.length())
			{
				VariablennameStart = sParam.indexOf("?", ParameterWertStart); //zählt das ? auch noch mit
				if (VariablennameStart < 0)
				{
					VariablennameStart = sParam.indexOf("&", ParameterWertStart);
				}
				ParameterWertStart = sParam.indexOf("=", ParameterWertStart);
				ParameterWertEnde = sParam.indexOf("&", ParameterWertStart);
				VariablennameEnde = ParameterWertStart;
				if (ParameterWertEnde < 0)
				{
					ParameterWertEnde = sParam.length();
				}
				NameVariable = sParam.substring(VariablennameStart + 1, VariablennameEnde);
				String Wert = sParam.substring(ParameterWertStart + 1, ParameterWertEnde); //+1 bewirkt, dass = nicht übernommen wird
				if (Debugmodus)
				{
					Serial.println("VariablennameStart: " + VariablennameStart);
					Serial.println("VariablennameEnde: " + VariablennameEnde);
					Serial.println("Variablenname: " + NameVariable);
					Serial.println("ParameterWertStart: " + ParameterWertStart);
					Serial.println("ParameterEnde: " + ParameterWertEnde);
					Serial.println("wert: " + (String)Wert);
					Serial.println("Sparam länge: " + (String)sParam.length());
				}
				ParameterWertStart++;
				if (NameVariable == "input3" && Wert != "")
				{
					Aufwachzeit = Wert.toInt();
				}
				if (NameVariable == "input4" && Wert != "")
				{
					Einschlafzeit = Wert.toInt();
				}
			}
			sPath = "/optionen/zeiten";
		}
		VerarbeiteNachricht(sCmd);
		//  sCmd = sParam.substring(iEqu+1,sParam.length());
		if (Debugmodus)
		{
			Serial.print("scmd: ");
			Serial.println(sCmd);
		}
	}
	if (Nachricht.indexOf("GET ") >= 0)
	{
		if (sPath == "/")
		{
			Seite_Startseite(sCmd);
		}
		else if (sPath == "/test")
		{
			Seite_Aktivezeit();
		}
		else if (sPath == "/optionen")
		{
			Seite_Optionen(sCmd);
		}
		else if (sPath == "/optionen/explorer")
		{
			Seite_Explorer(sCmd);
		}
		else if (sPath == "/optionen/pixel")
		{
			Tastenmodus = 13;
			werteveraendern(Temporaererwert);
			Seite_Pixelfaerben(sCmd);
		}
		else if (sPath == "/optionen/zeiten")
		{
			Seite_Zeiten();
		}
		else if (sPath == "/Tasteneingabe")
		{

		}
		else if (sPath == "/optionen/konsole")
		{
			Seite_Konsole();
		}
		else if (sPath == "/xml")
		{
			XMLcontent();
		}
		else
		{
			Seite_Nichtguelitg();
		}
		client.print(sHeader);
		client.print(sResponse);
	}
	client.stop();
}
void Seite_Startseite(String sCmd)
{
	if (Debugmodus) Serial.println("InternetStartseite");
	sResponse = "<html><head><title>Led Steuerung</title></head><body>";
	sResponse += Farben("#37f2e8", "#0e2aa5");
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Led Steuerung</h1>";
	sResponse += "<FONT SIZE=+2>";
	sResponse += Buttonhinzufuegen("Blau", "Blau", false);
	sResponse += Buttonhinzufuegen("Gruen", "Gruen", false);
	sResponse += Buttonhinzufuegen("Rot", "Rot", false);
	sResponse += Buttonhinzufuegen("Modus", "Modus", false);
	sResponse += Buttonhinzufuegen("Warte", "Warte", false);
	sResponse += Buttonhinzufuegen("Vor", "Vor", false);
	sResponse += Buttonhinzufuegen("Hinter", "Hinter", false);
	sResponse += Buttonhinzufuegen("Farbanzahl", "Farbanzahl", false);
	sResponse += Buttonhinzufuegen("Frequenzfilter", "Frequenzfilter", false);
	sResponse += Buttonhinzufuegen("Randomwerte", "Randomwerte", false);
	sResponse += Buttonhinzufuegen("NumPixels", "NumPixels", false);
	sResponse += Buttonhinzufuegen("FunktionsTaste", "FunktionsTaste", false);
	sResponse += Buttonhinzufuegen("Bassersatz", "Bassersatz", false) + "</p>";
	//	sResponse += Buttonhinzufuegen("Weitere Optionen", "Optionen", true);
	sResponse += "<FONT SIZE=-4>";
	sResponse += Buttonhinzufuegen("7", "7", false);
	sResponse += Buttonhinzufuegen("8", "8", false);
	sResponse += Buttonhinzufuegen("9", "9", false);
	sResponse += Buttonhinzufuegen("loeschen", "loeschen", true);
	sResponse += Buttonhinzufuegen("4", "4", false);
	sResponse += Buttonhinzufuegen("5", "5", false);
	sResponse += Buttonhinzufuegen("6", "6", false);
	sResponse += Buttonhinzufuegen("Uebernehmen", "senden", true);
	sResponse += Buttonhinzufuegen("1", "1", false);
	sResponse += Buttonhinzufuegen("2", "2", false);
	sResponse += Buttonhinzufuegen("3", "3", true);
	sResponse += Buttonhinzufuegen("0", "0", false);
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen'; \" value = \"Optionen\" / ></p>";
	sResponse += "<p>Taste:" + sCmd + "<BR>";
	sResponse += "<form action=\"?sCmd\" >";    // ?sCmd forced the '?' at the right spot  
	sResponse += "<BR>Brightness &nbsp;&nbsp";  // perhaps we can show here the current value
	sResponse += round(255 / 2.5);    // this is just a scale depending on the max value; round for better readability
	sResponse += " %";
	sResponse += "<BR>";
	sResponse += "<input style=\"width:200px; height:50px\" type=\"range\" name=\"=Slider1\" id=\"cmd\" value=\"";   // '=' in front of FUNCTION_200 forced the = at the right spot
	sResponse += Temporaererwert;
	sResponse += "\" min=0 max=250 step=1 onchange=\"showValue(points)\" />";
	sResponse += "<BR><BR>";
	sResponse += "<input type=\"submit\">"; //senden button
	sResponse += "</form>";
	sResponse += "<p>";
	sResponse += "<FONT SIZE=-2>";
	sResponse += "Gespeicherter wert: " + (String)Temporaererwert;
	sResponse += "<p>";
	sResponse += " Tastenmodus: " + (String)Tastenmodus;
	if (Tastenmodus == 4)
	{
		sResponse += "<p>";
		sResponse += "Modus information: " + (String)ModusInformationen(Temporaererwert);
	}
	if (Tastenmodus == 10)
	{
		sResponse += "<p>";
		sResponse += "creativ information: ";
		sResponse += CreativInformation(Temporaererwert);
	}
	sResponse += "</body></html>";
	sHeader = "HTTP/1.1 200 OK\r\n";
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";
}
void Seite_Optionen(String sCmd)
{
	if (Debugmodus) Serial.println("Optionenseite");
	sResponse = "<html><head><title>Optionen</title></head><body>";
	sResponse += Farben("#37f2e8", "#0e2aa5");
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Led Optionen</h1>";
	sResponse += "<FONT SIZE=-2>";
	sResponse += "<p>Taste:" + sCmd + "<BR>";
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "'; \" value = \"zurueck\" / >";
	sResponse += Buttonhinzufuegen("Esp Off", "Off", false);
	sResponse += Buttonhinzufuegen("Parameter Speichern", "speichern", false);
	sResponse += Buttonhinzufuegen("Parameter Laden", "laden", false);
	sResponse += Buttonhinzufuegen("Parameter syncen", "sync", false);
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen/explorer'; \" value = \"Explorer\" / ></p>";
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen/zeiten'; \" value = \"Schlafzeiten\" / >";
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen/pixel'; \" value = \"pixel\" / >";
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen/konsole'; \" value = \"Konsole\" / >";
	if (Ersetzebass)
	{
		sResponse += "<form action = \"optionen\"> <input type = \"submit\" name = \"pin\"style = \"BACKGROUND-COLOR: green;\"value = \"bassersatz\">";
	}
	else
	{
		sResponse += "<form action = \"optionen\"> <input type = \"submit\" name = \"pin\"style = \"BACKGROUND-COLOR: red;\"value = \"bassersatz\">";
	}
	if (Debugmodus)
	{
		sResponse += "<form action = \"optionen\"> <input type = \"submit\" name = \"pin\"style = \"BACKGROUND-COLOR: green;\"value = \"debugmodus\">";
	}
	else
	{
		sResponse += "<form action = \"optionen\"> <input type = \"submit\" name = \"pin\"style = \"BACKGROUND-COLOR: red;\"value = \"debugmodus\">";
	}
	if (Debugmodus_Internet)
	{
		sResponse += "<form action = \"optionen\"> <input type = \"submit\" name = \"pin\"style = \"BACKGROUND-COLOR: green;\"value = \"debugmodus_internet\">";
	}
	else
	{
		sResponse += "<form action = \"optionen\"> <input type = \"submit\" name = \"pin\"style = \"BACKGROUND-COLOR: red;\"value = \"debugmodus_internet\">";
	}
	sResponse += "</body></html>";
	sHeader = "HTTP/1.1 200 OK\r\n"; //setzt den cursor auf anfang und fängt eine neue zeile an
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";
}
void Seite_Pixelfaerben(String sCmd)
{
	if (Debugmodus) Serial.println("InternetPixelfärben");
	if (!sliderbool)Tastenmodus = 13;
	sResponse = "<html><head><title>Optionen</title></head><body>";
	sResponse += Farben("#37f2e8", "#0e2aa5");
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Led Pixel</h1>";
	sResponse += "<FONT SIZE=-2>";
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen'; \" value = \"zuruck\" / >";
	//		sResponse += Buttonhinzufuegen("Pixelposition", "pixposition", false);
	sResponse += Buttonhinzufuegen("ubertragen", "ubetragen", true);
	sResponse += "<FONT SIZE=-4>";
	sResponse += Buttonhinzufuegen("7", "7", false);
	sResponse += Buttonhinzufuegen("8", "8", false);
	sResponse += Buttonhinzufuegen("9", "9", false);
	sResponse += Buttonhinzufuegen("loeschen", "loeschen", true);
	sResponse += Buttonhinzufuegen("4", "4", false);
	sResponse += Buttonhinzufuegen("5", "5", false);
	sResponse += Buttonhinzufuegen("6", "6", true);
	sResponse += Buttonhinzufuegen("1", "1", false);
	sResponse += Buttonhinzufuegen("2", "2", false);
	sResponse += Buttonhinzufuegen("3", "3", true);
	sResponse += Buttonhinzufuegen("0", "0", false);
	sResponse += "<p>Taste:" + sCmd + "<BR>";
	sResponse += "<BR>Pixel: " + (String)pixelposition;
	sResponse += "<form action=\"?sCmd\" >";    // ?sCmd forced the '?' at the right spot  
	sResponse += "<BR>Einzelngruen &nbsp;&nbsp";  // perhaps we can show here the current value
	sResponse += round(einzelngruen[pixelposition]);    // this is just a scale depending on the max value; round for better readability
	sResponse += "<BR>";
	sResponse += "<input style=\"width:200px; height:50px\" type=\"range\" name=\"=Slider2\" id=\"cmd\" value=\"";   // '=' in front of FUNCTION_200 forced the = at the right spot
	sResponse += einzelngruen[pixelposition];
	sResponse += "\" min=0 max=250 step=1 onchange=\"showValue(points)\" />";
	sResponse += "<BR><BR>";
	sResponse += "<input type=\"submit\">"; //senden button
	sResponse += "</form>";
	sResponse += "<p>";

	sResponse += "<form action=\"?sCmd\" >";    // ?sCmd forced the '?' at the right spot  
	sResponse += "<BR>Einzelnrot &nbsp;&nbsp";  // perhaps we can show here the current value
	sResponse += round(einzelnrot[pixelposition]);    // this is just a scale depending on the max value; round for better readability
	sResponse += "<BR>";
	sResponse += "<input style=\"width:200px; height:50px\" type=\"range\" name=\"=Slider3\" id=\"cmd\" value=\"";   // '=' in front of FUNCTION_200 forced the = at the right spot
	sResponse += einzelnrot[pixelposition];
	sResponse += "\" min=0 max=250 step=1 onchange=\"showValue(points)\" />";
	sResponse += "<BR><BR>";
	sResponse += "<input type=\"submit\">"; //senden button
	sResponse += "</form>";
	sResponse += "<p>";
	sResponse += "<form action=\"?sCmd\" >";    // ?sCmd forced the '?' at the right spot  
	sResponse += "<BR>Einzelnblau &nbsp;&nbsp";  // perhaps we can show here the current value
	sResponse += round(einzelnblau[pixelposition]);    // this is just a scale depending on the max value; round for better readability
	sResponse += "<BR>";
	sResponse += "<input style=\"width:200px; height:50px\" type=\"range\" name=\"=Slider4\" id=\"cmd\" value=\"";   // '=' in front of FUNCTION_200 forced the = at the right spot
	sResponse += einzelnblau[pixelposition];
	sResponse += "\" min=0 max=250 step=1 onchange=\"showValue(points)\" />";
	sResponse += "<BR><BR>";
	sResponse += "<input type=\"submit\">"; //senden button
	sResponse += "</form>";
	sResponse += "<p>";
	sResponse += "</body></html>";
	sHeader = "HTTP/1.1 200 OK\r\n";
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";
}
void Seite_Explorer(String sCmd)
{
	sResponse = "<html><head><title>Optionen</title></head><body>";
	sResponse += Farben("#37f2e8", "#0e2aa5");
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Explorer</h1>";
	sResponse += "<FONT SIZE=-2>";
	sResponse += "<p>Taste:" + sCmd + "<BR>";
	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen'; \" value = \"zuruck\" / >";
	sResponse += Buttonhinzufuegen("Txt erstellen", "txterstellen", false);
	sResponse += Buttonhinzufuegen("Txt lesen", "txtlesen", false);
	sResponse += Buttonhinzufuegen("Txt Ergänzen", "txtaendern", false);
	sResponse += Buttonhinzufuegen("Verzeichnis", "Verzeichnis", false);
	sResponse += Buttonhinzufuegen("Txt loeschen", "txtloeschen", false);


	sResponse += "<form action = \"/get\">";
	sResponse += "input2: <input type = \"text\" name = \"input2\" placeholder = \"Eingabedatei\" >";
	sResponse += "<input type = \"submit\" value = \"Eingabe\">";
	sResponse += "</p>";

	sResponse += "</body></html>";
	sHeader = "HTTP/1.1 200 OK\r\n";
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";
}
void Seite_Zeiten()
{
	sResponse = "<html><head><title>Schlafzeiten</title></head><body>";
	sResponse += Farben("#37f2e8", "#0e2aa5");
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Schlafzeiten</h1>";
	sResponse += "<FONT SIZE=-2>";

	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen'; \" value = \"zurueck\" / ></p>";
	sResponse += Textfeldeinfuegen((String)Aufwachzeit, "Startzeit", "input3", "Uebernehmen", "number");
	sResponse += Textfeldeinfuegen((String)Einschlafzeit, "Schlafzeit", "input4", "Uebernehmen", "number");
}
void Seite_Konsole()
{
	sResponse = "<html><head><title>Konsole</title></head><body>";
	sResponse += Farben("#37f2e8", "#0e2aa5");
	sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
	sResponse += "<h1>Konsole</h1>";
	sResponse += "<FONT SIZE=-2>";

	sResponse += "<input type = \"button\" onclick = \"window.location.href = 'http://" + IPAdresse + "/optionen'; \" value = \"zuruck\" / >";

	sResponse += "<form action = \"/get\">";
	sResponse += "Befehl: <input type = \"text\" name = \"input1\" placeholder = \"z.B. b4 fuer Blau -> 4\" >";
	sResponse += "<input type = \"submit\" value = \"Submit\">";
	sResponse += "</p>";

	sResponse += "</body></html>";
	sHeader = "HTTP/1.1 200 OK\r\n";
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";

}
void printLocalTime() {
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		Serial.println("Failed to obtain time");
		return;
	}
	Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
	Serial.print("Day of week: ");
	Serial.println(&timeinfo, "%A");
	Serial.print("Month: ");
	Serial.println(&timeinfo, "%B");
	Serial.print("Day of Month: ");
	Serial.println(&timeinfo, "%d");
	Serial.print("Year: ");
	Serial.println(&timeinfo, "%Y");
	Serial.print("Hour: ");
	Serial.println(&timeinfo, "%H");
	Stunden = timeinfo.tm_hour;
	Minuten = timeinfo.tm_min;
	Serial.print("Hour (12 hour format): ");
	Serial.println(&timeinfo, "%I");
	Serial.print("Minute: ");
	Serial.println(&timeinfo, "%M");
	Serial.print("Second: ");
	Serial.println(&timeinfo, "%S");
	Serial.println("Test: " + (String)Stunden + ":" + (String)Minuten);
	Serial.println("Time variables");
	char timeHour[3];
	strftime(timeHour, 3, "%H", &timeinfo);
	Serial.println(timeHour);
	char timeWeekDay[10];
	strftime(timeWeekDay, 10, "%A", &timeinfo);
	Serial.println(timeWeekDay);
	Serial.println();
}
void deleteFile(fs::FS& fs, const char* path) {
	Serial.printf("Deleting file: %s\r\n", path);
	if (fs.remove(path)) {
		Serial.println("- file deleted");
	}
	else {
		Serial.println("- delete failed");
	}
}

void listDir(fs::FS& fs, const char* dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\r\n", dirname);
	File root = fs.open(dirname);
	if (!root) {
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(" - not a directory");
		return;
	}
	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels) {
				listDir(fs, file.path(), levels - 1);
			}
		}
		else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("\tSIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}
void SchreibeTxtFile(fs::FS& fs, const char* path, const char* message) {
	Serial.printf("Writing file: %s\r\n", path);

	File file = fs.open(path, FILE_WRITE);

	if (!file) {
		Serial.println("- failed to open file for writing");
		return;
	}
	if (file.println(message)) {
		Serial.println("- file written");
	}
	else {
		Serial.println("- write failed");
	}
	file.close();
}
void ErgaenzeTxtFile(fs::FS& fs, const char* path, String message) {
	{
		Serial.printf("Writing file: %s\r\n", path);

		File file = fs.open(path, FILE_APPEND);

		if (!file) {
			Serial.println("- failed to open file for writing");
			return;
		}
		if (file.println(message)) {
			Serial.println("- file written");
		}
		else {
			Serial.println("- write failed");
		}
		file.close();
	}
}
void LeseTxtFile(fs::FS& fs, const char* path) {
	Serial.printf("Reading file: %s\r\n", path);

	File file = fs.open(path);
	if (!file || file.isDirectory()) {
		Serial.println("- failed to open file for reading");
		return;
	}

	Serial.println("- read from file:");
	while (file.available()) {
		Serial.write(file.read());
	}
	Serial.println("- Ende");
	file.close();
}

void Seite_Nichtguelitg()
{
	sResponse = "<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server...</p></body></html>";
	sHeader = "HTTP/1.1 404 Not found\r\n";
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";
}
void Seite_Aktivezeit()
{
	Seite_Javascript();
	const String data = "4444";
	sResponse += "<html>\n";
	sResponse += "<style>\n";
	sResponse += "#div1{\n";
	sResponse += "width:400px;\n";
	sResponse += "margin:0 auto;\n";
	sResponse += "margin-top:130px;\n";
	sResponse += "font-size:600%;\n";
	sResponse += "color:powderblue;\n";
	sResponse += "}\n";
	sResponse += "</style>\n";
	sResponse += "<body onload='process()'>";
	sResponse += "<div id='div1'>" + data + "</div></body></html>";
	//Website += "<div id='div2'>" + data2 + "</div></body></html>";
	sResponse += "<a href=\"?pin=senden\"><button>senden</button></a><p>";

	sHeader = "HTTP/1.1 200 OK\r\n";
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/html\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";
}
void Seite_Javascript()
{
	if (Debugmodus)	Serial.println("javascribtcontent");
	sResponse = "<SCRIPT>\n";
	sResponse += "var xmlHttp=createXmlHttpObject();\n";
	sResponse += "function createXmlHttpObject(){\n";
	sResponse += "if(window.XMLHttpRequest){\n";
	sResponse += "xmlHttp=new XMLHttpRequest();\n";
	sResponse += "}else{\n";
	sResponse += "xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
	sResponse += "}\n";
	sResponse += "return xmlHttp;\n";
	sResponse += "}\n";
	sResponse += "\n";
	sResponse += "function response(){\n";
	sResponse += "xmlResponse=xmlHttp.responseXML;\n";
	sResponse += "xmldoc = xmlResponse.getElementsByTagName('data');\n";
	sResponse += "message = xmldoc[0].firstChild.nodeValue;\n";
	sResponse += "document.getElementById('div1').innerHTML=message;\n";
	//Javascript += "document.getElementById('div2').innerHTML=message;\n";
	sResponse += "}\n";

	sResponse += "function process(){\n";
	sResponse += "xmlHttp.open('PUT','xml',true);\n";
	sResponse += "xmlHttp.onreadystatechange=response;\n";
	sResponse += "xmlHttp.send(null);\n";
	sResponse += "setTimeout('process()',1000);\n";
	sResponse += "}\n";

	sResponse += "</SCRIPT>\n";
}
void XMLcontent() {
	//�bergibt den wert 133
	if (Debugmodus)Serial.println("Xmlcontent");
	sResponse = "<?xml version='1.0'?>";
	sResponse += "<data>";
	sResponse += (String)millis();
	sResponse += "</data>";
	sHeader = "HTTP/1.1 200 OK\r\n";
	sHeader += "Content-Length: ";
	sHeader += sResponse.length();
	sHeader += "\r\n";
	sHeader += "Content-Type: text/xml\r\n";
	sHeader += "Connection: close\r\n";
	sHeader += "\r\n";
	//server.send(200, "text/xml", XML);
}
String Farben(String Frontfarbe, String Hintergrundfarbe)
{
	String Antwort = "";
	Antwort = "<font color=\"" + Frontfarbe + "\"><body bgcolor=""\"" + Hintergrundfarbe + "\">";
	return Antwort;
}
String Buttonhinzufuegen(String Text, String Pinname, bool neuezeile)
{
	String Antwort = "";
	Antwort = "<a href=\"?pin=" + Pinname + "\"><button>" + Pinname + "</button></a>";
	if (neuezeile)
	{
		Antwort += "</p>";
	}
	return Antwort;
}
//Input types: text oder number
String Textfeldeinfuegen(String Nachrichtvorschau, String Nametextfeld, String SParam, String Bestaetigung, String InputType)
{
	String Antwort = "";
	Antwort = "<form action = \"/get\">";
	Antwort += Nametextfeld + ": <input type = " + InputType + " name = " + SParam + " placeholder = " + Nachrichtvorschau + " >";
	Antwort += "<form action = \"/get\">";
	Antwort += "<input type = \"submit\" value = " + Bestaetigung + ">";
	Antwort += "</p>";
	return Antwort;
	//sResponse += "<form action = \"/get\">";
	//sResponse += "input2: <input type = \"text\" name = \"input2\" placeholder = \"Eingabedatei\" >";
	//sResponse += "<input type = \"submit\" value = \"Eingabe\">";
	//sResponse += "</p>";
}
void VerarbeiteNachricht(String sCmd) //sCmd ist die Zahl oder anweisung
{
	if (sCmd.length() > 0)
	{
		if (sliderbool)
		{
			werteveraendern(sCmd.toInt());  //wenn ein wert per slider geändert wird, wird immer sofort aktuelisiert
			Temporaererwert = sCmd.toInt();
		}
		else
		{
			Pinzuweisen(sCmd);
		}
	}
}
int Werteabrufen()
{
	switch (Tastenmodus)
	{
	case 1: return FARBEBLAU; break;
	case 2: return FARBEGRUEN; break;
	case 3: return FARBEROT; break;
	case 4: return Modus; break;
	case 5: return Warte; break;
	case 6: return Vor; break;
	case 7: return Hinten; break;
	case 8: return Farbanzahl; break;
	case 9: return Randomwert; break;
		//	case 10: return ; break; //creativmodus
	case 11: return Bassfilter; break;
	case 12: return Bass; break;
	case 13: return pixelposition; break;
	case 14: return NUMPIXELS; break;
	case 15: return Hardwaremodus; break;
	default:
		break;
	}
}
void werteveraendern(int Neuezahl)
{
	if (Debugmodus)
	{
		Serial.println("Tastenmodus: " + Tastenmodus);
		Serial.println("Neuezahl: " + Neuezahl);
	}
	switch (Tastenmodus)
	{
	case 1: if (Neuezahl < 255) FARBEBLAU = Neuezahl; else FARBEBLAU = 255; break;
	case 2: if (Neuezahl < 255) FARBEGRUEN = Neuezahl; else FARBEGRUEN = 255; break;
	case 3: if (Neuezahl < 255)  FARBEROT = Neuezahl; else FARBEROT = 255; break;
	case 4: Modus = Neuezahl; break;
	case 5: if (Neuezahl > 0) Warte = Neuezahl; else Warte = 0; break;
	case 6: Vor = Neuezahl; break;
	case 7: Hinten = Neuezahl; break;
	case 8: if (Neuezahl < 100) Farbanzahl = Neuezahl; else Farbanzahl = 100; break;
	case 9: Randomwert = Neuezahl; break;
	case 10: CreativMode(Neuezahl); break;
	case 11: Bassfilter = Neuezahl; break;
	case 12: Bass = Neuezahl; break;
	case 13: pixelposition = Neuezahl; break;
	case 14: if (Neuezahl < 72) NUMPIXELS = Neuezahl; else NUMPIXELS = 72; pixels.updateLength(NUMPIXELS); break;
	case 15: Hardwaremodus = Neuezahl; break;
	default:
		break;
	}
}
void Pinzuweisen(String Taste)
{
	if (Debugmodus) Serial.println("Taste: " + Taste);
	if (Debugmodus) Serial.println("slider: " + (String)sliderbool);
	if (Taste == "Blau") Tastenmodus = 1;
	else if (Taste == "Gruen") Tastenmodus = 2;
	else if (Taste == "Rot") Tastenmodus = 3;
	else if (Taste == "Modus") Tastenmodus = 4;
	else if (Taste == "Warte") Tastenmodus = 5;
	else if (Taste == "Vor") Tastenmodus = 6;
	else if (Taste == "Hinter") Tastenmodus = 7;
	else if (Taste == "Farbanzahl") Tastenmodus = 8;
	else if (Taste == "Randomwerte") Tastenmodus = 9;
	else if (Taste == "CreativMode") Tastenmodus = 10;
	else if (Taste == "Frequenzfilter") Tastenmodus = 11;
	else if (Taste == "Bassersatz") Tastenmodus = 12;
	else if (Taste == "NumPixels") Tastenmodus = 14;
	else if (Taste == "FunktionsTaste") Tastenmodus = 15;
	else if (Taste == "txtaendern")
	{
		ErgaenzeTxtFile(SPIFFS, "/hello.txt", "Hello2 ");
	}
	if (Taste == "Blau" || Taste == "Gruen" || Taste == "Rot" || Taste == "Modus" || Taste == "Warte" || Taste == "Vor" || Taste == "Hinter" || Taste == "Farbanzahl" || Taste == "Randomwerte" || Taste == "Frequenzfilter" || Taste == "Bassersatz" || Taste == "FunktionsTaste" || Taste == "NumPixels")
	{
		Temporaererwert = Werteabrufen();
	}
	if (Taste == "txterstellen")
	{
		SchreibeTxtFile(SPIFFS, "/hello.txt", "Hello ");
	}
	if (Taste == "txtlesen") LeseTxtFile(SPIFFS, "/hello.txt");
	if (Taste == "Verzeichnis") listDir(SPIFFS, "/", 0);
	if (Taste == "txtloeschen") deleteFile(SPIFFS, "/hello.txt");
	if (Taste == "Off") Controllerausschalten(10);
	if (Taste == "laden") Ladeparameter();
	if (Taste == "speichern") Speicherparameter();
	if (Taste == "bassersatz")
	{
		if (Ersetzebass)
		{
			Ersetzebass = false;
		}
		else
		{
			Ersetzebass = true;
		}
	}
	if (Taste == "debugmodus")
	{
		if (Debugmodus)
		{
			Debugmodus = false;
		}
		else
		{
			Debugmodus = true;
		}

		Serial.println("Debug: " + (String)Debugmodus);
	}
	if (Taste == "debugmodus_internet")
	{
		if (Debugmodus_Internet)
		{
			Debugmodus_Internet = false;
		}
		else
		{
			Debugmodus_Internet = true;
		}
	}
	// if (Taste == "pixel") ();
	if (Taste == "ubetragen")
	{
		Serial.print("c0p" + (String)pixelposition + "r" + (String)einzelnrot[pixelposition] + "g" + (String)einzelngruen[pixelposition] + "b" + einzelnblau[pixelposition]);
	}
	if (Taste == "senden")
	{
		werteveraendern(Temporaererwert);
	}
	if (Taste == "1" || Taste == "2" || Taste == "3" || Taste == "4" || Taste == "5" || Taste == "6" || Taste == "7" || Taste == "8" || Taste == "9" || Taste == "0" || Taste == "loeschen")
	{
		Temporaererwert = ZahlenString(Taste, Temporaererwert);
	}
}
int ZahlenString(String Taste, int Zahl)
{
	int Wert = 0;
	String Zahlenstring;
	Zahlenstring = String(Zahl);
	if (Taste == "1" || Taste == "2" || Taste == "3" || Taste == "4" || Taste == "5" || Taste == "6" || Taste == "7" || Taste == "8" || Taste == "9" || Taste == "0")
	{
		Zahlenstring = Zahlenstring + Taste;
	}
	if (Taste == "loeschen")
	{
		if (Zahlenstring.length() != 0)
		{
			int num1 = Zahlenstring.length();
			Zahlenstring.remove(num1 - 1);
		}
		else
		{
			Zahlenstring = "0";
		}
	}
	Wert = Zahlenstring.toInt();
	return Wert;
}
String ModusInformationen(int Modus)
{
	switch (Modus)
	{
	case 0:
		return "Alle Leds nehmen gleichzeitig die gleiche Farbe an";    break;
	case 1:
		return "Alle Leds nehmen verzögert die gleiche Farbe an"; break;
	case 2:
		return "Alle leds werden nacheinander an/ausgeschaltet"; break;
	case 3:
		return "Alle Leds werden sofort an/aus geschalten"; break;
	case 4:
		return "Alle Leds werden abwechselnd heller/dunkler"; break;
	case 5:
		return "Erste Led wird gefärbt und wird weiter gereicht "; break;
	case 7:
		return "Es Werden zufällig generierte Leds vor/zurück geschoben"; break;
	case 8:
		return "Led wird nach Basstärke gefärbt(Weiß)"; break;
	case 9:
		return "Led wird nach Basstärke gefärbt(Bunt)"; break;
	case 10:
		return "Led wechselt die Farbe in rot/blau"; break;
	case 11:
		return "Zufällige farbige Streifen", "black", "white"; break;;
	case 12:
		return "Farbiger Bass Equalizer"; break;
	case 13:
		return "Bass Equalizer von grün nach rot"; break;
	case 14:
		return "Streifen die dunkler/heller werden"; break;
	case 15:
		return"Regenbogen Streifen"; break;
	case 16:
		return "Zufällige weiche Streifen(Farbig)"; break;
	case 17:
		return "Led wird Blockmässig aus/an geschalten"; break;
	case 18:
		return "Streifen die Abgedunkelt werden"; break;
	case 22:
		return "Bassabhängige Weiße Streifen"; break;
	case 23:
		return"Bassabhängige Weiße Streifen die abnehmen"; break;
	case 24:
		return "Bassabhängige Farbige Streifen"; break;
	case 25:
		return "Bassabhängige Farbige Streifen die abnehmen "; break;;
	case 26:
		return "Einzelne Leds werden abgedunkelt und erscheinen durch Bass "; break;
	case 27:
		return "Bassabhängige/bewegliche Spawner"; break;
	case 28:
		return "BassabhängigeSpawner"; break;
	case 29:
		return "Bassabhängige/bewegliche Leds"; break;
	case 30:
		return "Bassabhängige/bewegliche Spawnstreifen(Farblimit = 2500 "; break;
	case 31:
		return "Bassabhängige/bewegliche Spawnstreifen(Farblimit = 250 "; break;
	case 32:
		return "BassabhängigeSpawner"; break;
	case 34:
		return "Bassabhängige leds werden gespawnt und werden verlangsamt  "; break;
	case 35:
		return "Led wird Blockmässig aus/an geschalten"; break;
	case 36:
		return "Bass abhängige Wellenförmige Steifen"; break;
	case 37:
		return "Einzelne Leds werten ringförmig gefärbt"; break;
	case 38:
		return "Leds kreisen um einen Punkt"; break;
	case 39:
		return "Durch Bass wird eine weiße welle ausgelöst"; break;
	case 40:
		return "Led wird farbig blogmässig ausgeschalter"; break;
	case 41:
		return "Meteoritenmodus "; break;
	case 42:
		return "Es wird mittig->draussen led gefärbt "; break;
	case 43:
		return "Led wird 2 geteilt und per einzelne Farben gefärbt"; break;
	case 45:
		return "Speichert die aktuell angezeigte leds in die einzel arrays auf"; break;
	case 46:
		return "Led wird sofort an/aus gechaltet, in abhängig von Bass"; break;
	case 100:
		return "Led stoppen"; break;
	case 101:
		return "Led abdunkeln"; break;
	case 102:
		return "Leds vor verschieben"; break;
	case 103:
		return "Leds hinter verschieben"; break;
	case 104:
		return "Esp wird ausgeschaltet"; break;
	case 200:
		return"Listet das gesamte Array(Einzelwerte) auf"; break;
	case 201:
		return"löscht das Array"; break;
	case 202:
		return"Listet die zwischenwerte auf"; break;
	case 203:
		return "Leds Werte werden ins Array eingespeichert"; break;
	case 204:
		return "Esp wird in Deepsleep geschaltet"; break;
	default: return ""; break;
	}
}
String CreativInformation(int Modus)
{
	switch (Modus)
	{
	case 0: return"Leds können per USB einzeln gefärbt werden"; break;
	case 1: return "Debugmodus aktivieren/deaktivieren"; break;
	case 2: return "Startcounter neustarten"; break;
	case 3: return "Tiefenersatz aktivieren/deaktivieren"; break;
	case 4: return "Speichert die Parameter in EEPROM"; break;
	case 5: return "Ruft den EEPROM speicher auf"; break;
	case 6: return "Gibt die Programmversion zurück"; break;
	case 7: return "Reserviert zum Ändern der Pixelanzahl"; break;
	case 8: return "Debugmodus_Internet An/Aus"; break;
	case 9: return "Änder die Empfindlichkeit von Analogpins"; break;
	}
}
void VerarbeiteBefehl(String Befehl)
{
	int Buffer[20];
	int Signallange = 0;
	int errechneterwert = 0;
	int Buchstabe = 0;
	int Buchstabe2 = 0;

	while (Befehl.length() > 0)
	{
		char Zeichen = Befehl.charAt(0);
		if (Debugmodus) Serial.println("Zeichen: " + String(Zeichen));
		if (Zeichen >= 65 && Zeichen <= 90 || Zeichen >= 97 && Zeichen <= 122) // Wenn es ein Buchstabe ist
		{
			if (Buchstabe == 0)
			{
				Buchstabe = Zeichen;
			}
			else
			{
				errechneterwert = 0;
				int multi = 1;
				while (Signallange != 0)
				{
					if ((Buffer[Signallange - 1] - 48) != 0)
					{
						errechneterwert += (Buffer[Signallange - 1] - 48) * multi;
					}
					Signallange = Signallange - 1;
					multi = multi * 10;
				}
				Signallange = 0;
				Wertezuordnen(Buchstabe, errechneterwert);
				Buchstabe = 0;
				Buchstabe2 = Zeichen;
				Serial.println("Zwei Buchstaben erkannt");
			}
		}
		else if (Zeichen >= 48 && Zeichen <= 57) //Wenn es eine Zahl ist
		{
			if (Buchstabe2 != 0)
			{
				Buchstabe = Buchstabe2;
				Buchstabe2 = 0;
			}
			Buffer[Signallange] = Zeichen;
			Signallange++;
		}
		Befehl.remove(0, 1); //Kürze den String um 1 Stelle
		if (Debugmodus)
		{
			Serial.println("signallange:"); Serial.println(Signallange);
			Serial.println("Buffer: "); Serial.println(Buffer[Signallange]);
			Serial.println("Befehl: " + String(Befehl));
		}
	}
	errechneterwert = 0;
	int multi = 1;
	while (Signallange != 0)
	{
		if ((Buffer[Signallange - 1] - 48) != 0)
		{
			errechneterwert += (Buffer[Signallange - 1] - 48) * multi;
		}
		Signallange = Signallange - 1;
		multi = multi * 10;
	}
	Signallange = 0;
	Wertezuordnen(Buchstabe, errechneterwert);
	Buchstabe = 0;


}
unsigned long Zeitumrechnen(unsigned short sekunden, unsigned short minuten, unsigned short stunden)
{
	return (sekunden * 1000 + minuten * 60000 + stunden * 3600000);
}
void Berechneschlafzeit()
{
	short Dauer;
	Dauer = (24 - Einschlafzeit) + Aufwachzeit;
	Schlafdauer = 60 * Dauer; //Schlafdauer in Minuten
	unsigned long Zeit = Zeitumrechnen(Sekunden, Minuten, Stunden);
	if (Debugmodus)	Serial.println("Aktuelle Zeit: " + (String)Zeit);
	unsigned long Ziel = Zeitumrechnen(0, 0, Einschlafzeit);
	if (Debugmodus)	Serial.println("Ziel Zeit: " + (String)Ziel);
	long Dif = Ziel - Zeit;
	if (Debugmodus)	Serial.println("Dif: " + (String)Dif);
	if (Dif < 0)
	{
		if (Debugmodus)	Serial.println("ESP wird in kürze Ausgeschaltet (1 Min)");
		Schlafenszeit = millis() + 60000;
	}
	else
	{
		Schlafenszeit = millis() + (unsigned long)Dif;
	}
}