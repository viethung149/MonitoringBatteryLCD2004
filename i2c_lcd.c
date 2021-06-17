#include "i2c_lcd.h"


void lcd_Write_byte(char data)
{
    /* Send START condition */
    I2C_GenerateSTART(I2C_Channel, ENABLE);
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C_Channel, I2C_EVENT_MASTER_MODE_SELECT));
    /* Send PCF8574A address for write */
    I2C_Send7bitAddress(I2C_Channel, PCF8574A_Address, I2C_Direction_Transmitter);
	/* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C_Channel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /* Send the byte to be written */
    I2C_SendData(I2C_Channel, data);
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C_Channel, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_Channel, ENABLE);		
}
void lcd_init (void)
{
	// init LCD
	lcd_Control_Write(0x33);		 
	Delay_msST(10);
	lcd_Control_Write(0x32);
	Delay_msST(50);
	/* Set 4-bits interface */
	lcd_Control_Write(0x28);
	Delay_msST(50);	
	/* clear LCD */
	lcd_Control_Write(0x01);
	Delay_msST(50);
	/* wait 60ms */
	
	/* set entry mode */
	lcd_Control_Write(0x06);	Delay_msST(50);;
	/* set display to on */	
	lcd_Control_Write(0x0C);	Delay_msST(50);;
	/* move cursor to home and set data address to 0 */
	lcd_Control_Write(0x02);	Delay_msST(50);
}
void lcd_Data_Write(char data)
{
	char data_u, data_l;
	uint8_t data_t[4],i=0;
	data_u = data&0xf0;
	data_l = (data<<4)&0xf0;
	data_t[0] = data_u|0x0d;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0d;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	for(i = 0;i<4;i++)
	{
	     lcd_Write_byte(data_t[i]);
	}
}
void lcd_Control_Write(char data)
{
  char data_u, data_l;
	uint8_t data_t[4],i=0;
	data_u = data&0xf0;
	data_l = (data<<4)&0xf0;
	data_t[0] = data_u|0x04;  //en=1, rs=0
	data_t[1] = data_u;  //en=0, rs=0
	data_t[2] = data_l|0x04;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	for(i = 0;i<4;i++)
	{
	     lcd_Write_byte(data_t[i]);
	}

}
void lcd_send_string (char *str)
{
	while (*str) lcd_Data_Write (*str++);
}
void lcd_send_string_size (char *str,int size)
{
	int i=size-1;
	while (i>=0) 
	{
		lcd_Data_Write (*(str+i));
		i--;
	}
}
void Delete_LCD(void)
{
	lcd_Control_Write(0x01);
	Delay_msST(10);
}

void GoToXY(int x,int y)
{
	uint8_t command;
	if(y==1)
	{
		command=0x80+(x-1);
		lcd_Control_Write(command);
	}
	else if(y==2)
	{
		command=0xC0+(x-1);
		lcd_Control_Write(command);
	}
	else if(y==3)
	{
		command=0x94+(x-1);
		lcd_Control_Write(command);
	}
	else if(y==4)
	{
		command=0xD4+(x-1);
		lcd_Control_Write(command);
	}
}
void I2C_init_LCD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0; // 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;       
	I2C_InitStructure.I2C_ClockSpeed = 100000;										
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);
}

void lcd_infor_cell(float voltage[], float temperature[], int number_of_cell, int number_of_package){
	lcd_Control_Write(CLEAR_LCD);
	Delay_msST(10);
	if(number_of_cell != 0) {
		GoToXY(1,1);
		Delay_msST(10);
		lcd_Data_Write('C');
		lcd_Data_Write(number_of_cell+48);
		Delay_msST(10);
		lcd_send_string(" Voltage: ");
		print_float_value(voltage[number_of_cell-1]);
		lcd_send_string(" V");
		GoToXY(1,2);
		Delay_msST(10);
		lcd_send_string("   Temper: ");
		print_float_value(temperature[number_of_cell-1]);
		lcd_send_string(" oC");
		GoToXY(1,3);
		Delay_msST(10);
		lcd_send_string("   Status_V: N");
		GoToXY(1,4);
		Delay_msST(10);
		lcd_send_string("   Status_T: N");
	}
	else if(number_of_package != 0) {
		GoToXY(1,1);
		Delay_msST(10);
		lcd_Data_Write('P');
		lcd_Data_Write(number_of_package+48);
		Delay_msST(10);
		lcd_send_string(" Voltage: ");
		float voltage_package =0;
		for(int i  =0 ;i<4;i++){
			voltage_package += (voltage[i+(number_of_package-1)*4]);
		}
		print_float_value(voltage_package);
		lcd_send_string(" V");

		float temperature_package =0;
		for(int i  =0 ;i<4;i++){
			temperature_package += (temperature[i+(number_of_package-1)*4]/4);
		}
		GoToXY(1,2);
		Delay_msST(10);
		lcd_send_string("   Temper: ");
		print_float_value(temperature_package/4);
		lcd_send_string(" oC");
		GoToXY(1,3);
		Delay_msST(10);
		lcd_send_string("   Status_V: N");
		GoToXY(1,4);
		Delay_msST(10);
		lcd_send_string("   Status_T: N");
	}
}

void InttoString(char text[], int number){
	int length = length_number(number);
	int temp = number;
	for(int i=length-1;i>=0;i--)
	{
		text[i]=temp%10+48;
		temp=temp/10;
	}
}

int length_number (int number){
	int temp=number;
	int cnt=0;
	if(temp==0)return 1;
	else{
		while(temp>0)
		{
				temp=temp/10;
				cnt++;
		}
		return cnt;
	}
}

void print_float_value(float value){
	if(value == 0) { lcd_Data_Write('0');return;}
	float f_value_x100 = value * 100;
	int i_value_x100 = (int)f_value_x100;
	int length = length_number(i_value_x100);
	int pos_print_dot = length - 2;
	char text[SIZE];
	InttoString(text,i_value_x100);
	for(int i = 0 ;i < pos_print_dot;i++){
		lcd_Data_Write(text[i]);
	}
	lcd_Data_Write('.');
	for(int i=pos_print_dot;i<length;i++){
		lcd_Data_Write(text[i]);
	}
}
 
void print_int_value(int value){
	char text[SIZE];
	int length = length_number(value);
	InttoString(text,value);
	for(int i = 0 ;i < length;i++){
		lcd_Data_Write(text[i]);
	}
}