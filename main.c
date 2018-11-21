#include "stm8s.h"


//bool GPIO_ReadInputPin(GPIOC, GPIO_PIN_4);

unsigned char  U8FLAG=0,U8temp=0;
unsigned char  U8T_data_H=0,U8T_data_L=0,U8RH_data_H=0,U8RH_data_L=0,U8checkdata=0;
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














int main( void )
{

  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  while (1)
    
  {
 GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
 GPIO_WriteLow(GPIOC, GPIO_PIN_3);
    	   //------------------------
	   //调用温湿度读取子程序 
	   RH();
	   //串口显示程序 

	   str[0]=U8RH_data_H;
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
