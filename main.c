#include <avr/io.h>
#include "I2C_slave.h"
#include <util/twi.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include "DS18B20/ds18x20.h"
#include "OW/dallas_one_wire.h"
#include "register.h"

#define LED1 PB6
#define LED2 PB7
#define LED3 PD5
#define LED4 PD6
#define LED5 PD7

#define LED1_ON (PORTB |= (1<< LED1))
#define LED1_OFF ( PORTB &= ~(1<<LED1))
#define LED2_ON (PORTB |= (1<< LED2))
#define LED2_OFF ( PORTB &= ~(1<<LED2))
#define LED3_ON (PORTD |= (1<< LED3))
#define LED3_OFF ( PORTD &= ~(1<<LED3))
#define LED4_ON (PORTD |= (1<< LED4))
#define LED4_OFF ( PORTD &= ~(1<<LED4))

#define LED5_ON (PORTD |= (1<< LED5))
#define LED5_OFF ( PORTD &= ~(1<<LED5))

uint8_t timer_counter = 0;

volatile uint8_t zero = 0;

TTEMP TEMP;

uint8_t led_data[7] = { 0, 0, 0, 0, 0, 0, 0 };
uint16_t counter = 0;
uint8_t temp_flag = 0;
uint8_t temp_inter_counter = 0;
uint8_t cel, fract;

int main(void) {

	uint8_t i = 0;

	// ustawienie rejestrow jako wyjscia
	DDRB |= (1 << LED1) | (1 << LED2);
	DDRD |= (1 << LED3) | (1 << LED4) | (1 << LED5);

	// inicjalizacja i2c
	I2C_init(0x44);
	DS18X20_int();

	DS18X20_get_temp(&TEMP);
	txbuffer[5] = TEMP.cel;
	txbuffer[6] = TEMP.fract;

	// ustawienie timera na 2500Hz

	TIMSK |= (1 << OCIE1A); // za³aczenie przerwañ dla timera
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10); // ustawienie CTC i preskalera na 64
	OCR1AL = 255;

	sei();

	while (1) {

		//if (TCNT1L < timer_counter)
		//	zero = 1;

		if (zero == 1) {
			if (txbuffer[0] != 0)
				LED1_ON;
			if (txbuffer[1] != 0)
				LED2_ON;
			if (txbuffer[2] != 0)
				LED3_ON;
			if (txbuffer[3] != 0)
				LED4_ON;
			if (txbuffer[4] != 0)
				LED5_ON;
			zero = 0;

			//przepisanie danych z rxbuffor do txbuffer
			for (i = 0; i < 5; i++) {
				txbuffer[i] = rxbuffer[i];
			}
			txbuffer[10] = rxbuffer[10];

			counter++;

			if (rxbuffer[10] == 1) {
				temp_flag = 1; // wlaczenie pomiaru
				temp_inter_counter = 0;
				rxbuffer[10] = 0;
				txbuffer[10] = 0;
			}
		} else {
			//pobranie wartosci licznika timera 1
			timer_counter = TCNT1L;
			if (timer_counter > txbuffer[0])
				LED1_OFF;
			if (timer_counter > txbuffer[1])
				LED2_OFF;
			if (timer_counter > txbuffer[2])
				LED3_OFF;
			if (timer_counter > txbuffer[3])
				LED4_OFF;
			if (timer_counter > txbuffer[4])
				LED5_OFF;
		}

		if (temp_flag == 1) {

			if (temp_inter_counter == 6) {
				txbuffer[5] = TEMP.cel;
				txbuffer[6] = TEMP.fract;
				//counter = 0; // zakonczenie pomiaru, ustawienie oczekiwania na ponowny pomiar
				// zakonczenie pomiaru
				temp_flag = 0;
			}

			//if (temp_inter_counter == 6) {
			//	dallas_reset_delay();
			//}

			if (temp_inter_counter == 5) {
				DSTempToStr(&TEMP, cel, fract);
				temp_inter_counter++;
			}

			if (temp_inter_counter == 4) {
				dallas_write_byte(DS18X20_SKIP_ROM);
				dallas_write_byte(DS18X20_READ);
				cel = dallas_read_byte();
				fract = dallas_read_byte();
				temp_inter_counter++;
			}

			if (temp_inter_counter == 3) {
				dallas_reset_delay();
				temp_inter_counter++;
			}

			if (temp_inter_counter == 2) { // oczekiwanie okolo 750 ms
				if (counter == 375) {
					temp_inter_counter++;
				}
			}

			if (temp_inter_counter == 1) {
				counter = 0;
				dallas_write_byte(DS18X20_SKIP_ROM);
				dallas_write_byte(DS18X20_CONVERT_T);
				temp_inter_counter++;
			}

			if (temp_inter_counter == 0) {
				dallas_reset_delay();
				//temp_ow_counter = 0;
				//dallas_write_byte(DS18X20_SKIP_ROM);
				temp_inter_counter++;
			}

			//temp_ow_counter++;
		}
	}
}

ISR(TIMER1_COMPA_vect) {
	zero = 1;
}

