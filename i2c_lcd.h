#ifndef __I2C_LCD_H
#define __I2C_LCD_H
#include "stm32f10x.h"
#define CLEAR_LCD 0x01
#define SIZE 10
#define PCF8574A_Address    0x4E
// #define PCF8574A_Address    0xFD
#define I2C_Channel      		I2C1
extern void Delay_msST(uint16_t nTime);
void I2C_init_LCD(void);
void lcd_Write_byte(char data);
void lcd_init (void);
void lcd_Data_Write(char data);
void lcd_Control_Write(char data);
void lcd_send_string (char *str);
void lcd_send_string_size (char *str,int size);
void Delete_LCD(void);
void GoToXY(int x,int y);
void lcd_infor_cell(float voltage[], float temperature[], int number_of_cell, int number_of_package);
int length_number (int number);
void InttoString(char text[], int number);
void print_float_value(float value);
void print_int_value(int value);
#endif
