
#include "gizwits_protocol.h"

static gizwits_pack gizwits;
static gizwits_result gizwits_r;
static gizwits_status gizwits_s;

/*
*	���ϲ�Ӧ�õ��ã��ñ�����Ӧ�������ݻ��� 
*/
gizwits_pack* get_gizwits(void)
{
	return &gizwits;
}

gizwits_status* get_gizwits_status(void)
{
	return &gizwits_s;
}

/* 
*	��ʼ�����õ� gizwits_pack ��  gizwits_result
*
 */
unsigned char gizwits_init(void)
{
	gizwits_r.parse = GIZWITS_PARSE_NONE;
	gizwits_r.result = GIZWITS_RESULT_NONE;
	gizwits_s.app_exist = 0;
	gizwits_s.base_station_exist = 0;
	gizwits_s.gizwits_exits = 0;
	gizwits_s.m2m_server_exist = 0;
	gizwits_s.rssi = 0;
	gizwits_s.atr_flag = 0;
	gizwits_s.atr_value = 0;
	gizwits_s.mdl_reply = 0;
	
	return memery_set((char*)&gizwits, sizeof(gizwits_pack), 0);
}

/* 
*	���ݴ�� 
*/
unsigned char gizwits_pack_char(gizwits_pack* pgp, unsigned char command, const char* data, unsigned short len, unsigned short flag)
{
	unsigned short count = 0;
	unsigned int crc_sum = 0;
	
	if( (void*)0 == pgp ) return 0;
	
	pgp->header = 0xffff;
	
	pgp->command = command;
	crc_sum += command;
	
	++pgp->seq;
	pgp->seq = pgp->seq%256;
	crc_sum += pgp->seq;
	
	pgp->flag = ((flag&0x00ff)<<8) | ((flag&0xff00)>>8);
	crc_sum += flag;
	
	while(len--)
	{
		pgp->data[count] = data[count];
		crc_sum += data[count];
		if(0xff==data[count])
		{
			++count;
			pgp->data[count] = 0x55;
			crc_sum += 0x55;
		}
		++count;
	}
	crc_sum += count+5;
	pgp->crc = crc_sum%256;
	pgp->data_len = (((count+5)&0x00ff)<<8) | (((count+5)&0xff00)>>8);
	pgp->data[count] = pgp->crc;
	pgp->length = count+9;
	
	return 1;
}


/* 
*	����ַ�����gizwitsģ������ 
*
*/
GIZWITS_RESULT gizwits_parse_char(unsigned char c, gizwits_pack* pg_pack, gizwits_result* pg_result)
{	
	unsigned char error = 0;
	
	if( (void*)0==pg_pack || (void*)0==pg_result ) return GIZWITS_RESULT_ERR;
	
	switch((GIZWITS_PARSE)pg_result->parse)
	{
		case GIZWITS_PARSE_NONE:
			if(0xff==c)
			{
				pg_result->parse = GIZWITS_PARSE_HEADER01;
				pg_result->result = GIZWITS_RESULT_NONE;
				memery_set((char*)pg_pack, sizeof(gizwits_pack), 0);
			}else 
			{
				error = 1;
			}				
			break;
			
		case GIZWITS_PARSE_HEADER01:
			if(0xff==c)
			{
				pg_result->parse = GIZWITS_PARSE_HEADER02;
			}else
			{
				error = 1;
			}
			break;
			
		case GIZWITS_PARSE_HEADER02:
			pg_pack->data_len = (unsigned short)c << 8;
			pg_result->parse = GIZWITS_PARSE_LENGTH01;
			pg_pack->crc += c;
			break;
			
		case GIZWITS_PARSE_LENGTH01:
			pg_pack->data_len += c;
			pg_result->parse = GIZWITS_PARSE_LENGTH02;
			pg_pack->crc += c;
			break;
			
		case GIZWITS_PARSE_LENGTH02:
			pg_pack->command = c;
			pg_result->parse = GIZWITS_PARSE_CMD;
			pg_pack->crc += c;
			break;
			
		case GIZWITS_PARSE_CMD:
			pg_pack->seq = c;
			pg_result->parse = GIZWITS_PARSE_SEQ;
			pg_pack->crc += c;
			break;
			
		case GIZWITS_PARSE_SEQ:
			pg_pack->flag = (unsigned short)c << 8;
			pg_result->parse = GIZWITS_PARSE_FLAG01;
			pg_pack->crc += c;
			break;
			
		case GIZWITS_PARSE_FLAG01:
			pg_pack->flag += c;
			pg_result->parse = GIZWITS_PARSE_FLAG02;
			pg_pack->crc += c;
			break;
			
		case GIZWITS_PARSE_FLAG02:
			if(pg_pack->length==pg_pack->data_len-5)
			{
				if(c==pg_pack->crc)
				{
					pg_result->parse = GIZWITS_PARSE_OK;
					pg_result->result = GIZWITS_RESULT_OK;
				}else
					error = 1;
			}else if(pg_pack->length < pg_pack->data_len-5) 			
			{
				pg_pack->data[pg_pack->length++] = c;
				pg_pack->crc += c;
			}else error = 1;
			
			break;
			
		default: break;
	}
	
	if(error)
	{
		pg_result->parse = GIZWITS_PARSE_NONE;
		pg_result->result = GIZWITS_RESULT_ERR;
		return GIZWITS_RESULT_ERR;
	}	
	
	if(GIZWITS_PARSE_OK==pg_result->parse)
		pg_result->parse = GIZWITS_PARSE_NONE;
	
	return pg_result->result;
}

/* 
*	�����ɹ�������ݽ��д�������������gizwits_parse_char���� 
*	 	
 */
unsigned char gizwits_data_process(gizwits_pack* pg_pack, gizwits_pack* pg_pack_send)
{
	gizwits_pack* pgp = (void*)0;
	
	if( (void*)0==pg_pack || (void*)0==pg_pack_send ) return 0;
	
	switch(pg_pack->command)
	{
		/* ��ȡMCU�豸��Ϣָ�� */ 
		case MDL_GET_DEV_MSG:
			gizwits_reply_pack(pg_pack_send, MCU_GET_DEV_MSG_REPLY);
			break;
			
		/* ������ָ�� */
		case MDL_HEARTBEAT:

			gizwits_reply_pack(pg_pack_send, MCU_HEARTBEAT);	
			gizwits_s.gizwits_exits = 1;		
			break;
		
		/* ͨ��ģ��״ָ̬�� */
		case MDL_STATUS:
			{
				unsigned short len = 0;
				//unsigned short data = 0;
				
				gizwits_reply_pack(pg_pack_send, MCU_STATUS_REPLY);
				len = pg_pack->data_len;
				//data = (unsigned char)pg_pack->data[len-2-5] + (((unsigned short)pg_pack->data[len-1-5])<<8);
							
				gizwits_s.app_exist = (pg_pack->data[len-2-5]&0x08)==0x08;
				gizwits_s.base_station_exist = (pg_pack->data[len-1-5]&0x1A)==0x1A;  
				gizwits_s.m2m_server_exist = (pg_pack->data[len-1-5]&0x2A)==0x2A; 
				gizwits_s.gizwits_exits = 1;
				gizwits_s.rssi = pg_pack->data[len-2-5]&0x07;
				
				break;
			}
			
		/* MCU�豸��λָ�� */
		case MCU_RESTART:
			gizwits_reply_pack(pg_pack_send, MCU_RESTART_REPLY);
			break;
			
		/* ͨ��ģ��ظ���Ϣָ�� */
		case MCU_GET_MDL_MSG_REPLY:
			return 0;
			break;
			
		/* ģ����ȷ�յ�����Ӧ��ָ�� */ 
		case MDL_REC_DATA_RETURN:
			gizwits_s.mdl_reply = 0;
			return 0;
			break; 
			
		/* ͨ��ģ���ȡMCU״̬����ָ�� */
		case MDL_READ_MCU_DATA:
			{
				unsigned short len = 0;
				len = pg_pack->data_len;
				
				gizwits_s.atr_flag = pg_pack->data[len-2-5];
				gizwits_s.atr_value = pg_pack->data[len-1-5];
				gizwits_reply_pack(pg_pack_send, MCU_REPLAY_MDL_DATA);
				break;	
			}
			
		default: return 0; break;
		
	}
	return 1; 
}

/*
*	��װ���Ͱ� 
*
*/ 
unsigned char gizwits_reply_pack(gizwits_pack* pg_pack, unsigned char command) 
{

	if( (void*)0 == pg_pack ) return 0;
	
	switch(command)
	{
		case MCU_HEARTBEAT:
			gizwits_pack_char(pg_pack, MCU_HEARTBEAT, (void*)0, 0, 0);
			break;
			
		case MCU_GET_DEV_MSG_REPLY:
			{
				unsigned char data[120] = {0};
				
				/* ���ư汾��Ϣ���ַ��� */
				string_copy((char*)data, MCU_DEV_VERSION);
				
				/* �ɰ�״̬ʱ��  2���ֽ�  ������ʱΪ0 */ 
				data[sizeof(MCU_DEV_VERSION)-1] = 0;
				data[sizeof(MCU_DEV_VERSION)] = 0;
				
				/* 8���ֵ��豸���� */
				data[sizeof(MCU_DEV_VERSION) + 1] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 2] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 3] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 4] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 5] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 6] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 7] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 8] = 0x00;
				
				/* 32�ֽڲ�Ʒ��Կ */
				string_copy((char*)&data[sizeof(MCU_DEV_VERSION) + 9], GIZWITS_PRODUCT_SECRET);
				
				gizwits_pack_char(pg_pack, MCU_GET_DEV_MSG_REPLY, data, 106, 0); 
			
			} 
		
			 
			break;
		
		case MCU_STATUS_REPLY:
			gizwits_pack_char(pg_pack, MCU_STATUS_REPLY, (void*)0, 0, 0);
			break;
			
		case MCU_RESTART_REPLY:
			gizwits_pack_char(pg_pack, MCU_RESTART_REPLY, (void*)0, 0, 0);
			break;
			
		case MCU_GET_MDL_MSG:
			{
				unsigned char data = 0x00;
				gizwits_pack_char(pg_pack, MCU_GET_MDL_MSG, (const char*)&data, 1, 0);
				break;
			}
			
		case MCU_REPLAY_MDL_DATA:
			gizwits_pack_char(pg_pack, MCU_REPLAY_MDL_DATA, (void*)0, 0, 0);
			break;
		
		/* ����������ͨ��ģ�� */	
		case MCU_SEND_DATA_MDL:
			
			break;
			
		default: return 0;
	}
	
	
	return 1;
}

/* �û�ʵ�ֿɱ䳤���ݴ��亯�� */
unsigned char  gizwits_data_pack(gizwits_pack* pg_pack, char* data, unsigned int len, unsigned short data_class)
{
	unsigned short temp = 0;
	
	if( (void*)0==pg_pack || (void*)0==data ) return 0;
	
	temp = data_class;
	
	/* ���е�������Ч */ 
	if((temp&DATA_VALID)==DATA_VALID)
	{
		if(len!=51) return 0;
		gizwits_pack_char(pg_pack, MCU_SEND_DATA_MDL, (const char*)data, 51, 0);	
		gizwits_s.mdl_reply = 1;	
	}
	
	return 1;

}
