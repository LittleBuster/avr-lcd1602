/* LCD1602 (HD44780) AVR library
 *
 * Copyright (C) 2016 Sergey Denisov.
 * Rewritten by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 */

#include "lcd1602.h"
#include "i2c.h"
#include <util/delay.h>


static struct {
	uint8_t led_pin;
} lcd;


void putnibble(char t)
{
	t <<= 4;
	i2c_send_packet(lcd.led_pin |= 0x04, SLA_W);
	_delay_us(50);
	i2c_send_packet(lcd.led_pin | t, SLA_W);
	i2c_send_packet(lcd.led_pin &= ~0x04, SLA_W);
	_delay_us(50);
}

void lcd1602_send_byte(char c, char rs)
{
	char highc = 0;
	highc = c >> 4;
	if (rs == LCD_COMMAND)
		i2c_send_packet(lcd.led_pin &=~ 0x01, SLA_W);
	else
		i2c_send_packet(lcd.led_pin |= 0x01, SLA_W);
	putnibble(highc);
	putnibble(c);
}

void lcd1602_send_char(char c)
{
	char highc = 0;

	highc = c >> 4;
	i2c_send_packet (lcd.led_pin |= 0x01, SLA_W);
	putnibble(highc);
	putnibble(c);
}

void lcd1602_init()
{
	lcd.led_pin = 0;
	i2c_init();
	_delay_ms(15);
	putnibble(0b00000011);
	_delay_ms(4);
	putnibble(0b00000011);
	_delay_us(100);
	putnibble(0b00000011);
	_delay_ms(1);
	putnibble(0b00000010);
	_delay_ms(1);
	lcd1602_send_byte(0x28, LCD_COMMAND);
	_delay_ms(1);
	lcd1602_send_byte(0x0C, LCD_COMMAND);
	_delay_ms(1);
	lcd1602_send_byte(0x06, LCD_COMMAND);
	_delay_ms(1);
	i2c_send_packet(lcd.led_pin |= 0x08, SLA_W);
	i2c_send_packet(lcd.led_pin &=~ 0x02, SLA_W);
}

void lcd1602_clear()
{
	lcd1602_send_byte(0x01, LCD_COMMAND);
	_delay_us(1500);
}

void lcd1602_goto_xy(char col, char row)
{
	char adr;

	adr = 0x40*row + col;
	adr |= 0x80;
	lcd1602_send_byte(adr, LCD_COMMAND);
}

void lcd1602_send_string(const char *str)
{
	uint8_t i;

	for(i = 0; str[i] != '\0'; i++)
		lcd1602_send_char(str[i]);
}
