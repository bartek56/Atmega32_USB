/*
 * Atmega32_USB.c
 *
 * Created: 2016-12-16 12:48:17
 * Author : Bartek
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>


#include "USB/usbdrv.h"
#include "lcd44780.h"


#define USB_ONEBAJT 1
#define USB_TWOBAJT 2
#define USB_THREEBAJT 3
#define LED_DDR DDRA
#define LED_PORT PORTA
#define LED_PIN (1<<PINA0)


int k;
static uchar bytesRemaining;
int dSent=2;
static uchar text[8]="HELLO   ";
volatile uint8_t newMessageFlag = 0;
volatile uint8_t messageLength;
uchar ReceivedData1[8];
uchar ReceivedData2[8];
uchar ReceivedData3[8];



USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	
	lcd_cls();
	
	usbRequest_t *rq = (void *)data; // cast data to correct type
	
	switch(rq->bRequest)
	{
		case USB_ONEBAJT:
		{
			messageLength=1;
			return USB_NO_MSG;
		}
		case USB_TWOBAJT:
		{
			messageLength=2;
			return USB_NO_MSG;
		}
		case USB_THREEBAJT:
		{
			messageLength=3;			
			return USB_NO_MSG;
		}
	}
	return 0;
}


USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len){  // proœba odczytu z uC do PC

	
	lcd_locate(0,0);
	lcd_str("odczyt"); // READ w javie
	lcd_locate(0,1);
	lcd_str("HELLO");
	
	uchar i;
	
	for(i =0; dSent <512 && i < len; i++, dSent++)
		data[i]=text[i];
	
	// zakoñcz przesy³ gdy zosta³ wys³any ostatni bajt
	if(i && dSent == 512)
		data[i-1]=0; // NULL
	return i;
}

USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) // proœba zapisu danych na uC
{
	
	uint16_t i = 0;
	
	uchar length = len;
	
	// Write w Javie
	
	lcd_locate(0,0);
	lcd_str("zapis ");
	lcd_int(len);
	lcd_str(" bajt");
	lcd_locate(0,1);
	
	
	while(length != 0)
	{
		lcd_char(data[i]);
		i++;
		length--;
	}
	
		
	return bytesRemaining == 0;
}


int main(void)
{
	DDRA |=(1<<PINA0);
	PORTA |=(1<<PINA0);
	uchar i;
	wdt_enable(WDTO_2S);

	usbInit();
	Lcd_init();
	usbDeviceDisconnect(); 
	for(i =0; i<250; i++)
	{
		
		wdt_reset(); 
		_delay_ms(2);
	}
	usbDeviceConnect();

	sei();

    while (1) 
    {

		wdt_reset();
		usbPoll();		
    }
	return 0;
}

