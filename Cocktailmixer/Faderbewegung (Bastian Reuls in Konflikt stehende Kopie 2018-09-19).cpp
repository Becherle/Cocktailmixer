/*
 * Faderbewegung.cpp
 *
 * Created: 04.07.2018 10:49:17
 *  Author: Bastian Reul
 */ 

#include "Faderbewegung.h"
#include <stdlib.h>
#define ObereBeschraenkung 240
#define UntereBeschraenkung 15


uint8_t Regler(uint8_t abweichung, uint8_t Iteration)
{
	uint8_t Reglerwert = 0;
	if ((abweichung > 80) && (Iteration > 3)) // Falls die Abweichung in der vierten Runde immer noch �ber 80 liegen sollte, beschneide die Verst�rkung um ein Schwingen zu verhindern
	{
		abweichung = 80;
	}
	Reglerwert = abweichung + Iteration;
	return Reglerwert;
}

__attribute__((optimize(0)))
void verfahre_Fader_auf_Wert(int *zeiger_auf_soll_array, PwmPin *Motor_Fader_Enable, AnalogDigitalConverter *FaderADC, Fader_Motor *Motor_Fader)
{
	
	/* alter code Anfang 
	volatile bool Drehsinn = Motor_Fader->get_Drehsinn();
	volatile uint8_t ist = FaderADC->getValue();
	volatile uint8_t abweichung;
	volatile uint8_t IterationsAnteil = 0;
	volatile bool verfahren_ist_fertig = false;
	Motor_Fader->Enable_Motor();
	while(!verfahren_ist_fertig)
	{
		ist = FaderADC->getValue();
		//Debug_ADC_Wert_ausgeben(ist);
		IterationsAnteil = IterationsAnteil +1;
		if (ist < soll )
		{
			abweichung = soll - ist;
			Motor_Fader->setze_Drehrichtung(true);
		}
		if (ist > soll )
		{
			abweichung = ist - soll;
			Motor_Fader->setze_Drehrichtung(false);
		}
		if (Drehsinn == Motor_Fader->get_Drehsinn())
		{
			
		}
		else
		{
			aktualisiere_alle_Gliederwerte();
			Schubverband.Setze_Ist_auf_Soll();
			Schubverband.Aktualisiere_Alle_Register();
			Schubverband.Aktualisiere_ist_gleich_Soll(true);
			Drehsinn = Motor_Fader->get_Drehsinn();
		}
		
		if (ist == soll)
		{
			Motor_Fader->Disable_Motor();
			verfahren_ist_fertig = true;
			//Debug_Halt_wurde_erreicht();
		}
		else
		{
			if ((abweichung + IterationsAnteil)>100)
			{
				Motor_Fader_Enable->set_Dutycycle(double(80));
				IterationsAnteil = 0;
			}
			else
			{
				Motor_Fader_Enable->set_Dutycycle(double(Regler(abweichung, IterationsAnteil)));
			}
		}
	} Alter Code ende */
	
		volatile uint8_t ist_array[6];
		volatile uint8_t abweichung_array[6] = {0,0,0,0,0,0};
		volatile bool Fader_Halt_erreicht[6] = {false, false, false, false, false, false};
		volatile int i = 0;
		int Richtungsinitialisierung = 0;
		double iterativer_PWM_Wert = 0.0;
		while (!(Fader_Halt_erreicht[0] && Fader_Halt_erreicht[1] && Fader_Halt_erreicht[2] && Fader_Halt_erreicht[3] && Fader_Halt_erreicht[4] && Fader_Halt_erreicht[5]) )
		{
			for (; i<6; i++)
			{
				iterativer_PWM_Wert = 0.0;
				while(!Fader_Halt_erreicht[i])
				{
					ist_array[i] = FaderADC_Array[i].getValue();
					if (ist_array[i] < zeiger_auf_soll_array[i] )
					{
						//Motor_Fader_Array[i].Enable_Motor();
						abweichung_array[i] = zeiger_auf_soll_array[i] - ist_array[i];
						Motor_Fader_Array[i].setze_Drehrichtung(true);
					}
					if (ist_array[i] > zeiger_auf_soll_array[i] )
					{
						//Motor_Fader_Array[i].Enable_Motor();
						abweichung_array[i] = ist_array[i] - zeiger_auf_soll_array[i];
						Motor_Fader_Array[i].setze_Drehrichtung(false);
					}
					//Debug_Abweichung_ausgeben(abweichung);
					if (Drehrichtung[i] == Motor_Fader_Array[i].get_Drehsinn())
					{
						if (Richtungsinitialisierung < 1)
						{
							aktualisiere_alle_Gliederwerte();
							Schubverband.Setze_Ist_auf_Soll();
							Schubverband.Aktualisiere_Alle_Register();
							Schubverband.Aktualisiere_ist_gleich_Soll(true);
							Drehrichtung[i] = Motor_Fader_Array[i].get_Drehsinn();
						}
						
					}
					else
					{
						aktualisiere_alle_Gliederwerte();
						Schubverband.Setze_Ist_auf_Soll();
						Schubverband.Aktualisiere_Alle_Register();
						Schubverband.Aktualisiere_ist_gleich_Soll(true);
						Drehrichtung[i] = Motor_Fader_Array[i].get_Drehsinn();
					}
					Richtungsinitialisierung++;
					Motor_Fader_Array[i].Enable_Motor();
					//if (ist_array[i] == soll)
				if (abweichung_array[i] <= 3)
				{
					if (abweichung_array[i] <= 1)
					{
						Motor_Fader_Array[i].Disable_Motor();
						Fader_Halt_erreicht[i] = true;
					}
					else
					{
						while (Fader_Halt_erreicht[i] == false)
						{
							iterativer_PWM_Wert = iterativer_PWM_Wert + 1.0;
							Motor_Fader_Enable_Array[i].set_Dutycycle(iterativer_PWM_Wert);
							if (abs(zeiger_auf_soll_array[i] - FaderADC_Array[i].getValue()) <= 1)
							{
								Motor_Fader_Array[i].Disable_Motor();
								Fader_Halt_erreicht[i] = true;	
							}
						}

					}

					//Debug_Halt_wurde_erreicht();
					//weitermachen = false;
				}
				else
				{
					Motor_Fader_Enable_Array[i].set_Dutycycle(double(abweichung_array[i]));
				}
				}
				//Nochmal schauen ob sich ein ehemals korrekt positionierter Fader wieder verfahren hat
		/*		if (i==5)
				{
					for (volatile int j=0; j<6; j++)
					{
						//if (soll == FaderADC_Array[j].getValue())
						ist_array[j] = FaderADC_Array[j].getValue();
						if ( ( ist_array[j]- soll) < 20)
						{
							Fader_Halt_erreicht[j] = true;
						}
						else
						{	//Dann werden alle nochmal korrigiert
							Fader_Halt_erreicht[j] = false;
							i=-1;
						}
					}
				}*/
			}
		}
}
__attribute__((optimize(0)))
void verfahre_alle_Fader_auf_gleichen_wert(uint8_t soll)
{
	volatile uint8_t ist_array[6];
	volatile uint8_t abweichung_array[6] = {0,0,0,0,0,0};
	volatile bool Fader_Halt_erreicht[6] = {false, false, false, false, false, false};
	volatile int i = 0;
	int Richtungsinitialisierung = 0;
	while (!(Fader_Halt_erreicht[0] && Fader_Halt_erreicht[1] && Fader_Halt_erreicht[2] && Fader_Halt_erreicht[3] && Fader_Halt_erreicht[4] && Fader_Halt_erreicht[5]) )
	{
		for (; i<6; i++)
		{
			while(!Fader_Halt_erreicht[i])
			{
				ist_array[i] = FaderADC_Array[i].getValue();
				if (ist_array[i] < soll )
				{
					//Motor_Fader_Array[i].Enable_Motor();
					abweichung_array[i] = soll - ist_array[i];
					Motor_Fader_Array[i].setze_Drehrichtung(true);
				}
				if (ist_array[i] > soll )
				{
					//Motor_Fader_Array[i].Enable_Motor();
					abweichung_array[i] = ist_array[i] - soll;
					Motor_Fader_Array[i].setze_Drehrichtung(false);
				}
				//Debug_Abweichung_ausgeben(abweichung);
				if (Drehrichtung[i] == Motor_Fader_Array[i].get_Drehsinn())
				{
					if (Richtungsinitialisierung < 1)
					{
						aktualisiere_alle_Gliederwerte();
						Schubverband.Setze_Ist_auf_Soll();
						Schubverband.Aktualisiere_Alle_Register();
						Schubverband.Aktualisiere_ist_gleich_Soll(true);
						Drehrichtung[i] = Motor_Fader_Array[i].get_Drehsinn();
					}
			
				}
				else
				{
					aktualisiere_alle_Gliederwerte();
					Schubverband.Setze_Ist_auf_Soll();
					Schubverband.Aktualisiere_Alle_Register();
					Schubverband.Aktualisiere_ist_gleich_Soll(true);
					Drehrichtung[i] = Motor_Fader_Array[i].get_Drehsinn();
				}
				Richtungsinitialisierung++;
				Motor_Fader_Array[i].Enable_Motor();
				//if (ist_array[i] == soll)
				if (abweichung_array[i] <= 3)
				{
					if (abweichung_array[i] <= 1)
					{
						Motor_Fader_Array[i].Disable_Motor();
						Fader_Halt_erreicht[i] = true;
					} 
					else
					{
						Motor_Fader_Enable_Array[i].set_Dutycycle(50.0);
						//_delay_ms(10);
						Motor_Fader_Enable_Array[i].set_Dutycycle(1.0);
					}

					//Debug_Halt_wurde_erreicht();
					//weitermachen = false;
				}
				else
				{
					Motor_Fader_Enable_Array[i].set_Dutycycle(double(abweichung_array[i]));
				}
			}
			//Nochmal schauen ob sich ein ehemals korrekt positionierter Fader wieder verfahren hat
			if (i==5)
			{
				for (volatile int j=0; j<6; j++)
				{
					//if (soll == FaderADC_Array[j].getValue())
					ist_array[j] = FaderADC_Array[j].getValue();
					if ( ( ist_array[j]- soll) < 20)
					{
						Fader_Halt_erreicht[j] = true;
					} 
					else
					{	//Dann werden alle nochmal korrigiert
						Fader_Halt_erreicht[j] = false;
						i=-1;
					}
				}
			}
		}
	}
	
}

//Der Funktion wird eine Zahl im Bereich 0-255 �bergeben und gibt eine Zahl von 0.0 - 100.0 zur�ck
__attribute__((optimize(0)))
double normiere_Fader_Wert_auf_Prozent(uint8_t FaderWert_uint8t)
{
	double Prozentwert;
	if (FaderWert_uint8t > ObereBeschraenkung)
	{
		FaderWert_uint8t = ObereBeschraenkung;
	}
	if (FaderWert_uint8t < UntereBeschraenkung)
	{
		FaderWert_uint8t = UntereBeschraenkung;
	}
	Prozentwert = double(FaderWert_uint8t) / double(ObereBeschraenkung) * 100.0;
	return Prozentwert;
}
__attribute__((optimize(0)))
uint8_t normiere_ProzentWert_auf_Fader_Wert(double Prozentwert)
{
	double Faderwert;
	if (Prozentwert >=100.0 || Prozentwert <= 0.0)
	{
			if (Prozentwert > 100.0)
			{
				Faderwert = 240.0;
			}
			if (Prozentwert <= 0.0)
			{
				Faderwert = 15.0;
			}
	} 
	else
	{
		Faderwert = (ObereBeschraenkung - UntereBeschraenkung) / (100 / Prozentwert)+15;
	}
	return round(Faderwert);
}
/*
__attribute__((optimize(0)))
int * normiere_Array_auf_Fader_Wert(int *ProzentWerteArray)
{
	int FaderWertArray[] = {0,0,0,0,0,0};
	int *ZeigerAufFaderWertArray = FaderWertArray;
	for (int i=0; i<6; i++)
	{
		FaderWertArray[i] = normiere_ProzentWert_auf_Fader_Wert(double(ProzentWerteArray[i]));
	}
	return ZeigerAufFaderWertArray;
}*/

__attribute__((optimize(0)))
void normiere_Array_auf_Fader_Wert(int *ProzentWerteArray)
{

	for (int i=0; i<6; i++)
	{
		ProzentWerteArray[i] = normiere_ProzentWert_auf_Fader_Wert(double(ProzentWerteArray[i]));
	}

}