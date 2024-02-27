#include <8051.h>

#define T0_DAT 65535-921 // przerwanie T0 co 1ms
#define TL_0 T0_DAT%256 // tak bêdzie ³atwiej
#define TH_0 T0_DAT/256 // prze³adowaæ timer
#define TRUE 1
#define FALSE 0
#define T100 1000 // pó³okres LED
#define KEY P3_5 // wciœniêcie klawisza na klawiaturze

//szablony cyfr (od 0 do 9) dla wyœwietlacza LED przechowywane w pamiêci programu (__code)
__code unsigned char WZOR[10] = {0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110, 0b1101101, 0b1111101, 0b0000111,0b1111111, 0b1101111};

//bit 6 portu 1 w³¹cza/wy³¹cza wyœwietlacz LED
__xdata unsigned char *led_wyb = (__xdata unsigned char *) 0xFF30;     //bufor wybieraj¹cy bitowo aktywny wyœwietlacz
__xdata unsigned char *led_led = (__xdata unsigned char *) 0xFF38;    //bufor wybieraj¹cy aktywne segmenty wyœwietlacza

//Deklaracje zmiennych bitowych
__bit __at(0x96)SEG_OFF;
__bit T0_flag = 0;
__bit Edit_flag = 1;
__bit KBD_flag = 0;

//Deklaracje zmiennych
unsigned char keySelected = 0;
unsigned char led_p = 1, led_b = 1;                    //indeks aktywnego wyœwietlacza / aktywny wyœwietlacz (bitowo)
unsigned char hh = 23, mm = 59, ss = 55;
unsigned int R2 = 240;
unsigned int R3 = 4;
unsigned char n = 0;
unsigned char display = 1;

//Deklaracje Funkcji
void t0_int(void)__interrupt(TRUE);
void t0_serv(void);

void inkrementacja(void);
void inkrementacja_klawiszem(void);

void wyswietlanie(void);

void inc_ss(void);
void inc_mm(void);
void inc_hh(void);

void dec_ss(void);
void dec_mm(void);
void dec_hh(void);

void obsluga_klawiatury(void);



//Funkcja g³ówna
void main() 
{
    TH0 = 226;
    TMOD = 0b01110000;
    IE = 0b10000010;
    TR0 = 0b00000001;

    while (TRUE) 
    {
        if (T0_flag == 1) 
	{
            T0_flag = 0;
            wyswietlanie();

            if (Edit_flag == 1) 
	    {
                t0_serv();
            }
            if (KEY == TRUE) 
	    {
                keySelected = led_b;
                obsluga_klawiatury();
            } 
	    else if (KEY == FALSE && KBD_flag == 1 && keySelected == led_b) 
	    {
                KBD_flag = 0;
            }
        }
    }
}



void obsluga_klawiatury()	//Obs³ugiwanie kolejnych przycisków klawiatury
{
    if (KBD_flag == 0) 
    {
        KBD_flag = 1;

        if (keySelected == 2) // //Przycisk enter w³¹cza tryb edycji
	{
	    n++;
            if (n == 1) 
	    {
                Edit_flag = 0; // zatrzymuje zegar
                display = 1;
                P1_7 = FALSE;
            } 
	    else if (n == 2) 
	    {
                Edit_flag = 1;  // wznawia zegar
                n = 0;
                P1_7 = TRUE;
            }
        } 
	else if (keySelected == 4 && Edit_flag == 0)    //Przycisk escape zeruje zegarek
	{
            ss = 0;
            mm = 0;
            hh = 0;

        }
	else if (keySelected == 8 && Edit_flag == 0) 	//Przycisk w prawo przechodzi po wyœwietlaczach w prawo
	{
            display--;
            if (display < 1) 
	    {
                display = 3;
            }
        } 
	else if (keySelected == 16 && Edit_flag == 0) 	//Przycisk w gore inkrementuje wartoœci sekund/minut/godzin
	{
            if (display == 1) 
	    {
                inc_ss();
            }
	    else if (display == 2) 
	    {
                inc_mm();
            }
	    else if (display == 3) 
	    {
                inc_hh();
            }

        } 
	else if (keySelected == 32 && Edit_flag == 0)	//Przycisk w dol dekrementuje wartoœci sekund/minut/godzin
	{
            if (display == 1) 
	    {
                dec_ss();
            }
	    else if (display == 2) 
	    {
                dec_mm();
            } 
	    else if (display == 3) 
	    {
                dec_hh();
            }
        } 
	else if (keySelected == 1 && Edit_flag == 0)  	//Przycisk w lewo przechodzi po wyœwietlaczach w lewo
	{
            display++;
            if (display > 3) 
	    {
                display = 1;
            }
        }
    }
}

void t0_int()__interrupt(TRUE)
{
	TH0 = 226;
	T0_flag = 1;
}

void t0_serv()
{
    R2--;
    if (R2 == 0) 
    {
        R3--;
        if (R3 == 0) 
	{
            R3 = 4;
            inkrementacja();
        }
        R2 = 240;
    }
}

void inkrementacja() 
{
    ss++;
    if (ss == 60) 
    {
        ss = 0;
        mm++;
    }
    if (mm == 60) 
    {
        mm = 0;
        hh++;
    }
    if (hh == 24) 
    {
        hh = 0;
    }
}

void inc_ss()
{
    ss++;
    if (ss == 60) 
    {
        ss = 0;
        mm++;
    }
    if (mm == 60) 
    {
        mm = 0;
        hh++;
    }
    if (hh == 24) 
    {
        hh = 0;
    }
}

void inc_mm() 
{
    mm++;
    if (mm == 60) 
    {
        mm = 0;
        hh++;
    }
    if (hh == 24) 
    {
        hh = 0;
    }
}

void inc_hh() 
{
    hh++;
    if (hh == 24) 
    {
        hh = 0;
    }
}

void dec_ss() 
{
    int minuty = mm;
    int godziny = hh;
    if (ss > 0) 
    {
        ss--;
    } 
    else 
    {
        ss = 59;
        mm--;
        minuty--;
    	if(minuty<0)
    	{
	   mm = 59;
	   hh--;
	   godziny--;
	   if(godziny<0)
	   {
	    hh = 23;
	   }
 	}
    }
}


void dec_mm() 
{
    int godziny = hh;
    if (mm > 0) 
    {
        mm--;
    } 
    else 
    {
        mm = 59;
        hh--;
        godziny--;
	if(godziny<0)
	{
 	 hh = 23;
	}
    }
}

void dec_hh() 
{
    if (hh > 0) 
    {
        hh--;
    } 
    else 
    {
        hh = 23;
    }
}


void wyswietlanie()
{
    if (led_p == 1) 
    {
        SEG_OFF = TRUE; //wy³¹cza wyœw. LED
        *led_wyb = led_b; //wybiera wyœwietlacz
        if (Edit_flag == 0 && display == 1) 
	{
            *led_led = WZOR[ss % 10] | 0b10000000; //wybiera segmenty
        } 
	else 
	{
            *led_led = WZOR[ss % 10];
        }
        led_b += led_b;
        led_p++;
        SEG_OFF = FALSE;
    } 
    else if (led_p == 2) 
    {
        SEG_OFF = TRUE; //wy³¹cza wyœw. LED
        *led_wyb = led_b; //wybiera wyœwietlacz
        if (Edit_flag == 0 && display == 1) 
	{
            *led_led = WZOR[ss / 10] | 0b10000000; //wybiera segmenty
        } 
	else 
	{
            *led_led = WZOR[ss / 10];
        }
        led_b += led_b;
        led_p++;
        SEG_OFF = FALSE;
    } 
    else if (led_p == 3) 
    {
        SEG_OFF = TRUE; //wy³¹cza wyœw. LED
        *led_wyb = led_b; //wybiera wyœwietlacz
        if (Edit_flag == 0 && display == 2) 
	{
            *led_led = WZOR[mm % 10] | 0b10000000; //wybiera segmenty
        } 
	else
	{
            *led_led = WZOR[mm % 10];
        }
        led_b += led_b;
        led_p++;
        SEG_OFF = FALSE;
    } 
    else if (led_p == 4) 
    {
        SEG_OFF = TRUE; //wy³¹cza wyœw. LED
        *led_wyb = led_b; //wybiera wyœwietlacz
        if (Edit_flag == 0 && display == 2) 
	{
            *led_led = WZOR[mm / 10] | 0b10000000; //wybiera segmenty
        } 
	else 
	{
            *led_led = WZOR[mm / 10];
        }
        led_b += led_b;
        led_p++;
        SEG_OFF = FALSE;
    } 
    else if (led_p == 5) 
    {
        SEG_OFF = TRUE; //wy³¹cza wyœw. LED
        *led_wyb = led_b; //wybiera wyœwietlacz
        if (Edit_flag == 0 && display == 3) 
	{
            *led_led = WZOR[hh % 10] | 0b10000000; //wybiera segmenty
        } 
	else 
	{
            *led_led = WZOR[hh % 10];
        }
        led_b += led_b;
        led_p++;
        SEG_OFF = FALSE;
    } 
    else if (led_p == 6) 
    {
        SEG_OFF = TRUE; //wy³¹cza wyœw. LED
        *led_wyb = led_b; //wybiera wyœwietlacz
        if (Edit_flag == 0 && display == 3) 
	{
            *led_led = WZOR[hh / 10] | 0b10000000; //wybiera segmenty
        } 
	else 
	{
            *led_led = WZOR[hh / 10];
        }
        led_b = 1;
        led_p = 1;
        SEG_OFF = FALSE;
    }
}