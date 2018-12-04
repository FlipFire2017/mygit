#include "stm8s.h"

#define BIT6  	0x0040
//bool GPIO_ReadInputPin(GPIOC, GPIO_PIN_4);

unsigned char  U8FLAG=0,U8temp=0;
unsigned char  U8T_data_H,U8T_data_L=0,U8RH_data_H=0,U8RH_data_L=0,U8checkdata=0;

unsigned char str[5];



void delay_us(u16 us)
{

    while (us--)
    {
        nop();nop();
        nop();nop();
        nop();nop();
        nop();nop();
    }
}
void Delay_10us(void)
{
   delay_us(10);   
}
void delay_ms(u16 ms)
{
    while (ms--)
        delay_us(1000);
}
void uart1Init(void)
{
        UART1_DeInit();
	UART1_Init((u32)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);//9600 8  1 无校验  发送接收允许
        UART1_ClearFlag(UART1_FLAG_RXNE);
        UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);//接收中断
}
void clearUartStatus(void)
{
  __disable_interrupt();

  __enable_interrupt();
}
void UART1_SendByte(uint8_t Data)
{
  while((UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET));
  UART1_SendData8(Data);
  while((UART1_GetFlagStatus(UART1_FLAG_TC)==RESET));
}
void UART1_Printf(uint8_t *String)
{
  while((*String) != '\0')
  {
    UART1_SendByte(*String);
    String++;
  }
}



u8 uartSendService(u8 *pdat,u8 len)
{
  u16 timeOut=0;
  u8 err=0;
  while(len)
  {
    UART1->SR&=~BIT6;
    UART1->DR =*pdat;
    
    pdat++;
    len--;
    timeOut = 32000;
    
    while((!(UART1->SR&BIT6)))
    {
      timeOut -- ;
	if(!timeOut)
	{
	  break;
	  }
    }
      
    if(!timeOut)
    {
      err = 1;
      break;
    	}
  }
  return err;
}
unsigned char COM(void)
      {
     
unsigned char i,U8comdata,count ;
       count=0;
          
       for(i=0;i<8;i++)	   
	    {
		
	   	    U8FLAG=2;	
	   	while((!GPIO_ReadInputPin(GPIOC, GPIO_PIN_4))&&U8FLAG++);
			Delay_10us();
		    Delay_10us();
			Delay_10us();
	  		U8temp=0;
	     if(GPIO_ReadInputPin(GPIOC, GPIO_PIN_4))U8temp=1;
		    U8FLAG=2;
            count += 1;
		 while((GPIO_ReadInputPin(GPIOC, GPIO_PIN_4))&&U8FLAG++);
	   	//超时则跳出for循环		  
	   	 if(U8FLAG==1)break;
	   	//判断数据位是0还是1	 
	   	   
		// 如果高电平高过预定0高电平值则数据位为 1 
	   	 
		   U8comdata<<=1;
	   	   U8comdata|=U8temp;        //0
	     }//rof
	  return  U8comdata	;
	}


void RH(void)
{
 unsigned char  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
	  //主机拉低18ms 
       GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
       GPIO_WriteLow(GPIOC, GPIO_PIN_4);
	   delay_ms(18);
	   GPIO_WriteHigh(GPIOC, GPIO_PIN_4);
	 //总线由上拉电阻拉高 主机延时20us
	   Delay_10us();
	   Delay_10us();
	   Delay_10us();
	   Delay_10us();
	 //主机设为输入 判断从机响应信号 
      
	   GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
	 //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	   if(!GPIO_ReadInputPin(GPIOC, GPIO_PIN_4))		 //T !	  
	   {
	   U8FLAG=2;
	 //判断从机是否发出 80us 的低电平响应信号是否结束	 
	   while((!GPIO_ReadInputPin(GPIOC, GPIO_PIN_4))&&U8FLAG++);
	   U8FLAG=2;
	 //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	   while((GPIO_ReadInputPin(GPIOC, GPIO_PIN_4))&&U8FLAG++);
	 //数据接收状态		 
	  U8RH_data_H_temp= COM();

	  U8RH_data_L_temp= COM();
	 
	  U8T_data_H_temp=COM();
	  U8T_data_L_temp=COM();
	  U8checkdata_temp=COM();
	  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
	  GPIO_WriteHigh(GPIOC, GPIO_PIN_4);
	 //数据校验 
	 
	   U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	   if(U8temp==U8checkdata_temp)
	   {
	   	  U8RH_data_H=U8RH_data_H_temp;
	   	  U8RH_data_L=U8RH_data_L_temp;
		  U8T_data_H=U8T_data_H_temp;
	   	  U8T_data_L=U8T_data_L_temp;
	   	  U8checkdata=U8checkdata_temp;
          
	   }//fi
	   }//fi

	}

unsigned char CharToHex(unsigned char bHex)
{
	if((bHex>=0)&&(bHex<=9))
	{
		bHex += 0x30;
	}
	else if((bHex>=10)&&(bHex<=15))//Capital
	{
		bHex += 0x37;
	}
	else 
	{
		bHex = 0xff;
	}
	return bHex;
}


int main( void )
{

  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  uart1Init();
  while (1)
    
  {
    unsigned char temp=0;
 GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
 GPIO_WriteLow(GPIOC, GPIO_PIN_3);
    	   //------------------------
	   //调用温湿度读取子程序 
	   RH();
	   //串口显示程序 

	   str[0]=U8RH_data_H;
           uart1Init();
           UART1_Printf((uint8_t*)"RH AND T IS");
           temp=CharToHex(0x01);
           uartSendService(0x31,1);
	   str[1]=U8RH_data_L;
           
	   str[2]=U8T_data_H;
           
	   str[3]=U8T_data_L;
           
	   str[4]=U8checkdata;
           
	    
	   //读取模块数据周期不易小于 2S 
	   delay_ms(2000);
  }
}
#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
    
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
