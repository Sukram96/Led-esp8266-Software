#include <SerialEsp.h>

#include <Adafruit_NeoPixel.h>

#define PIN         D2
#define NUMPIXELS 195


int FARBEROT = 0;
int FARBEBLAU =250;
int FARBEGRUEN =20;
int Modus =8;
int Warte = 0;
int Vor = 2;
int Hinten = 0;
int Bassfilter = 2;
int Randomwert = 240;
short  nfarben_m6 =10; 
bool Debugmodus = false;
bool submodus = true;
bool Startecounter = false;

//Led startanfang 
short Startanfang =0;





int Bass = 0;


short Zwischenspeicher[200];
double zwischenrot = 0;
double zwischenblau = 0;
double zwischengruen = 0;



int groessterwert = 0;

short Programmmodus = 1;
short einzelnrot[NUMPIXELS];
short einzelnblau[NUMPIXELS];
short einzelngruen[NUMPIXELS];
short arraygruen[200];
short arrayrot[200];
short arrayblau[200];
double GenauePosition[200];
short Position[200];
//wird für zwischenwerte benötigt
short Bufferlocal[200];
double Geschwindigkeit[200];
int Tastenmodus = 0;
int Wert = 0;
//Laufzeit von Esp
int Zeit = 0;
//zeit, wann des nächste programm starten soll
int Programmzeit = 1000;
bool zweiterbuchstabe = false;
int Zweiterbuchstabe = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);
SerialEsp Daten = SerialEsp();

void setup()
{
	Serial.begin(9600);
	  pixels.begin();

	Position[0] = 0;
	Bufferlocal[0] = 0;
	arraygruen[0] = 250;
	Position[1] = 74;
	Bufferlocal[1] = 0;
	arrayblau[1] = 250;
	Einschlagberechnen();
	while (arraygruen[0] != 0)
	{
		Einschlagberechnen();
	}
	Warte = 0;

}
void loop()
{

	Programm();
	if (Serial.available() != 0)
	{
		aktualisierewerte();
  }


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
  case 40: Modus40(); break;
  case 100: Modus100(); break;
  case 101: LedKetteMinus(); Arraylicht(); break;
  case 102: Modus102(); break;
  case 103: Modus103(); break;
	case 200: LedArrayauflisten(); break;
	case 201: LEDARRAYReset(); break;
	case 202: ZeigeZwischenwerte();
	case 203: Ledarrayausbuffer(); break;
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
  Lichter(FARBEROT, FARBEBLAU, FARBEGRUEN, Warte,0,200);
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
	  Programm();
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
		Lichtersofort(zwischenrot*i, zwischenblau*i, zwischengruen*i);
		delay(Warte);
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
	}
	for (int i = 100; i != 1; i--)
	{
		Lichtersofort(zwischenrot*i, zwischenblau*i, zwischengruen*i);
		delay(Warte);
		Programm();
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
	for (int a = 1; a < nfarben_m6 + 1; a++)
	{
		Position[a] = (NUMPIXELS / nfarben_m6) * a;
	}
	for (int i = 0; i < nfarben_m6; i++)
	{
		Zwischenspeicher[i] = random(NUMPIXELS);
	}
	for (int i = 0; i < nfarben_m6; i++)
	{
		einzelnrot[i] = random(Randomwert);
		einzelnblau[i] = random(Randomwert);
		einzelngruen[i] = random(Randomwert);
	}
	while (Modus == 6)
	{
		for (int i = 0; i < NUMPIXELS; i++)
		{
			for (int e = 0; e < nfarben_m6; e++)
			{
				if (Zwischenspeicher[e] == i)
				{
					Zwischenspeicher[e] = random(NUMPIXELS);
					for (int b = 0; b < nfarben_m6; b++)
					{
						einzelnrot[b] = random(Randomwert);
						einzelnblau[b] = random(Randomwert);
						einzelngruen[b] = random(Randomwert);
					}
				}
			}
			for (int a = 0; a != nfarben_m6; a++)
			{

				pixels.setPixelColor(Position[a] + Startanfang, pixels.Color(einzelnrot[a], einzelnblau[a], einzelngruen[a]));
			}
			pixels.show();
			delay(Warte);
			for (int e = 0; e < nfarben_m6; e++)
			{
				Position[e]++;
				if (Position[e] == NUMPIXELS)
				{
					Position[e] = 0;
				}
			}
			Programm();
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
		Programm();
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
		for (int i = 0; i < Bassfilter; i++)
		{
			Bass = Lesenalogpin(0) ;
			if (Bass >= groessterwert)
			{
				groessterwert = Bass;
			}
		}	
		Bass = groessterwert;
		groessterwert = 0;
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
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
		delay(0);
	}
}
void Modus9()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	while (Modus == 9)
	{
		for (int i = 0; i < Bassfilter; i++)
		{
			Bass = Lesenalogpin(0);
			if (Bass >= groessterwert)
			{
				groessterwert = Bass;
			}
		}
		Bass = groessterwert;
		groessterwert = 0;
		Lichtersofort(Bass*zwischenrot, Bass*zwischenblau, Bass*zwischengruen);
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
		}
		delay(0);
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
		Programm();
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
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
		Zwischenspeicher[0] = random(3);
		Zwischenspeicher[1] = random(255);
		Zwischenspeicher[2] = random(2);
		if (Zwischenspeicher[2] == 0)
		{
			for (int i = 0; i < Zwischenspeicher[1]; i++)
			{
				switch (Zwischenspeicher[0])
				{
				case 0: zwischenblau++; if (zwischenblau > 250) zwischenblau = 250; break;
				case 1: zwischenrot++; if (zwischenrot > 250) zwischenrot = 250; break;
				case 2: zwischengruen++; if (zwischengruen > 250) zwischengruen= 250; break;
				default:
					break;
				}
				delay(Warte);
				Lichthinzufuegen(1, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);
			
				
			}
		}
		else
		{
			for (int i = 0; i < Zwischenspeicher[1]; i++)
			{

				switch (Zwischenspeicher[0])
				{
				case 0: zwischenblau--; if (zwischenblau < 0) zwischenblau = 0; break;
				case 1: zwischenrot--; if (zwischenrot < 0) zwischenrot = 0; break;
				case 2: zwischengruen--; if (zwischengruen < 0) zwischengruen = 0; break;
				default:
					break;
				}
				delay(Warte);
				Lichthinzufuegen(1, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);

			}
		}


	}
}



void Modus12()
{
	while (Modus == 12)
	{
		for (int i = 0; i < Bassfilter; i++)
		{
			Bass = Lesenalogpin(0);
			if (Bass >= groessterwert)
			{
				groessterwert = Bass;
			}
		}
		Bass = groessterwert;
		groessterwert = 0;
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
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
		delay(0);
		pixels.show();
	}

}
void Modus13()
{
	while (Modus == 13)
	{
	
		for (int i = 0; i < Bassfilter; i++)
		{
			Bass = Lesenalogpin(0);
			if (Bass >= groessterwert)
			{
				groessterwert = Bass;
			}
		}
		Bass = groessterwert;
		groessterwert = 0;
		zwischengruen = 200;
		zwischenrot = 0;
		for (int a = 0; a != NUMPIXELS; a++)
		{
			if (Bass > a)
			{
				if (a < NUMPIXELS/2)
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
		delay(0);
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}

	}

}
void Modus14()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN,NUMPIXELS);
	for (int i = 0; i < NUMPIXELS; i++)
	{
		Lichthinzufuegen(i, zwischenrot*i, zwischenblau*i, zwischengruen*i);
	}
	while (Modus == 14)
	{
		Streifen(true);
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			LEDARRAYReset();

			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, NUMPIXELS);

			for (int i = 0; i < NUMPIXELS; i++)
			{
				Lichthinzufuegen(i, zwischenrot*i, zwischenblau*i, zwischengruen*i);
			}
			
		}
	}
}

void Modus15()
{
	LEDARRAYReset();
	Errechnezwischenwerte(250, 250, 250, 66);
	for (int i = 0; i < 66; i++)
	{
		einzelnrot[i] = i *zwischenrot;
	}
	for (int i = 66; i < 132; i++)
	{
		einzelnrot[i] = einzelnrot[i-1] -zwischenrot;
		if (einzelnrot[i] <= 0)
			einzelnrot[i] = 0;
	}

	for (int i = 66; i < 132; i++)
	{
		einzelnblau[i] = (i-66) *zwischenblau;
	}
	for (int i = 132; i < 200; i++)
	{
		einzelnblau[i] = einzelnblau[i - 1] - zwischenblau;
		if (einzelnblau[i] <= 0)
			einzelnblau[i] = 0;
	}
	for (int i = 132; i < 200; i++)
	{
		einzelngruen[i] = (i - 132) *zwischenblau;
	}
	einzelngruen[0] = 250;
	for (int i = 1; i < 66; i++)
	{
		einzelngruen[i] = einzelngruen[i - 1]- zwischengruen;
		if (einzelngruen[i] <= 0)
			einzelngruen[i] = 0;
	}


 
  while (Modus == 15)
  {
	  Streifen(true);
	  Programm();
    if (Serial.available() != 0)
    {
      aktualisierewerte();

    }
  }
}
void Modus16()
{
	zwischenblau = 0; zwischengruen = 0; zwischenrot = 0;
	while (Modus == 16)
	{
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
		}
		Zwischenspeicher[0] = random(3);
		Zwischenspeicher[1] = random(Randomwert);
		Zwischenspeicher[2] = random(2);
		if (Zwischenspeicher[2] == 0)
		{
			for (int i = 0; i < Zwischenspeicher[1]; i++)
			{
				switch (Zwischenspeicher[0])
				{
				case 0: zwischenblau++; if (zwischenblau > FARBEBLAU)  zwischenblau = FARBEBLAU; break;
				case 1: zwischenrot++; if (zwischenrot > FARBEROT) zwischenrot = FARBEROT; break;
				case 2: zwischengruen++; if (zwischengruen > FARBEGRUEN) zwischengruen = FARBEGRUEN; break;
				default:
					break;
				}
				Lichthinzufuegen(1, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);
			}
		}
		else
		{
			for (int i = 0; i < Zwischenspeicher[1]; i++)
			{

				switch (Zwischenspeicher[0])
				{
				case 0: zwischenblau--; if (zwischenblau < 0) zwischenblau = 0; break;
				case 1: zwischenrot--; if (zwischenrot < 0) zwischenrot = 0; break;
				case 2: zwischengruen--; if (zwischengruen < 0) zwischengruen = 0; break;
				default:
					break;
				}
				delay(Warte);
				Lichthinzufuegen(1, zwischenrot, zwischenblau, zwischengruen);
				Streifen(false);

			}
		}
	}
}
void Modus17()
{
	if (Warte == 0)Warte = 1;
	for (int i = 0; i < nfarben_m6; i++)
	{
		Zwischenspeicher[i] = 0;
	}
	int Abstand = NUMPIXELS / nfarben_m6;
	while (Modus ==17)
	{
		for (int e = 0; e < nfarben_m6; e++)
		{
		Zwischenspeicher[random(nfarben_m6)] = 1;
		 Zwischenspeicher[random(nfarben_m6)] = 0;
		}
		 
		for (int i = 0; i < nfarben_m6; i++)
		{
			if(Zwischenspeicher[i] == 1)
				for (int a = 0; a < Abstand; a++)
				{
					Lichthinzufuegen(a + i*Abstand, FARBEROT, FARBEBLAU, FARBEGRUEN);
				}
			else	
				for (int a = 0; a < Abstand; a++)
			{
				Lichthinzufuegen(a + i*Abstand, 0, 0, 0);
			}
		}
		Arraylicht();
		delay(Warte);
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Abstand = NUMPIXELS / nfarben_m6;
			if (Warte == 0)Warte = 1;
		}
	}
}

void Einschlagberechnen()
{
  for (int a = 0; a != nfarben_m6; a++)
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
 
  Arraylicht();
}
void Einschlagberechneneinmalig( int Berrechnearray)
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


  for (int a = 0; a != nfarben_m6; a++)
  {
	  Position[a] = random(0, NUMPIXELS);
	  Bufferlocal[a] = 0;
	  switch (random(0, 3))
	  {
	  case 0: arrayrot[a] = random(Randomwert)*zwischenrot; break;
	  case 1: arraygruen[a] = random(Randomwert)*zwischengruen; break;
	  case 2: arrayblau[a] = random(Randomwert)*zwischenblau; break;
	  default: break;
	  }
  }
  while (Modus == 18)
  {
    Einschlagberechnen();
	delay(Warte);
   for (int a = 0; a != nfarben_m6; a++)
    {
      if (arrayblau[a] <= 0 && arraygruen[a] <= 0 && arrayrot[a] <= 0  )
      {
        Bufferlocal[a] = 0;
        Position[a] = random(0, NUMPIXELS);
        switch (random(0, 3))
        {
          case 0: arrayrot[a] = random(Randomwert)*zwischenrot; break;
          case 1: arraygruen[a] = random(Randomwert)*zwischengruen; break;
          case 2: arrayblau[a] = random(Randomwert)*zwischenblau; break;
          default:; break;

        }
      }
    }
   Programm();
    if (Serial.available() != 0)
    {
      aktualisierewerte();
	  Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
    }
  }
}
void Modus19()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);

	while (Modus == 19)
	{
		Einschlagberechnen(); delay(Warte);
		for (int a = 0; a != nfarben_m6; a++)
		{
			if (arrayblau[a] <= 0 && arraygruen[a] <= 0 && arrayrot[a] <= 0)
			{
				Bufferlocal[a] = 0;
				Position[a] = random(0, 200);
				switch (random(0, 3))
				{
				case 0: arrayrot[a] = random(0, Lesenalogpin(0))*zwischenrot; break;
				case 1: arraygruen[a] = random(0, Lesenalogpin(0))*zwischengruen; break;
				case 2: arrayblau[a] = random(0, Lesenalogpin(0))*zwischenblau; break;
				default:; break;
				}
			}
		}
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 250);
		}
	}
}
void Modus20()
{
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	short Randomwert2 = 0;
	for (int a = 0; a < NUMPIXELS; a++)
	{
		Randomwert2 = random(Randomwert);
		Lichthinzufuegen(a, zwischenrot*Randomwert2, zwischenblau*Randomwert2, zwischengruen*Randomwert2);
	}
	while (Modus == 20)
	{

		for (int a = 0; a < NUMPIXELS; a++)
		{
			if (einzelnrot[a] <= 1 && einzelngruen[a] <= 1 && einzelnblau[a] <= 1)
			{
				Randomwert2 = random(Randomwert);
				Lichthinzufuegen(a, zwischenrot*Randomwert2, zwischenblau*Randomwert2, zwischengruen*Randomwert2);
			}
		}
		LedKetteMinus();
		Arraylicht();
		Programm();
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
 
  if(Vor == 0)
  {
  Lichthinzufuegen(NUMPIXELS-1, FARBEROT, FARBEBLAU, FARBEGRUEN);
  Streifen(false);
  Lichthinzufuegen(NUMPIXELS-1, 0, 0, 0);
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
 
  short Bass = Lesenalogpin(0);
  Lichthinzufuegen(0, Bass, Bass, Bass);
  Streifen(false);
}
void Modus23()
{
   short Bass = Lesenalogpin(0);
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
  Errechnezwischenwerte(FARBEROT,FARBEBLAU,FARBEGRUEN,NUMPIXELS);
  short Bass = 0;
  while (Modus == 24)
  {
  Bass = Lesenalogpin(0);
  Lichthinzufuegen(0, Bass*zwischenrot, Bass*zwischenblau, Bass*zwischengruen);
  Streifen(false);
  Programm();
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
		Bass = Lesenalogpin(0);
		Lichthinzufuegen(0, Bass*zwischenrot, Bass*zwischenblau, Bass*zwischengruen);
		LedKetteplusverkleinern(false);
		Arraylicht();
		delay(Warte);
		Programm();
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
				Randomwert = random(Lesenalogpin(0));
				Lichthinzufuegen(a, zwischenrot*Randomwert, zwischenblau*Randomwert, zwischengruen*Randomwert);
			}
		}
		LedKetteMinus();
		Arraylicht();
		Programm();
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
	short Abstaende = NUMPIXELS / nfarben_m6;

	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	while (Modus == 27)
	{

		Randomwert = random(Lesenalogpin(0));

			for (int a = 0; a < nfarben_m6; a++)
			{
				if (zwischenblau*Randomwert > einzelnblau[0])
				{
					Lichthinzufuegen(Position[0], zwischenrot*Randomwert, zwischenblau*Randomwert, zwischengruen*Randomwert);
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
		Programm();		
		Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
		Abstaende = NUMPIXELS / nfarben_m6;
		if (nfarben_m6 == 1) Abstaende = 0;
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
			Abstaende = NUMPIXELS / nfarben_m6;
			if (nfarben_m6 == 1) Abstaende = 0;
		}
	}
}

void Modus28()
{
	short Random = 0;
	Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	short Abstand = NUMPIXELS / nfarben_m6;
	while (Modus == 28)
	{


		for (int i = 0; i < nfarben_m6; i++)
		{
			Random = random(Lesenalogpin(0));
			if (Random != 0)
			{
				Lichthinzufuegen(Abstand*i, zwischenrot*Random, zwischenblau*Random, zwischengruen*Random);
			}
		}
		LedKetteplusverkleinern(false);
		Arraylicht();
		delay(Warte);
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
			Abstand = NUMPIXELS / nfarben_m6;
			
		}
	}
}
void Modus29()
{
	int bass;
	for (int i = 0; i < nfarben_m6; i++)
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
		
		for (int i = 0; i < nfarben_m6; i++)
		{
			einzelnblau[Position[i]] = 0;
			einzelngruen[Position[i]] = 0;
			einzelnrot[Position[i]] = 0;
		}
		bass = Lesenalogpin(0);
		for (int i = 0; i < nfarben_m6; i++)
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
					if (-(Geschwindigkeit[i]) < bass*0.003)
						Geschwindigkeit[i] = -((random(80, 120) / 50)*(bass*0.003));
				}
				else
				{
					if (Geschwindigkeit[i] < bass*0.003)
						Geschwindigkeit[i] = (random(80, 120) / 50) * (bass*0.003);
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
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			for (int i = 0; i < nfarben_m6; i++)
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
	Serial.println("Modus beeendet");

}
void Modus30()
{
	int bass;
for (int i = 0; i < nfarben_m6; i++)
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
		while (Modus == 30)
		{
			//Programm(5000);
			LedKetteMinus();
			bass = Lesenalogpin(0);
			for (int i = 0; i < nfarben_m6; i++)
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
						if (-(Geschwindigkeit[i]) < bass*0.003)
						{
							Geschwindigkeit[i] = -((random(80, 120) / 50)*(bass*0.003));
							if (Geschwindigkeit[i] > 1) Geschwindigkeit[i] = 1;
						}
					}
					else
					{
						if (Geschwindigkeit[i] < bass*0.003)
						{
							Geschwindigkeit[i] = (random(80, 120) / 50) * (bass*0.003);
							if (Geschwindigkeit[i] < -1) Geschwindigkeit[i] = -1;
						}

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

					
					GenauePosition[i] = random(NUMPIXELS);
				}
				ArrayFarbeMinus(i);
				Position[i] = (int)GenauePosition[i];
				if (einzelnblau[Position[i]] == 0)
				{
					einzelnblau[Position[i]] = arrayblau[i];
				}
				else if(einzelnblau[Position[i]] < 2000)
				{
					einzelnblau[Position[i]] = (arrayblau[i] + einzelnblau[Position[i]]) ;
				}
				if (einzelngruen[Position[i]] == 0)
				{
					einzelngruen[Position[i]] = arraygruen[i];
				}
				else if(einzelngruen[Position[i]] < 2000)
				{
					einzelngruen[Position[i]] = (arraygruen[i] + einzelngruen[Position[i]]);
				}
				if (einzelnrot[Position[i]] == 0)
				{
					einzelnrot[Position[i]] = arrayrot[i];
				}
				else if(einzelnrot[Position[i]] < 2000)
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
			Programm();
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
	for (int i = 0; i < nfarben_m6; i++)
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
		while (Modus == 31)
		{
			//Programm(5000);
			LedKetteMinus();
			bass = Lesenalogpin(0);
			for (int i = 0; i < nfarben_m6; i++)
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
						if (-(Geschwindigkeit[i]) < bass*0.003)
							Geschwindigkeit[i] = -((random(80, 120) / 50)*(bass*0.003));
					}
					else
					{
						if (Geschwindigkeit[i] < bass*0.003)
							Geschwindigkeit[i] = (random(80, 120) / 50) * (bass*0.003);
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
			Programm();
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
	short Abstaende = NUMPIXELS / nfarben_m6;
	for (int i = 0; i < nfarben_m6; i++)
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

		Randomwert = random(Lesenalogpin(0));

		for (int a = 0; a < nfarben_m6; a++)
		{
			Lichthinzufuegen(Position[0], (arrayrot[a]*Randomwert),arrayblau[a]*Randomwert, arraygruen[a]*Randomwert);
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
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Abstaende = NUMPIXELS / nfarben_m6;
			if (nfarben_m6 == 1) Abstaende = 0;
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
			for (int i = 0; i < nfarben_m6; i++)
			{
				GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
				Position[i] = (int)GenauePosition[i];
				Arraylicht();
				for (int a = 0; a < nfarben_m6; a++)
				{
					if (Geschwindigkeit[a] != 0)
						pixels.setPixelColor(Position[a] + Startanfang, arrayrot[a], arrayblau[a], arraygruen[a]);
				}
				Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;
				if (Geschwindigkeit[i] == 0)
				{
					Einschlagberechneneinmalig(i);
					int bass =	Lesenalogpin(0);
					if (Bufferlocal[i] == 0 && bass >50)
					{
						if (millis() > Zeit)
						{
							Zeit = millis()+150;
							Position[i] = 0;
							Geschwindigkeit[i] = Geschwindigkeit[i] = (random(80, 120) / 50) * (bass*0.02);
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
	ARRAYFARBENResest();
	while(Modus == 34)
	{
		for (int i = 0; i < nfarben_m6; i++)
		{
			GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
			Position[i] = (int)GenauePosition[i];
			Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;
			
			
			if (arraygruen[i] == 0 && arrayrot[i] == 0 && arrayblau[i]== 0)
			{
				Bass  =	Lesenalogpin(0);
				if (millis() > Zeit&& Bass >0)
				{
				  
					Zeit = millis() + 100;
					arraygruen[i] = random(250);
					arrayrot[i] = random(250);
					arrayblau[i] = random(250);
				//	int ges= random(0,Bass);
			//		Geschwindigkeit[i] = Geschwindigkeit[i] = (double)ges / 50;
					Geschwindigkeit[i] = (double) Bass/40;
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
	for (int i = 0; i < nfarben_m6; i++)
	{
		Zwischenspeicher[i] = 0;
	}
	int Abstand = NUMPIXELS / nfarben_m6;
	while (Modus == 35)
	{
		for (int e = 0; e < nfarben_m6; e++)
		{
			Zwischenspeicher[random(nfarben_m6)] = 1;
			Zwischenspeicher[random(nfarben_m6)] = 0;
		}
		Bass = Lesenalogpin(0);
		if (Bass > 5)
		{
			for (int i = 0; i < nfarben_m6; i++)
			{
				if (Zwischenspeicher[i] == 1)
					for (int a = 0; a < Abstand; a++)
					{

						Lichthinzufuegen(a + i*Abstand, FARBEROT, FARBEBLAU, FARBEGRUEN);
					}
				else
					for (int a = 0; a < Abstand; a++)
					{
						Lichthinzufuegen(a + i*Abstand, 0, 0, 0);
					}
			}
		}
		Arraylicht();
		delay(Warte);
		Programm();
		if (Serial.available() != 0)
		{
			aktualisierewerte();
			Abstand = NUMPIXELS / nfarben_m6;
			if (Warte == 0)Warte = 1;
		}
	}
}
void Modus40()
{
	Bufferlocal[0] = 0;
	ARRAYFARBENResest();
	while (Modus == 40)
	{
		for (int i = 0; i < 5; i++)
		{
			arraygruen[i] = random(240);
			arrayrot[i] = random(240);
			arrayblau[i] = random(240);
			Geschwindigkeit[i] = random(1, 4);
			Position[i] = 0;
		}
		while (true)
		{
			for (int i = 0; i < nfarben_m6; i++)
			{
				GenauePosition[i] = ErrechneGenaueFarbposition(GenauePosition[i], Geschwindigkeit[i]);
				Position[i] = (int)GenauePosition[i];
				Arraylicht();
				for (int a = 0; a < nfarben_m6; a++)
				{
					if(Geschwindigkeit[a] != 0)
					pixels.setPixelColor(Position[a] + Startanfang, arrayrot[a], arrayblau[a], arraygruen[a]);
				}
				Geschwindigkeit[i] = Geschwindigkeit[i] * 0.98;					
				if (Geschwindigkeit[i] == 0)
				{
					Einschlagberechneneinmalig(i);
					if (Bufferlocal[i] == 0)
					{
						Position[i] = 0;
						Geschwindigkeit[i] = Geschwindigkeit[i] = (random(10, 240) / 50);
						arraygruen[i] = random(240);
						arrayrot[i] = random(240);
						arrayblau[i] = random(240);
						GenauePosition[i] = 0;
					}
				}
				if (Geschwindigkeit[i] <= 0.05 && Bufferlocal[i] == 0)
				{
					Bufferlocal[i] = 1;
					Geschwindigkeit[i] = 0;
				}
			}
			Programm();
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

			pixels.show();
			if (Serial.available() != 0)
			{
				aktualisierewerte();
				break;
			}
			delay(Warte);
		}
	}
}

void Modus100()
{
  Arraylicht();
}
void Modus102()
{
	LedKetteVor(true);
	Modus = 100;
}
void Modus103()
{
	LedKetteHinten(true);
	Modus= 100;
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
	for (int i= 0; i < NUMPIXELS; i++)
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
void Ledarrayausbuffer()
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
	//	Serial.print("Wert pixel:");
		//Serial.println(pixels.getPixelColor(i));
		int led = pixels.getPixelColor(i);
		if (led >= 65536) { einzelnrot[i] = led / 65536; led = led -(65536 * einzelnrot[i]); }
		else einzelnrot[i] = 0;
		//Serial.print("einzelrot:");
		//Serial.println(einzelnrot[i]);

		if (led >= 256) { einzelnblau[i] = led / 256; led = led - (256 * einzelnblau[i]); }
		else einzelnblau[i] = 0;
		//Serial.print("einzelblau");
		//Serial.println(einzelnblau[i]);

		if (led >= 1) { einzelngruen[i] = led; }
		else einzelngruen[i] = 0;
		//Serial.print("einzelgruen:");
		//Serial.println(einzelngruen[i]);

	}

	Modus = 100;
}
void ZeigeZwischenwerte()
{
	for (int a = 0; a != NUMPIXELS; a++)
	{
		if(arrayrot[a] != 0 || arrayblau[a] != 0 ||arraygruen[a] != 0)
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
/*void Programm(int neuanfang)
{
	if (analogRead(0) >= 10)
	{
		if (Startecounter == false)
		{
			Zwischenspeicher[0] = 0;
			Startecounter = true;
		}
	}
	if (Startecounter)
	{

		if (millis() > Zeit + neuanfang)
		{
			Zeit = millis();
			switch (Zwischenspeicher[0])
			{
			case 0: FARBEBLAU--; if (FARBEBLAU <= 0) Zwischenspeicher[0] = 1; break;
			case 1: FARBEROT++; if (FARBEROT >= 250) Zwischenspeicher[0] = 2; break;
			case 2: FARBEGRUEN--; if (FARBEGRUEN <= 0) Zwischenspeicher[0] = 3; break;
			case 3: FARBEBLAU++; if (FARBEBLAU >= 250) Zwischenspeicher[0] = 4; break;
			case 4: FARBEROT--; if (FARBEROT <= 0) Zwischenspeicher[0] = 5; break;
			case 5: FARBEGRUEN++; if (FARBEGRUEN >= 250) Zwischenspeicher[0] = 0; break;
			default:
				break;
			}
		}
		Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	}
}*/
void Programm()
{
	if (Startecounter)
	{

		if (millis() > Zeit + Programmzeit)
		{
			Zeit = millis();
			switch (Programmmodus)
			{
			case 0: Modus = 1; Warte = 0; FARBEBLAU = 250; FARBEGRUEN = 0; FARBEROT = 0; Programmzeit = 300; break;
			case 1: FARBEBLAU = 0; FARBEGRUEN = 250; break;
			case 2: FARBEGRUEN = 0; FARBEROT = 250; break;
			case 3: Modus = 8; Programmzeit = 8000; break;
			case 4: Modus = 9; break;
			case 5:  FARBEROT = 0; FARBEGRUEN = 250; Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200); break;
			case 6: FARBEBLAU = 250; FARBEGRUEN = 0; Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200); break;
			case 7: FARBEBLAU = 250; Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200); break;
			case 8: Programmzeit = 100; break; //Modus = 18; nfarben_m6 = 5; FARBEGRUEN = 50; FARBEBLAU = 50; Programmzeit = 10000; break;
			case 9: Modus = 19; FARBEBLAU = 150; FARBEGRUEN = 150; Programmzeit = 8000; break;
			case 10: Modus = 26; Warte = 10; FARBEBLAU = 250; FARBEGRUEN = 250; break;
			case 11: FARBEBLAU = 0; FARBEGRUEN = 0; FARBEROT = 250;  Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200); break;
			case 12: FARBEBLAU = 200; FARBEGRUEN = 250; FARBEROT = 0;  Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200); break;
			case 13: Modus = 28; Vor = 2; Hinten = 0; nfarben_m6 = 3; Programmzeit = 5000; FARBEBLAU = 1; break;
			case 14: Modus = 27; Vor = 3; break;
			case 15: nfarben_m6 = 2; break;
			case 16: nfarben_m6 = 1; break;
			case 17: FARBEBLAU = 250; FARBEGRUEN = 0; Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200); break;
			case 18: Warte = 5; break;
			case 19: Warte = 1; Modus = 22; break;
			case 20: Modus = 23; Warte = 0; Programmzeit =10000; break;
			case 21: Modus = 25; FARBEGRUEN = 180; FARBEROT = 250; FARBEBLAU = 0;  Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200); break;
			case 22: FARBEBLAU = 250; FARBEGRUEN = 0; FARBEROT = 0; break;
			case 23: Modus = 101; Programmzeit = 500; break;
			case 24: Modus = 29; nfarben_m6 = 3; FARBEBLAU = 250; FARBEGRUEN = 250; FARBEROT = 250; Programmzeit = 5000; break;
			case 25: Neuewerte(2); nfarben_m6 = 5; break;
			case 26:Neuewerte(5); nfarben_m6 = 10; break;
			case 27:Neuewerte(10); nfarben_m6 = 20; break;
			case 28:Neuewerte(20); nfarben_m6 = 40; break;
			default: Programmmodus = 1;
				break;
			}
			Serial.println(Programmmodus);
			Serial.println(Zeit);
			Programmmodus++;
			yield();
		}
		//Errechnezwischenwerte(FARBEROT, FARBEBLAU, FARBEGRUEN, 200);
	}
	
}
void Neuewerte(int anzahl)
{
	for (int i = 0; i < anzahl; i++)
	{
		GenauePosition[nfarben_m6+i] = random(200);
		Geschwindigkeit[nfarben_m6 + i] = random(-1, 1);
		if (Geschwindigkeit[nfarben_m6 + i] == 0)
		{
			if (random(0, 2) == 1)Geschwindigkeit[nfarben_m6 + i] = 1;
			else Geschwindigkeit[nfarben_m6 + i] = -1;
		}
		arrayrot[nfarben_m6 + i] = random(250);
		arraygruen[nfarben_m6 + i] = random(250);
		arrayblau[nfarben_m6 + i] = random(250);
	}
}
void aktualisierewerte()
{
	Daten.lesewertsub();
	Wertezuordnen(Daten.Buchstabe, Daten.Errechneterwert);
}
int Lesenalogpin(int ANALOGPIN)
{
	int wert = analogRead(ANALOGPIN)-5;
   if (wert <= 0) wert =0;
   return wert;
}


void Wertezuordnen(int Buchstabe, int Wert)
{
	switch (Buchstabe)
	{
	case 83 : Synchron(); break;
	case 98: FARBEBLAU = Wert; break;
	case 99: CreativMode(Wert); break;
	case 102: Bassfilter = Wert; break;
	case 104: Hinten = Wert; break;
	case 103: FARBEGRUEN = Wert; break;
	case 109: Modus = Wert; break;
	case 110: nfarben_m6 = Wert; break;
	case 112: Randomwert = Wert; break;
	case 114: FARBEROT = Wert; break;
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
	Serial.print("A"); Serial.print(nfarben_m6);
	Serial.print("F"); Serial.print(Bassfilter);

}
void Lichter(int Rot, int Blau, int Gruen, int Wartev, int Start, int Stop)
{
	for (int i = Start; i < Stop; i++)
	{
		pixels.setPixelColor(i + Startanfang, pixels.Color(Rot, Blau, Gruen));
		delay(Wartev);
		pixels.show();
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
void Arraylicht()
{
	
	for (int i = 0; i < NUMPIXELS - 1; i++)
	{
		
			pixels.setPixelColor(i + Startanfang, einzelnrot[i], einzelnblau[i], einzelngruen[i]);
		
	}
	if(Modus != 40 && Modus != 33)pixels.show();
	
	
}
void Lichthinzufuegen(int Position, int Rot, int Blau, int Gruen)
{
  einzelnrot[Position] = Rot;
  einzelnblau[Position] = Blau;
  einzelngruen[Position] = Gruen;
}
void LedKetteHinten(bool Schleife)
{
  zwischenrot = einzelnrot[0];
  zwischenblau = einzelnblau[0];
  zwischengruen = einzelngruen[0];
  for (int i = 1; i < NUMPIXELS; i++)
  {
    einzelnrot[i - 1] = einzelnrot[i];
    einzelnblau[i - 1] = einzelnblau[i];
    einzelngruen[i - 1] = einzelngruen[i];
  }
  if(Schleife)
  {
  einzelnrot[NUMPIXELS - 1] = zwischenrot;
  einzelnblau[NUMPIXELS - 1] = zwischenblau;
  einzelngruen[NUMPIXELS - 1] = zwischengruen;
  }
}
void LedKetteVor(bool Schleife)
{
  if(Schleife)
  {
  zwischenrot = einzelnrot[NUMPIXELS-1];
  zwischenblau = einzelnblau[NUMPIXELS - 1];
  zwischengruen = einzelngruen[NUMPIXELS - 1];
  }
  for (int i = NUMPIXELS - 1; i != 0 ; i--)
  {
    einzelnrot[i] = einzelnrot[i - 1];
    einzelnblau[i] = einzelnblau[i - 1];
    einzelngruen[i] = einzelngruen[i - 1];
  }
  if(Schleife)
  {
  einzelnrot[0] = zwischenrot;
  einzelnblau[0] = zwischenblau;
  einzelngruen[0] = zwischengruen;
  }
}
void LedKetteMinus()
{
	for (int i =0; i != NUMPIXELS; i++)
	{
		einzelnrot[i] = einzelnrot[i] - 1;
		einzelnblau[i] = einzelnblau[i] - 1;
		einzelngruen[i] = einzelngruen[i] - 1;
		if (einzelnrot[i] < 0) einzelnrot[i] = 0;
		if (einzelngruen[i] < 0) einzelngruen[i] = 0;
		if (einzelnblau[i] < 0) einzelnblau[i] = 0;
	}
}
void ArrayFarbeMinus(int i)
{
	if (arraygruen[i] != 0) arraygruen[i]--;
	if (arrayrot[i] != 0) arrayrot[i]--;
	if (arrayblau[i] != 0) arrayblau[i]--;
}
void LedKetteplusverkleinern(bool Schleife)
{
	if (Schleife)
	{
		zwischenrot = einzelnrot[NUMPIXELS - 1];
		zwischenblau = einzelnblau[NUMPIXELS - 1];
		zwischengruen = einzelngruen[NUMPIXELS - 1];
	}
	for (int i = NUMPIXELS - 1; i != 0; i--)
	{
		einzelnrot[i] = einzelnrot[i - 1] - 1;
		einzelnblau[i] = einzelnblau[i - 1] - 1;
		einzelngruen[i] = einzelngruen[i - 1] - 1;
		if (einzelnrot[i] < 0) einzelnrot[i] = 0;
		if (einzelngruen[i] < 0) einzelngruen[i] = 0;
		if (einzelnblau[i] < 0) einzelnblau[i] = 0;
	}
	if (Schleife)
	{
		einzelnrot[0] = zwischenrot;
		einzelnblau[0] = zwischenblau;
		einzelngruen[0] = zwischengruen;
	}

}
void Streifen(bool Schleife)
{
	for (int i = 0; i < Vor; i++)
	{
		LedKetteVor(Schleife);
		Arraylicht();
	
		delay(Warte);

	}
	for (int i = 0; i < Hinten; i++)
	{

		LedKetteHinten(Schleife);
		Arraylicht();
		delay(Warte);
	}
}
int ErrechneFarbpositionen(int Position,int Geschwindigkeit)
{
	Position = Position + Geschwindigkeit;
	if (Position > NUMPIXELS) Position = NUMPIXELS;
	if (Position < 0) Position = 0;
	return Position;
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
void Errechnezwischenwerte(double Rot,double Blau,double Gruen,double Schritte)
{  
   zwischenrot = Rot/Schritte;
   zwischenblau = Blau/Schritte;
   zwischengruen = Gruen/Schritte;
}
void CreativMode(int Wert)
{
switch (Wert)
	{
	case 0: 
		delay(200);
		Serial.print("p");
		WarteaufSignal(); Daten.LeseZahlen(); Position[0] = Daten.Errechnewert(); 
		Serial.print("r");
		WarteaufSignal(); Daten.LeseZahlen(); zwischenrot = Daten.Errechnewert();
		Serial.print("b");
		WarteaufSignal(); Daten.LeseZahlen(); zwischenblau = Daten.Errechnewert();
		Serial.print("g");
		WarteaufSignal(); Daten.LeseZahlen(); zwischengruen = Daten.Errechnewert();
		Lichthinzufuegen(Position[0], zwischenrot, zwischenblau, zwischengruen);
		break;
	case 1: if (Debugmodus) Debugmodus = false; else Debugmodus = true; break;
	case 2: if (Startecounter) Startecounter = false; else Startecounter = true;	Zeit = millis(); break;
	default:
		break;
	}
}
void WarteaufSignal()
{
	while (Serial.available() == 0)
	{
		delay(1);
	}
	delay(4);
}
