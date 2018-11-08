/*
 * Cocktailmixer.cpp
 *
 * Created: 23.05.2018 15:12:54
 * Author : Bastian Reul
 ToDo:
	1: Externe Bestellung integrieren
	2: Bildschirmausgaben korrigieren
	3: Wippe includieren
	4: Ausgabenmengen verifizieren & anpassen
	5: Testen
	6: Fader skalieren (ein zu kleiner Ausschlag auf dem Fader bedeutet schon 100mll)
	7: Fader Werte Filtern / Mittelwert bilden
	
	x: LED Kranz beleuchtung
	
 */ 

#ifndef F_CPU
#define F_CPU 32000000UL				// CPU Takt
#endif
#define Dauer_Motor_Test	1000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Init_XMEGA.h"
#include "Bildschirmausgaben.h"
#include "Pin.h"
#include "PwmPin.h"
#include "Schieberegister.h"
#include "IC_74HC595.h"
#include "Fader_Motor.h"
#include "AnalogDigitalConverter.h"
#include "Pumpenklasse.h"
#include "Pumpensteuerung.h"
#include "Faderbewegung.h"
#include "LED.h"
#include "StringOperationen.h"

#include "BringMe2IdleState.h"
#include "Potentiometer.h"
#include <stdint-gcc.h>

#include "UART.h"
#include "uart_string_handling.h"


#define UART_MAXSTRLEN 60


//Globale Pindeklarationen
	//	Taster init (PA6 & PA7 als Input)
	Pin Stoptaster('A', 6, true);
	Pin Starttaster('A', 7, true);
	//Schieberegister Pins (Alles Outputs)
	Pin Shiftregister_Data_Input('D', 3, false);
	Pin Shiftregister_Clock_Input('D', 4, false);
	Pin Shiftregister_Master_Reset('D', 5, false);
	Pin Shiftregister_Storage_Register_Clock_Input('D', 7, false);
	Pin Shiftregister_Output_Enable('D', 6, false);

	PwmPin Motor_Fader_Enable_Array[6] = {PwmPin(0.0, 2.0, 'C', 0), PwmPin(0.0, 2.0, 'C', 1), PwmPin(0.0, 2.0, 'C', 2), PwmPin(0.0, 2.0, 'C', 3), PwmPin(0.0, 2.0, 'D', 0), PwmPin(0.0, 2.0, 'D', 1)};
	
	/*AnalogDigitalConverter FaderADC_1('A', 0);
	AnalogDigitalConverter FaderADC_2('A', 1);
	AnalogDigitalConverter FaderADC_3('A', 2);
	AnalogDigitalConverter FaderADC_4('A', 3);
	AnalogDigitalConverter FaderADC_5('A', 4);
	AnalogDigitalConverter FaderADC_6('A', 5);*/
	
	
//Globale Objektdeklaration
	Schieberegister Schubverband;
/*	IC_74HC595 Glied_1(7, 1);	//Glied 1 ist der unterste von den Schieberegistern, bei einer 7 sind die bits Rechts oben gesetzt
	IC_74HC595 Glied_2(0, 2);
	IC_74HC595 Glied_3(0, 3);
	IC_74HC595 Glied_4(0, 4);
	IC_74HC595 Glied_5(0, 5);
	IC_74HC595 Glied_6(0, 6);
	*/
	IC_74HC595 GliederVektor[6] = {IC_74HC595(0,1), IC_74HC595(0,2), IC_74HC595(0,3), IC_74HC595(0,4), IC_74HC595(0,5), IC_74HC595(0,6)};
	AnalogDigitalConverter FaderADC_Array[6] = {AnalogDigitalConverter ('A', 0), AnalogDigitalConverter ('A', 1), AnalogDigitalConverter ('A', 2), AnalogDigitalConverter ('A', 3), AnalogDigitalConverter ('A', 4), AnalogDigitalConverter ('A', 5) };
	Fader_Motor Motor_Fader_Array[6] = {Fader_Motor(&Motor_Fader_Enable_Array[0], &GliederVektor[5], 1), Fader_Motor(&Motor_Fader_Enable_Array[1], &GliederVektor[5], 2), Fader_Motor(&Motor_Fader_Enable_Array[2], &GliederVektor[4], 1), Fader_Motor(&Motor_Fader_Enable_Array[3], &GliederVektor[4], 2), Fader_Motor(&Motor_Fader_Enable_Array[4], &GliederVektor[3], 1), Fader_Motor(&Motor_Fader_Enable_Array[5], &GliederVektor[3], 2)};
	bool Drehrichtung[6] = {true,true,true,true,true,true}; 
	//uint8_t soll_Fader_Array[6] = {30, 70, 110, 150, 190, 240};
	uint8_t soll_Fader_Array[6] = {200, 200, 200, 200, 200, 200};
	//Fader_Motor Motor_Fader_1(&Motor_Fader_1_Enable, &GliederVektor[5], 1);

	//PumpenobjektArray
	Pumpenklasse Pumpenarray[6] = {Pumpenklasse(1, &GliederVektor[2]), Pumpenklasse(2, &GliederVektor[2]), Pumpenklasse(3, &GliederVektor[2]), Pumpenklasse(4, &GliederVektor[2]), Pumpenklasse(5, &GliederVektor[2]), Pumpenklasse(6, &GliederVektor[2]) }	;
	
	//Der Wert der maximal vom Getrnkemixer als GEtränk ausgegeben wird
	volatile int max_Rezeptmenge = 100;

	//volatile bool weitermachen = true;
	uint8_t counter = 0;
	
	const uint8_t Sprungmarke_Start							= 0;
	const uint8_t Sprungmarke_Initialisierung				= 1;
	const uint8_t Sprungmarke_Bring_me_to_Idle_State		= 2;
	const uint8_t Sprungmarke_IdleState						= 3;
	const uint8_t Sprungmarke_Start_Gedrueckt				= 4;
	const uint8_t Sprungmarke_Remote_Rezept					= 5;
	const uint8_t Sprungmarke_Lokal_Rezept					= 8;
	const uint8_t Sprungmarke_Fader_einlesen				= 9;
	const uint8_t Sprungmarke_Rezept_ausgeben				=10;
	const uint8_t Sprungmarke_Rezept_ausgegeben				=11;
	
	volatile uint8_t ProgrammablaufStatus = Sprungmarke_Start;
	volatile bool FlagFaderRemote = false;
	

	
	LED StartTasterLED(8, &GliederVektor[1]);
	LED StopTasterLED(7, &GliederVektor[1]);
	LED LEDKranz[6] = {LED(1, &GliederVektor[1]), LED(2, &GliederVektor[1]), LED(3, &GliederVektor[1]), LED(4, &GliederVektor[1]), LED(5, &GliederVektor[1]), LED(6, &GliederVektor[1])};

	volatile bool Kein_Offset_Abgleich_noetig = false; //Dieses Flag ist nötig, da bei einer Wiederhohlung der Rezepturaufforderung
	//(aufgrund von angeforderter menge > 100mll) ein Offset von der aktuellen Position gemacht würde. 
	volatile bool StartTasterIgnorieren = true;
	uint8_t OffsetArray[6] = {0, 0, 0, 0, 0, 0};
	volatile int Rezept_Array[6] = {0, 0, 0, 0, 0, 0};
		
	//Für UART Kommunikation
	volatile uint8_t global_uart_str_complete = 0;
	volatile char global_uart_string[UART_MAXSTRLEN + 1] = "";
	int uart_getraenke_wert[6];
	bool automodus = false;
	//Variablen für UART Kommunikation
	char *Letzter_Status;
	int *Prozentmengen;
		
		
	/*
void PWM_Messung(int Durchlaeufe)
{
	//Bei 12V Spannungsversorgung fahren die motoren bei 30% bei ca 63Hz los.
	double Geschwindigkeit = 0.0;
	for (int i=0; i<Durchlaeufe; i++)
	{
		Geschwindigkeit = Geschwindigkeit + 10.0;
		Debug_MotorTest_PMW_Status(Geschwindigkeit);
		Motor_Fader_Enable_Array[0].set_Dutycycle(Geschwindigkeit);
		Motor_Fader_Array[0].setze_Drehrichtung(false);
		aktualisiere_alle_Gliederwerte();
				Schubverband.Setze_Ist_auf_Soll();
				Schubverband.Aktualisiere_Alle_Register();
				Schubverband.Aktualisiere_ist_gleich_Soll(true);
				Motor_Fader_Array[0].Enable_Motor();
				_delay_ms(Dauer_Motor_Test);
				Motor_Fader_Array[0].Disable_Motor();
				_delay_ms(Dauer_Motor_Test);
				Motor_Fader_Array[0].setze_Drehrichtung(true);
		aktualisiere_alle_Gliederwerte();
					Schubverband.Setze_Ist_auf_Soll();
					Schubverband.Aktualisiere_Alle_Register();
					Schubverband.Aktualisiere_ist_gleich_Soll(true);
					Motor_Fader_Array[0].Enable_Motor();
					_delay_ms(Dauer_Motor_Test);
					Motor_Fader_Array[0].Disable_Motor();
					_delay_ms(Dauer_Motor_Test);
	}
}	

void Motor_Test(int Durchlaeufe)
{
	double Geschwindigkeit = 0.0;
	for (int i=0; i<Durchlaeufe; i++)
	{
		Geschwindigkeit = Geschwindigkeit + 10.0;
		Debug_MotorTest_PMW_Status(Geschwindigkeit);
		//FaderMotor Test
		Motor_Fader_Enable_Array[0].set_Dutycycle(Geschwindigkeit);
		Motor_Fader_Enable_Array[1].set_Dutycycle(Geschwindigkeit);
		Motor_Fader_Enable_Array[2].set_Dutycycle(Geschwindigkeit);
		Motor_Fader_Enable_Array[3].set_Dutycycle(Geschwindigkeit);
		Motor_Fader_Enable_Array[4].set_Dutycycle(Geschwindigkeit);
		Motor_Fader_Enable_Array[5].set_Dutycycle(Geschwindigkeit);
		Motor_Fader_Array[0].setze_Drehrichtung(false);
		Motor_Fader_Array[1].setze_Drehrichtung(false);
		Motor_Fader_Array[2].setze_Drehrichtung(false);
		Motor_Fader_Array[3].setze_Drehrichtung(false);
		Motor_Fader_Array[4].setze_Drehrichtung(false);
		Motor_Fader_Array[5].setze_Drehrichtung(false);
		aktualisiere_alle_Gliederwerte();
		Schubverband.Setze_Ist_auf_Soll();
		Schubverband.Aktualisiere_Alle_Register();
		Schubverband.Aktualisiere_ist_gleich_Soll(true);
		Motor_Fader_Array[0].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[0].Disable_Motor();
		Motor_Fader_Array[1].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[1].Disable_Motor();
		Motor_Fader_Array[2].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[2].Disable_Motor();
		Motor_Fader_Array[3].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[3].Disable_Motor();
		Motor_Fader_Array[4].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[4].Disable_Motor();
		Motor_Fader_Array[5].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[5].Disable_Motor();
		Motor_Fader_Array[0].setze_Drehrichtung(true);
		Motor_Fader_Array[1].setze_Drehrichtung(true);
		Motor_Fader_Array[2].setze_Drehrichtung(true);
		Motor_Fader_Array[3].setze_Drehrichtung(true);
		Motor_Fader_Array[4].setze_Drehrichtung(true);
		Motor_Fader_Array[5].setze_Drehrichtung(true);
		aktualisiere_alle_Gliederwerte();
		Schubverband.Setze_Ist_auf_Soll();
		Schubverband.Aktualisiere_Alle_Register();
		Schubverband.Aktualisiere_ist_gleich_Soll(true);
		Motor_Fader_Array[0].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[0].Disable_Motor();
		Motor_Fader_Array[1].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[1].Disable_Motor();
		Motor_Fader_Array[2].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[2].Disable_Motor();
		Motor_Fader_Array[3].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[3].Disable_Motor();
		Motor_Fader_Array[4].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[4].Disable_Motor();
		Motor_Fader_Array[5].Enable_Motor();
		_delay_ms(Dauer_Motor_Test);
		Motor_Fader_Array[5].Disable_Motor();

		
	}
}

*/

void Initialisierung (void)
{
//	Initialisiere den µ-Controller
	Init_XMEGA();
	_delay_ms(100);
	Display_Init();
	_delay_ms(100);
	UART_init();
	//Motor_Fader_1.Disable_Motor();
	/*Motor_Fader_2.Disable_Motor();
	Motor_Fader_3.Disable_Motor();
	Motor_Fader_4.Disable_Motor();
	Motor_Fader_5.Disable_Motor();
	Motor_Fader_6.Disable_Motor();*/

	//PORTA_DIRCLR = PIN6_bm;		//PINA 6 (Stop) als Input
	//PORTA_DIRCLR = PIN7_bm;		//PINA 7 (Start) als Input

/*	PORTC.DIR |= PIN2_bm | PIN3_bm;			// PINC 2 & 3 als out
	PORTC.OUTCLR = PIN2_bm;			 		// PIN2 Clear
	PORTC.OUTCLR = PIN3_bm;					// Pin3 Clear

// FaderRegelung Schalter init (PB1)
	PORTB.DIR %= ~ PIN1_bm;
	PORTB.PIN1CTRL = PORT_OPC_PULLDOWN_gc;
*/
//	Interrupts Einstellen
	cli();									// Interrupts deaktivieren
/*	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;		// Presacler 1024 - Timer0 - 1Khz
	TCC0.CTRLB = 0x00;						// select Modus: Normal
	TCC0.PER = 3906;						// Zähler Top-Wert für 8 Hz->  Topwert = (CPU_Takt/ Eventintervall/ Prescaler)
	TCC0.CNT = 0x00;						// Zähler zurücksetzen
	TCC0.INTCTRLA = 0b00000011;				// Interrupt Highlevel
*/	PORTA.INT0MASK |= 0b10000000;					// in der Iterruptmaske 0 Pin 0 freigeben
	PORTA.INT1MASK |= 0b01000000;					// in der Iterruptmaske 1 Pin 1 freigeben
	PORTA.PIN6CTRL	= PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;	// Pin 0 PullUp aktivieren, fallende Flanke aktivieren
	PORTA.PIN7CTRL	= PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;	// Pin 1 PullUp aktivieren, fallende Flanke aktivieren
	PORTA.INTCTRL	= PORT_INT0LVL_LO_gc | PORT_INT1LVL_LO_gc;	// Interrupt LoLevel für PORTC und INT0MASK und INT0MASK freigeben

	PMIC.CTRL |= PMIC_HILVLEN_bm |PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;	// Interrupts High-,Medium- und Lowlevel freigeben
	sei();									// Interrupts aktivieren
	
}


//__attribute__((optimize(0)))
int main(void)
{
	ProgrammablaufStatus = Sprungmarke_Start;
	while (true)
	{
		//Alles was noch vor der Initialisierung passieren muss (Wird nur nach einem Neustart ausgefuehrt)
		if (ProgrammablaufStatus == Sprungmarke_Start)
		{
			ProgrammablaufStatus = Sprungmarke_Initialisierung;
		}
		
		//Initialisierung des µC, es wird NICHT die externe Peripherie (LCD, Schiebreregister, Pumpen, etc. Initialisiert) (Wird nur nach einem Neustart ausgefuehrt)
		if (ProgrammablaufStatus == Sprungmarke_Initialisierung)
		{
			Initialisierung();
			BootBildschirm();
			//verfahre_alle_Fader_auf_gleichen_wert(240);
			Schubverband.Reset_Shift_Register();
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		}

		//(Re-)Initialisierung der externen Peripherie, LCD, Schieberegister Pumpen, Leds
		if (ProgrammablaufStatus == Sprungmarke_Bring_me_to_Idle_State)
		{
			bringMe2IdleState();
			ProgrammablaufStatus = Sprungmarke_IdleState;
			Kein_Offset_Abgleich_noetig = false;
			Letzter_Status = "####IDLE++++";
			Send_UART(Letzter_Status);
			release_global_uart_string();
			FlagFaderRemote = false;
		}
		
		//Das ist die Warteschleife in der der Getränkemixer bereit ist, dass durch den druck auf Start ein Gesammtvorgang gestartet werden kann
		//Die meiste Zeit im Leerlauf wird der Getränkemixer in dieser Schleife verplempern
		if (ProgrammablaufStatus == Sprungmarke_IdleState)
		{
			//Led Kranz rotieren lassen
			
			//Falls ein UART Befehl vom Raspberry gekommen sein sollte
			if (global_uart_str_complete == 1)
			{
				//####0:1:070;0:2:010;0:3:000;0:4:010;0:5:000;0:6:010;++++  Beispielstring für eine Rezeptanforderung
				if ((global_uart_string[4] == '0') && (global_uart_string[6] == '1')) // vermutlich ein Rezeptauftrag
				{
					Prozentmengen = Rezeptur_berechnen();
					int gesammt_Prozent = 0;
					for (int i=0; i<6; i++)
					{
						gesammt_Prozent = gesammt_Prozent + Prozentmengen[i];
					}
					if (gesammt_Prozent == 100)
					{
						Send_UART("####Data_OK++++");
						Letzter_Status = "####Data_OK++++";
								// Send_UART("Prozentmenge 1: ");
								// Send_UART(int_to_char_array(Prozentmengen[0]));
						FlagFaderRemote = true;
					}
					else
					{
						Send_UART("Data Invalid");
					}
					release_global_uart_string();
				}
			} 

		}

		//Das Programm ist durch einen Druck auf Start aus dem Idle State ausgebrochen
		if (ProgrammablaufStatus == Sprungmarke_Start_Gedrueckt)
		{
				if (FlagFaderRemote)
				{//Es wurde eine Rezeptur über den Raspberry eingegeben
					//Routinen eingeben um die externe Rezeptur einzugeben
					normiere_Array_auf_Fader_Wert(Prozentmengen);
					verfahre_Fader_auf_Wert(Prozentmengen, &Motor_Fader_Enable_Array[0],&FaderADC_Array[0], &Motor_Fader_Array[0]);
					FlagFaderRemote = false;
					ProgrammablaufStatus = Sprungmarke_Fader_einlesen;
				/*	Kein_Offset_Abgleich_noetig = true;
					for (int i=0; i<6; i++)
					{
						OffsetArray[i] = 15;
					}
				*/		
				}
				else
				{//Es wird das Getraänk am Cocktailmixer selbst rezeptuiert
					//LCD Ausgabe dass die aktuelle Fuellmenge beachtet werden soll
					Max_Fuellmaenge_beachten(100);
				//	_delay_ms(2000);
					ProgrammablaufStatus = Sprungmarke_Fader_einlesen;
				}
		}
		
		//Egal ob remote oder Lokal, jetzt werden die Werte der Fader eingelesen und aufsummiert auf dem Display ausgegeben
		//Hier bleibt das Programm jetzt in der Schleife bis der Programmablaufstatus durch einen Druck auf Start oder Stop verändert wird.
		if (ProgrammablaufStatus == Sprungmarke_Fader_einlesen)
		{
			while(ProgrammablaufStatus == Sprungmarke_Fader_einlesen)
			{
				Rezeptur_einlesen();
			}
			
		}
		
		if (ProgrammablaufStatus == Sprungmarke_Rezept_ausgeben)
		{
			Debug_Rezept_wird_ausgegeben();
			//_delay_ms(2000);
			Rezeptur_ausgeben(Pumpenarray);
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		}
		
	/*//Debuging UART
	if (global_uart_str_complete == 1)
	{
		//ProgrammablaufStatus = 99;
		//Debug_UART_String_erkannt();
		
		if ((global_uart_string[4] == 'g') && (global_uart_string[8] == 's')) //vermutlich ####get_status+++
		{
			Send_UART(Letzter_Status);
		}
		//####0:1:070;0:2:010;0:3:000;0:4:010;0:5:000;0:6:010;++++
		if ((global_uart_string[4] == '0') && (global_uart_string[6] == '1')) // vermutlich ein Rezeptauftrag
		{
			 Prozentmengen = Rezeptur_berechnen();
			 int gesammt_Prozent = 0;
			 for (int i=0; i<6; i++)
			 {
				 gesammt_Prozent = gesammt_Prozent + Prozentmengen[i];
			 }
			 if (gesammt_Prozent == 100)
			 {
				 Send_UART("####Data_OK++++");
				 Letzter_Status = "####Data_OK++++";
				// Send_UART("Prozentmenge 1: ");
				// Send_UART(int_to_char_array(Prozentmengen[0]));
				FlagFaderRemote = true;
			 }

		}
		release_global_uart_string();
	/*	else
		{
			Send_UART("Data Invalid");
		}
		release_global_uart_string();
		*/
	//}
	
	
	}
	
}


ISR(PORTA_INT0_vect) //Startknopf Interrupt
{	
	cli();
	if (!(StartTasterIgnorieren)) //Zum entprellen der Starttase. Wird auf False gesetzt in BringmeToIdleState
	{
		StartTasterIgnorieren = true;
		Debug_Start_gedrueckt();
	/*Motor_Fader_1_Enable.set_PWM_status(true);
	Motor_Fader_2_Enable.set_PWM_status(true);
	Motor_Fader_3_Enable.set_PWM_status(true);
	Motor_Fader_4_Enable.set_PWM_status(true);
	Motor_Fader_5_Enable.set_PWM_status(true);
	Motor_Fader_6_Enable.set_PWM_status(true);
	Debug_PWM_aktiviert();
	
	weitermachen = true;*/
	_delay_ms(800);
	//weitermachen = true;
	//Motor_Fader_Array[0].Enable_Motor();
	//counter = counter + 1;
	switch (ProgrammablaufStatus)
	{
		case Sprungmarke_Start:
		ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Initialisierung:
		ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_IdleState:
		ProgrammablaufStatus = Sprungmarke_Start_Gedrueckt;
		break;
		
		case Sprungmarke_Start_Gedrueckt:
		ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Remote_Rezept:
		ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Lokal_Rezept:
		ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Fader_einlesen:
		ProgrammablaufStatus = Sprungmarke_Rezept_ausgeben;
		break;
		
		default: 
		ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
	}
	
	}
	
	sei();
}

ISR(PORTA_INT1_vect) //Stopknopf Interrupt
{
	cli();
	Debug_Stop_gedrueckt();
/*	Motor_Fader_1_Enable.set_PWM_status(false);
	Motor_Fader_2_Enable.set_PWM_status(false);
	Motor_Fader_3_Enable.set_PWM_status(false);
	Motor_Fader_4_Enable.set_PWM_status(false);
	Motor_Fader_5_Enable.set_PWM_status(false);
	Motor_Fader_6_Enable.set_PWM_status(false);
	_delay_ms(200);
	weitermachen = false;*/
	_delay_ms(400);
	switch (ProgrammablaufStatus)
	{
		case Sprungmarke_Start:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Initialisierung:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_IdleState:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Start_Gedrueckt:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Remote_Rezept:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Lokal_Rezept:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Fader_einlesen:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		case Sprungmarke_Rezept_ausgeben:
			ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
		
		default:
		ProgrammablaufStatus = Sprungmarke_Bring_me_to_Idle_State;
		break;
	}
	//counter = counter - 1;
	sei();
}

void SchalteKranzAus()
{
	for (int i=0; i<6; i++)
	{ 
		LEDKranz[i].schalte_aus();
	}
	aktualisiere_alle_Gliederwerte();
	Schubverband.Setze_Ist_auf_Soll();
	Schubverband.Aktualisiere_Alle_Register();
	Schubverband.Aktualisiere_ist_gleich_Soll(true);
}

void SchalteLEDAnKranzEin(uint8_t nummer)
{
	LEDKranz[nummer].schalte_ein();
	aktualisiere_alle_Gliederwerte();
	Schubverband.Setze_Ist_auf_Soll();
	Schubverband.Aktualisiere_Alle_Register();
	Schubverband.Aktualisiere_ist_gleich_Soll(true);
}