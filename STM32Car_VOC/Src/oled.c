//////////////////////////////////////////////////////////////////////////////////	 
// 
//
//
//
//  Œƒ º˛ √˚   : main.c
//  ∞Ê ±æ ∫≈   : v2.0
//  ◊˜    ’ﬂ   : Evk123
//  …˙≥…»’∆⁄   : 2014-0101
//  ◊ÓΩ¸–ﬁ∏ƒ   : 
//  π¶ƒ‹√Ë ˆ   : 0.69¥ÁOLED Ω”ø⁄—› æ¿˝≥Ã(STM32F103ZEœµ¡–IIC)
//              Àµ√˜: 
//              ----------------------------------------------------------------
//              GND   µÁ‘¥µÿ
//              VCC   Ω”5VªÚ3.3vµÁ‘¥
//              SCL   Ω”PD6£®SCL£©
//              SDA   Ω”PD7£®SDA£©            
//              ----------------------------------------------------------------
//
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////£

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  
#include "cmsis_os.h"

//OLEDµƒœ‘¥Ê
//¥Ê∑≈∏Ò Ω»Áœ¬.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
/**********************************************
//IIC Start
**********************************************/
/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{

	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
	OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void IIC_Wait_Ack()
{

	//GPIOB->CRH &= 0XFFF0FFFF;	//…Ë÷√PB12Œ™…œ¿≠ ‰»Îƒ£ Ω
	//GPIOB->CRH |= 0x00080000;
//	OLED_SDA = 1;
//	delay_us(1);
	//OLED_SCL = 1;
	//delay_us(50000);
/*	while(1)
	{
		if(!OLED_SDA)				//≈–∂œ «∑ÒΩ” ’µΩOLED ”¶¥–≈∫≈
		{
			//GPIOB->CRH &= 0XFFF0FFFF;	//…Ë÷√PB12Œ™Õ®”√Õ∆√‚ ‰≥ˆƒ£ Ω
			//GPIOB->CRH |= 0x00030000;
			return;
		}
	}
*/
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{
			OLED_SDIN_Set();
		}
		else 
		{
			OLED_SDIN_Clr();
		}
		da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x7A);     //Slave address,SA0=0
	IIC_Wait_Ack();	
	
	Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
	
	Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
	IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x7A);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
	
	Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
	
	Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
	IIC_Stop();
}

void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		Write_IIC_Data(dat);
	}
	else 
	{
		Write_IIC_Command(dat);		
	}
}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);	//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(fill_Data,1);
		}
	}
}


/***********************Delay****************************************/
//void Delay_50ms(unsigned int Del_50ms)
//{
//	unsigned int m;
//	for(;Del_50ms>0;Del_50ms--)
//		for(m=6245;m>0;m--);
//}

//void Delay_1ms(unsigned int Del_1ms)
//{
//	unsigned char j;
//	while(Del_1ms--)
//	{	
//		for(j=0;j<123;j++);
//	}
//}

//◊¯±Í…Ë÷√

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
//ø™∆ÙOLEDœ‘ æ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC√¸¡Ó
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//πÿ±’OLEDœ‘ æ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC√¸¡Ó
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//«Â∆¡∫Ø ˝,«ÂÕÍ∆¡,’˚∏ˆ∆¡ƒª «∫⁄…´µƒ!∫Õ√ªµ„¡¡“ª—˘!!!	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //…Ë÷√“≥µÿ÷∑£®0~7£©
		OLED_WR_Byte (0x00,OLED_CMD);      //…Ë÷√œ‘ æŒª÷√°™¡–µÕµÿ÷∑
		OLED_WR_Byte (0x10,OLED_CMD);      //…Ë÷√œ‘ æŒª÷√°™¡–∏ﬂµÿ÷∑   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //∏¸–¬œ‘ æ
}
void OLED_On(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //…Ë÷√“≥µÿ÷∑£®0~7£©
		OLED_WR_Byte (0x00,OLED_CMD);      //…Ë÷√œ‘ æŒª÷√°™¡–µÕµÿ÷∑
		OLED_WR_Byte (0x10,OLED_CMD);      //…Ë÷√œ‘ æŒª÷√°™¡–∏ﬂµÿ÷∑   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //∏¸–¬œ‘ æ
}

void OLED_Clear_Area(unsigned char sx, unsigned char sy, unsigned char ex, unsigned char ey)  
{
	unsigned char x,y;
	
	if(ex-sx<0||ey-sy<0) return;	
	
	x = sx;
	y = sy;
	
	for(y=sy; y<=ey; ++y)
	{	
	  OLED_Set_Pos(x,y);		
		for(x=sx; x<ex; ++x)
		{ 
			OLED_WR_Byte(0,OLED_DATA);
		}	
	}
}

void OLED_set_area(unsigned char sx, unsigned char sy, unsigned char ex, unsigned char ey)  
{
	unsigned char x,y;
	
	if(ex-sx<0||ey-sy<0) return;	
	
	x = sx;
	y = sy;
	
	for(y=sy; y<=ey; ++y)
	{	
	  OLED_Set_Pos(x,y);		
		for(x=sx; x<ex; ++x)
		{ 
			OLED_WR_Byte(0xff,OLED_DATA);
		}	
	}
}

//‘⁄÷∏∂®Œª÷√œ‘ æ“ª∏ˆ◊÷∑˚,∞¸¿®≤ø∑÷◊÷∑˚
//x:0~127
//y:0~63
//mode:0,∑¥∞◊œ‘ æ;1,’˝≥£œ‘ æ				 
//size:—°‘Ò◊÷ÃÂ 16/12 
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size)
{      	
	unsigned char c=0,i=0;	
	
	c=chr-' ';//µ√µΩ∆´“∆∫Ûµƒ÷µ	 //ø’∏Ò0x20-32   ø’∏Ò“‘∫Û «œ‘ æ◊÷∑˚	

	if(x>Max_Column-1)  //≥¨π˝◊Ó¥Û¡–
	{
		x=0;
		y=y+2;  //y÷·±„“À2∏ˆµ„’Û
	}
	if(Char_Size ==16)  //8*16
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
		
		OLED_Set_Pos(x,y+1);		
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
	else 
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WR_Byte(F6x8[c][i],OLED_DATA);		
	}
}
//m^n∫Ø ˝
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//œ‘ æ2∏ˆ ˝◊÷
//x,y :∆µ„◊¯±Í	 
//len : ˝◊÷µƒŒª ˝
//size:◊÷ÃÂ¥Û–°
//mode:ƒ£ Ω	0,ÃÓ≥‰ƒ£ Ω;1,µ˛º”ƒ£ Ω
//num: ˝÷µ(0~4294967295);	 		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2)
{         	
	unsigned char t,temp;
	unsigned char enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//œ‘ æ“ª∏ˆ◊÷∑˚∫≈¥Æ
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr,unsigned char Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120)
		{
			x=0;
			y+=2;
		}
		j++;
	}
}
//œ‘ æ∫∫◊÷
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)  //16*16
{      			    
	unsigned char t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}
/***********π¶ƒ‹√Ë ˆ£∫œ‘ æœ‘ æBMPÕº∆¨128°¡64∆ ºµ„◊¯±Í(x,y),xµƒ∑∂Œß0°´127£¨yŒ™“≥µƒ∑∂Œß0°´7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
//  if(y1%8==0) y=y1/8;      
//  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 

void OLED_DrawPowerBmp(unsigned char x0, unsigned char y0, unsigned int count)
{
	unsigned char i,j;
	unsigned int index = 0;
	
	if(x0>127||y0>7||count>=10) return;
	
	for(i=0; i<2; ++i)
	{
		OLED_Set_Pos(x0,i);
		for(j=0; j<32; ++j)
		{
			OLED_WR_Byte(0,OLED_DATA);	
		}
	}
	
	for(i=0; i<2; ++i)
	{
		OLED_Set_Pos(x0,i);
		for(j=0; j<32; ++j)
		{
			OLED_WR_Byte(powerValueBmp[count][index++],OLED_DATA);	
		}
	}
	
}

void OLED_DrawRadioBmp(unsigned char x0, unsigned char y0, unsigned char count)
{
	unsigned char i,j;
	unsigned int index = 0;
	
	if(x0>127||y0>7||count>=2) return;
	
	for(i=y0; i<2+y0; ++i)
	{
		OLED_Set_Pos(x0,i);
		for(j=0; j<16; ++j)
		{
			OLED_WR_Byte(0,OLED_DATA);	
		}
	}
	
	for(i=y0; i<2+y0; ++i)
	{
		OLED_Set_Pos(x0,i);
		for(j=0; j<16; ++j)
		{
			OLED_WR_Byte(radioBmp[count][index++],OLED_DATA);	
		}
	}
	
}

//≥ı ºªØSSD1306					    
void OLED_Init(void)
{ 	 
// 	GPIO_InitTypeDef  GPIO_InitStructure; 	

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 // πƒ‹A∂Àø⁄ ±÷”
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	 
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //Õ∆ÕÏ ‰≥ˆ
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ÀŸ∂»50MHz
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //≥ı ºªØGPIOD3,6
// 	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);	

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

	//delay_ms(100);
	osDelay(100);
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address 
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	
  OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xCF,OLED_CMD);//--128  
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
//	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
//	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xA4,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	
	OLED_Clear();
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	osDelay(100);
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	osDelay(100);
}  





























