
#include "UsartDriver.h"

#define USART1_REC_MAX    256
#define USART1_SEND_MAX   256
#define USART2_REC_MAX    256
#define USART2_SEND_MAX   512
#define USART3_REC_MAX    1024
#define USART3_SEND_MAX   256
#define RTOS_ENABLE 1

static char usart1_rec_buff[USART1_REC_MAX];
static char usart1_send_buff[USART1_SEND_MAX];
static LoopQueue usart1_rec_queue;
static LoopQueue usart1_send_queue;

static char usart2_rec_buff[USART2_REC_MAX];
static char usart2_send_buff[USART2_SEND_MAX];
static LoopQueue usart2_rec_queue;
static LoopQueue usart2_send_queue;

static char usart3_rec_buff[USART3_REC_MAX];
static char usart3_send_buff[USART3_SEND_MAX];
static LoopQueue usart3_rec_queue;
static LoopQueue usart3_send_queue;

/* 初始化串口接收发送队列，可调用getUsartSendLoopQueue()和getUsartRecLoopQueue()函数获取队列*/
unsigned char initUsartBuff(unsigned char id)
{
	if(id>=USART_MAX_ID)
		return 0;
	
	if(USART1_ID==id)
	{
		initCharLoopQueue(&usart1_rec_queue, (unsigned char*)usart1_rec_buff, USART1_REC_MAX, 0);
		initCharLoopQueue(&usart1_send_queue, (unsigned char*)usart1_send_buff, USART1_SEND_MAX, 0);
		return 1;
	}else if(USART2_ID==id)
	{
		initCharLoopQueue(&usart2_rec_queue, (unsigned char*)usart2_rec_buff, USART2_REC_MAX, 0);
		initCharLoopQueue(&usart2_send_queue, (unsigned char*)usart2_send_buff, USART2_SEND_MAX, 0);
		return 1;
	}else if(USART3_ID==id)
	{
		initCharLoopQueue(&usart3_rec_queue, (unsigned char*)usart3_rec_buff, USART3_REC_MAX, 0);
		initCharLoopQueue(&usart3_send_queue, (unsigned char*)usart3_send_buff, USART3_SEND_MAX, 0);
		return 1;		
	}
	
	return 0;
}


unsigned char write(unsigned char id, char* pc, unsigned int len)
{
	if(id>=USART_MAX_ID || 0==pc || 0==len)
	return 0;
	
	if(USART1_ID==id)
	{
		unsigned int dataLen = len;
		for( ; dataLen>0; --dataLen )
		{
			insertCharLoopQueue( &usart1_send_queue, (unsigned char)(*(unsigned char*)pc));
			++pc;
		}
		return len;		

	}else if(USART2_ID==id)
	{
		unsigned int dataLen = len;
		for( ; dataLen>0; --dataLen )
		{
			insertCharLoopQueue( &usart2_send_queue, (unsigned char)(*(unsigned char*)pc));
			++pc;
		}
		return len;		

	}else if(USART3_ID==id)
	{
		unsigned int dataLen = len;
		for( ; dataLen>0; --dataLen )
		{
			insertCharLoopQueue( &usart3_send_queue, (unsigned char)(*(unsigned char*)pc));
			++pc;
		}
		return len;		
	}
	
	return 0;
}

unsigned int read(unsigned char id, char* buff, unsigned int Len)
{
	int dataLen = 0;
	unsigned int rlen = 0;
	unsigned int llen = 0;
	
	if(id>=USART_MAX_ID || 0==buff || 0==Len )
		return 0;
	
	if(USART1_ID==id)
	{
		dataLen = loopQueueLength(&usart1_rec_queue);
		if(0==dataLen) 
			return 0;
		
		if(dataLen>Len) 
			llen = Len;
		else 
			llen = dataLen;
		rlen = llen;
		
		for( ; llen>0; --llen)
		{
			*buff = (char)readCharLoopQueue(&usart1_rec_queue);
			++buff;
		}			
		
		return rlen;		

	}else if(USART2_ID==id)
	{
		dataLen = loopQueueLength(&usart2_rec_queue);
		if(0==dataLen) 
			return 0;
		
		if(dataLen>Len) 
			llen = Len;
		else 
			llen = dataLen;
		rlen = llen;
		
		for( ; llen>0; --llen)
		{
			*buff = (char)readCharLoopQueue(&usart2_rec_queue);
			++buff;
		}			
		
		return rlen;		
	}else if(USART3_ID==id)
	{		
		dataLen = loopQueueLength(&usart3_rec_queue);
		if(0==dataLen) 
			return 0;
		
		if(dataLen>Len) 
			llen = Len;
		else 
			llen = dataLen;
		rlen = llen;
		
		for( ; llen>0; --llen)
		{
			*buff = (char)readCharLoopQueue(&usart3_rec_queue);
			++buff;
		}			
		
		return rlen;		
	}
	
	return 0;
}

unsigned int readBuffLen(unsigned char id)
{
	if(id>=USART_MAX_ID)
	return 0;
	
	if(USART1_ID==id)
	{
		return (unsigned int)loopQueueLength(&usart1_rec_queue);
	}else if(USART2_ID==id)
	{
		return (unsigned int)loopQueueLength(&usart2_rec_queue);
	}else if(USART3_ID==id)
	{
			return (unsigned int)loopQueueLength(&usart3_rec_queue);
	}
	return 0;
}

unsigned int writeBuffLen(unsigned char id)
{
	if(id>=USART_MAX_ID)
	return 0;
	
	if(USART1_ID==id)
	{
		return (unsigned int)loopQueueLength(&usart1_send_queue);
	}else if(USART2_ID==id)
	{
		return (unsigned int)loopQueueLength(&usart2_send_queue);
	}else if(USART3_ID==id)
	{
			return (unsigned int)loopQueueLength(&usart3_send_queue);
	}
	return 0;
}


LoopQueue* getUsartSendLoopQueue(unsigned char id)
{
	if(id>=USART_MAX_ID)
		return 0;
	
	if(USART1_ID==id)
	{
		return &usart1_send_queue;	
	}else if(USART2_ID==id)
	{
		return &usart2_send_queue;	
	}else if(USART3_ID==id)
	{
		return &usart3_send_queue;	
	}
	return 0;
}
LoopQueue* getUsartRecLoopQueue(unsigned char id)
{
	if(id>=USART_MAX_ID)
		return 0;
	
	if(USART1_ID==id)
	{
		return &usart1_rec_queue;	
	}else if(USART2_ID==id)
	{
		return &usart2_rec_queue;	
	}else if(USART3_ID==id)
	{
		return &usart3_rec_queue;	
	}
	return 0;
}

void usartClearRxBuff(unsigned char id)
{
	if(id>=USART_MAX_ID)
		return ;
	
	if(USART1_ID==id)
	{
		clearLoopQueue(&usart1_rec_queue);
	}else if(USART2_ID==id)
	{

	}else if(USART3_ID==id)
	{

	}
}

void usartClearTxBuff(unsigned char id)
{
	if(id>=USART_MAX_ID)
		return ;
	
	if(USART1_ID==id)
	{
		clearLoopQueue(&usart1_send_queue);
	}else if(USART2_ID==id)
	{

	}else if(USART3_ID==id)
	{

	}
}

/* stm32f1xx_hal_uart.h文件中修改函数 UART_Receive_IT */

//static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
//{
//  uint16_t* tmp;
//  
//  /* Check that a Rx process is ongoing */
//  if(huart->RxState == HAL_UART_STATE_BUSY_RX) 
//  {
//    if(huart->Init.WordLength == UART_WORDLENGTH_9B)
//    {
//      tmp = (uint16_t*) huart->pRxBuffPtr;
//      if(huart->Init.Parity == UART_PARITY_NONE)
//      {
//        //*tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
//        //huart->pRxBuffPtr += 2U;
//      }
//      else
//      {
//        //*tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
//        //huart->pRxBuffPtr += 1U;
//      }
//    }
//    else
//    {
//      if(huart->Init.Parity == UART_PARITY_NONE)
//      {
//        //*huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
//				if(USART1==huart->Instance)
//					insertCharLoopQueue( getUsartRecLoopQueue(USART1_ID), huart->Instance->DR & (uint8_t)0x00FF);
//				else if((USART2==huart->Instance))
//					insertCharLoopQueue( getUsartRecLoopQueue(USART2_ID), huart->Instance->DR & (uint8_t)0x00FF);
//				else if((USART3==huart->Instance))
//					insertCharLoopQueue( getUsartRecLoopQueue(USART3_ID), huart->Instance->DR & (uint8_t)0x00FF);
//      }
//      else
//      {
//        //*huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
//				if(USART1==huart->Instance)
//					insertCharLoopQueue( getUsartRecLoopQueue(USART1_ID), huart->Instance->DR & (uint8_t)0x007F);
//				else if(USART2==huart->Instance)
//					insertCharLoopQueue( getUsartRecLoopQueue(USART2_ID), huart->Instance->DR & (uint8_t)0x007F);
//				else if(USART3==huart->Instance)
//					insertCharLoopQueue( getUsartRecLoopQueue(USART3_ID), huart->Instance->DR & (uint8_t)0x007F);
//      }
//    }

//    if(--huart->RxXferCount == 0U)
//    {
//      /* Disable the IRDA Data Register not empty Interrupt */
//      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

//      /* Disable the UART Parity Error Interrupt */
//      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
//        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
//        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

//      /* Rx process is completed, restore huart->RxState to Ready */
//      huart->RxState = HAL_UART_STATE_READY;

//      HAL_UART_RxCpltCallback(huart);

//      return HAL_OK;
//    }
//    return HAL_OK;
//  }
//  else
//  {
//    return HAL_BUSY;
//  }
//}
