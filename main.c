/* 
 * File:   main.c
 * Author: naseer
 *
 * Created on November 18, 2021, 1:41 PM
 */

#define F_CPU 1843200
#define UART_BAUD 38400  

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// To keep track of time of the event. Int used by clock, string used for display
uint8_t tenth = 0;
uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours = 0;
uint8_t days = 0;

char tenth_s[] = "";
char seconds_s[] = "";
char minutes_s[] = "";
char hours_s[] = "";
char days_s[] = "";

// Implementation of interrupt routine
ISR(TIMER0_COMPA_vect, ISR_BLOCK) {
    tenth++;
    if (tenth == 10)
    {
        tenth = 0;
        seconds++; 
    }
    if (seconds == 60)
    {
        seconds = 0;
        minutes++; 
    }
    if (minutes == 60)
    {
        minutes = 0;
        hours++; 
    }
    if (hours == 24)
    {
        hours = 0;
        days++; 
    }
}

// transmit function followed as per the data sheet
void USART_Transmit(char data )
{
    /* Wait for empty transmit buffer */
    while(!( UCSRA & (1<<UDRE)));
    /* Put data into buffer, sends the data */
    UDR = data;
}

// sending time and day to the terminal application
void transmit_time(int button_pressed) 
{
    // converting from integer to string data type first 
    itoa(tenth, tenth_s, 10);
    itoa(seconds, seconds_s, 10); 
    itoa(minutes, minutes_s, 10); 
    itoa(hours, hours_s, 10); 
    itoa(days, days_s, 10); 
    
    if (days <= 9) {
        days_s[0] = '0'; 
        days_s[1] = '0';
        days_s[2] = *days_s; 
    }
    
    if (days < 100) {
        days_s[0] = '0';
        days_s[1] = *days_s;
    }
    
    // sending the items in the string characters array 
    for (int j = 0; j < sizeof(days_s); j++)
    {
        USART_Transmit(days_s[j]);
    }
    USART_Transmit(' ');
    
    for (int j = 0; j < sizeof(hours_s); j++)
    {
        USART_Transmit(hours_s[j]);
    }
    USART_Transmit(':');
    
    for (int j = 0; j < sizeof(minutes_s); j++)
    {
        USART_Transmit(minutes_s[j]);
    }
    USART_Transmit(':');
    
    for (int j = 0; j < sizeof(seconds_s); j++)
    {
        USART_Transmit(seconds_s[j]);
    }
    USART_Transmit('.');
    
    for (int j = 0; j < sizeof(tenth_s); j++)
    {
        USART_Transmit(tenth_s[j]);
    }
    USART_Transmit(' ');
    
    // identifying unique events by channels
    if (button_pressed == 6) {
        char message[] = "Channel 1 "; 
        for (int i = 0; i < sizeof(message); i++)
        {
            USART_Transmit(message[i]); 
        }
    }  
    if (button_pressed == 5) {
        char message[] = "Channel 2 "; 
        for (int i = 0; i < sizeof(message); i++)
        {
            USART_Transmit(message[i]); 
        }
    }
    if (button_pressed == 4) {
        char message[] = "Channel 3 "; 
        for (int i = 0; i < sizeof(message); i++)
        {
            USART_Transmit(message[i]); 
        }
    }
    if (button_pressed == 3) {
        char message[] = "Channel 4 "; 
        for (int i = 0; i < sizeof(message); i++)
        {
            USART_Transmit(message[i]); 
        }
    }
}

// bits to be turned on for UART
void UART_init() 
{
    UCSRB |= (1 << TXEN); 
    UCSRC |= (1 << UCSZ1) | (1 << UCSZ0);
    UBRRL = 2; 
}

void clock_setup() {      
    OCR0A = 179;           
    TCCR0A |= (1 << WGM01);
    TIMSK |= (1 << OCIE0A);
    TCCR0B |= (1 << CS02) | (1 << CS00); 
}

int button_pressed() {                            // checking what button's been pressed
    if (!(PIND & (1 << PIND6))) {
        _delay_ms(200);
        while (!(PIND & (1 << PIND6))) {}
        transmit_time(6); 
        return 6;                                   // return value corresponds to the button
    }
    else if (!(PIND & (1 << PIND5))) {
        _delay_ms(200);
        while (!(PIND & (1 << PIND5))) {}
        transmit_time(5); 
        return 5;
    }
    else if (!(PIND & (1 << PIND4))) {
        _delay_ms(200);
        while (!(PIND & (1 << PIND4))) {}
        transmit_time(4); 
        return 4;
    }
    else if (!(PIND & (1 << PIND3))) {
        _delay_ms(200);
        while (!(PIND & (1 << PIND3))) {}
        transmit_time(3); 
        return 3;
    }
    return 0; 
}

int main(int argc, char** argv) {
    
    UART_init(); 
    sei(); 
    clock_setup(); 
    
    DDRD &= ~(1 << DDD6);
    PORTD |= (1 << PORTD6); 
    
    DDRD &= ~(1 << DDD5);
    PORTD |= (1 << PORTD5);
    
    DDRD &= ~(1 << DDD4);
    PORTD |= (1 << PORTD4);
    
    DDRD &= ~(1 << DDD3);
    PORTD |= (1 << PORTD3);
    
    while (1) {
        
        button_pressed();         

    }

    return (EXIT_SUCCESS);
}

