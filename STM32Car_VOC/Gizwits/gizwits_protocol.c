
#include "gizwits_protocol.h"

static gizwits_pack gizwits;
static gizwits_result gizwits_r;
static gizwits_status gizwits_s;

static DHT11* dht11;
static NMEA0183* nmea_data;
static AirSensor* air;

static void gizwits_process_node(unsigned short flag, unsigned char value);

void gizwits_set_param(DHT11* pdht11, NMEA0183* pnmea_data, AirSensor* pair)
{
	if((void*)0==pdht11 || (void*)0==pnmea_data || (void*)0==pair) return;
	
	dht11 = pdht11;
	nmea_data = pnmea_data;
	air = pair;
}

/*
*	供上层应用调用，该变量对应发送数据缓冲 
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
*	初始化内置的 gizwits_pack 和  gizwits_result
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
	gizwits_s.data_node = 0;
	gizwits_s.action = 0;
	
	return memery_set((char*)&gizwits, sizeof(gizwits_pack), 0);
}

/* 
*	数据打包 ,注意：有效数据长度不包括0xFF后面自动添加的0x55
*/
unsigned char gizwits_pack_char(gizwits_pack* pgp, unsigned char command, const char* data, unsigned short len, unsigned short flag)
{
	unsigned short count = 0;
	unsigned short data_len = 0;
	unsigned int crc_sum = 0;
	unsigned int re_count = 0;
	
	if( (void*)0 == pgp ) return 0;
	
	pgp->header = 0xffff;
	
	pgp->command = command;
	crc_sum += command;
	
	++pgp->seq;
	pgp->seq = pgp->seq%256;
	crc_sum += pgp->seq;
	
	pgp->flag = ((flag&0x00ff)<<8) | ((flag&0xff00)>>8);
	crc_sum += (flag>>8)&0x0ff;
	crc_sum += flag&0x0ff;
	
	while(len--)
	{
		pgp->data[count+re_count] = data[count];
		crc_sum += data[count];
		if(0xff==data[count])
		{
			++re_count;
			pgp->data[count+re_count] = 0x55;
			//crc_sum += 0x55;
		}
		++count;
	}
	
	data_len = count + 5;
	crc_sum += (data_len>>8)&0x0ff;
	crc_sum += data_len&0x0ff;
	pgp->crc = (unsigned char)(crc_sum%256);
	
	pgp->data_len = ((data_len&0x00ff)<<8) | ((data_len&0xff00)>>8); /* 包长不包括oxff后面的0x55 */
	pgp->data[count+re_count] = pgp->crc;
	pgp->length = count+re_count+9;
	
	return 1;
}


/* 
*	逐个字符解析gizwits模块数据 
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
				if(0xFF==pg_pack->data[pg_pack->length-1] && 0x55==c)  /* 0xFF后面的0x55不进行保存 */
				{
					pg_pack->crc += c;
					break;
				}if(0xFF==pg_pack->data[pg_pack->length-1])
				{
					error = 1;
					break;
				}
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
*	解析成功后对数据进行处理，解析调用gizwits_parse_char函数 
*	 	
 */
unsigned char gizwits_data_process(gizwits_pack* pg_pack, gizwits_pack* pg_pack_send)
{
	gizwits_pack* pgp = (void*)0;
	
	if( (void*)0==pg_pack || (void*)0==pg_pack_send ) return 0;
	
	switch(pg_pack->command)
	{
		/* 获取MCU设备信息指令 */ 
		case MDL_GET_DEV_MSG:
			gizwits_reply_pack(pg_pack_send, MCU_GET_DEV_MSG_REPLY);
			break;
			
		/* 心跳包指令 */
		case MDL_HEARTBEAT:

			gizwits_reply_pack(pg_pack_send, MCU_HEARTBEAT);	
			gizwits_s.gizwits_exits = 1;		
			break;
		
		/* 通信模组状态指令 */
		case MDL_STATUS:
			{
				unsigned short len = 0;
				//unsigned short data = 0;
				
				gizwits_reply_pack(pg_pack_send, MCU_STATUS_REPLY);
				len = pg_pack->data_len;
				//data = (unsigned char)pg_pack->data[len-2-5] + (((unsigned short)pg_pack->data[len-1-5])<<8);
							
				gizwits_s.app_exist = (pg_pack->data[0]&0x08)==0x08;
				gizwits_s.base_station_exist = (pg_pack->data[1]&0x1A)==0x1A;  
				gizwits_s.m2m_server_exist = (pg_pack->data[1]&0x2A)==0x2A; 
				gizwits_s.gizwits_exits = 1;
				gizwits_s.rssi = pg_pack->data[0]&0x07;
				
				if(gizwits_is_link())
					return 2;
				
				break;
			}
			
		/* MCU设备复位指令 */
		case MCU_RESTART:
			gizwits_reply_pack(pg_pack_send, MCU_RESTART_REPLY);
			break;
			
		/* 通信模组回复信息指令 */
		case MCU_GET_MDL_MSG_REPLY:
			return 0;
			break;
			
		/* 模组正确收到数据应答指令 */ 
		case MDL_REC_DATA_RETURN:
			gizwits_s.mdl_reply = 0;
			return 0;
			break; 
			
		/* 通信模块读取MCU状态数据指令 */
		case MDL_READ_MCU_DATA:
			{
				unsigned short len = 0;
				len = pg_pack->data_len;
				gizwits_s.atr_flag = (pg_pack->data[1]<<8)|pg_pack->data[2];
				gizwits_s.atr_value = pg_pack->data[3];
				gizwits_s.action = pg_pack->data[0];
				
				if(0x12==pg_pack->data[0]) /* action=0x12 */
				{
					if(gizwits_s.atr_flag&GET_ALL_DATA_VALID) /* 获取所有的数据 */
					{
						gizwits_reply_pack(pg_pack_send, MCU_REPLAY_MDL_DATA);
					}
				}else if(0x11==pg_pack->data[0])
				{
					gizwits_process_node(gizwits_s.atr_flag, gizwits_s.atr_value);   /* 处理数据节点 */
					//if((gizwits_s.atr_flag&GET_ALL_DATA_VALID) && (gizwits_s.atr_value&GET_ALL_DATA_VALID)) /* 获取所有的数据 */
					{
						return gizwits_reply_pack(pg_pack_send, MCU_REPLAY_MDL_DATA);
					}
				}
				break;	
			}
			
		default: return 0; break;
		
	}
	return 1; 
}

/*
*	封装发送包 
*
*/ 
unsigned	 char gizwits_reply_pack(gizwits_pack* pg_pack, unsigned char command) 
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
				static unsigned int count = 0;
				
				/* 复制版本信息等字符串 */
				string_copy((char*)data, MCU_DEV_VERSION);
				
				/* 可绑定状态时间  2个字节  保留暂时为0 */ 
				data[sizeof(MCU_DEV_VERSION)-1] = 0;
				data[sizeof(MCU_DEV_VERSION)] = 0;
				
				/* 8个字的设备属性 */
				data[sizeof(MCU_DEV_VERSION) + 1] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 2] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 3] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 4] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 5] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 6] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 7] = 0x00;
				data[sizeof(MCU_DEV_VERSION) + 8] = 0x02;
				
				/* 32字节产品密钥 */
				string_copy((char*)&data[sizeof(MCU_DEV_VERSION) + 9], GIZWITS_PRODUCT_SECRET);
				
				if(count++%12==0) /* 模块感觉有bug，不需要及时响应该指令，等待模块自我配置完成 */
					gizwits_pack_char(pg_pack, MCU_GET_DEV_MSG_REPLY, data, 106, 0); 
				
				return 3;
			
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
			
		case MCU_REPLAY_MDL_DATA:    /* 返回模块数据给通信模组 */
		{
			unsigned char data[60] = {0};
			unsigned short int t = 0;
			
			if(0x11==gizwits_s.action)
			{
				gizwits_pack_char(pg_pack, MCU_REPLAY_MDL_DATA, (void*)0, 0, 0);
				return 2;
	
			}else if(0x12==gizwits_s.action)
			{
				/* 传输数据 */
				data[0] = 0x13;
				data[1] = 0x03;
				data[2] = 0xFF;
				
				data[3] = gizwits_s.node.node[1];
				data[4] = gizwits_s.node.node[0];
				
				/* 湿度 */
				data[5] = (unsigned char)dht11->RH; 

				/* TVOC PPM */
				taskDISABLE_INTERRUPTS();				
				t = (unsigned short int )(air->air_ppm*10);
				taskENABLE_INTERRUPTS();
				data[6] = (unsigned char)((t&0x0FF00)>>8);
				data[7] = (unsigned char)(t&0x0FF);
				
				/* TEMP */
				taskDISABLE_INTERRUPTS();
				t = (unsigned short int )(dht11->TEMP*10);
				taskENABLE_INTERRUPTS();
				data[8] = (unsigned char)((t&0x0FF00)>>8);
				data[9] = (unsigned char)(t&0x0FF);
				
				/* 经纬度坐标 */
				for(t=10; t<46; ++t)
				{
					data[t] = t;
				}
				
				gizwits_pack_char(pg_pack, MCU_REPLAY_MDL_DATA, (const char*)data, 46, 0);
			}
			break;
		}
		
		/* 发送数据至通信模块 */	
		case MCU_SEND_DATA_MDL:
		{
			unsigned char data[60] = {0};
			unsigned short t = 0;
			/* 传输数据 */
			data[0] = 0x14;
			data[1] = 0x03;
			data[2] = 0xFF;
			
			data[3] = gizwits_s.node.node[1];
			data[4] = gizwits_s.node.node[0];
			
			/* 湿度 */
			data[5] = (unsigned char)dht11->RH; 

			/* TVOC PPM */
				taskDISABLE_INTERRUPTS();				
				t = (unsigned short int )(air->air_ppm*10);
				taskENABLE_INTERRUPTS();
				data[6] = (unsigned char)((t&0x0FF00)>>8);
				data[7] = (unsigned char)(t&0x0FF);
				
				/* TEMP */
				taskDISABLE_INTERRUPTS();
				t = (unsigned short int )(dht11->TEMP*10);
				taskENABLE_INTERRUPTS();
				data[8] = (unsigned char)((t&0x0FF00)>>8);
				data[9] = (unsigned char)(t&0x0FF);
			
			/* 经纬度坐标 */
			for(t=10; t<46; ++t)
			{
				data[t] = t;
			}
			
			gizwits_pack_char(pg_pack, MCU_SEND_DATA_MDL, (const char*)data, 46, 0);
			break;
		}
			
		default: return 0;
	}
	
	
	return 1;
}

/* 用户实现可变长数据传输函数 */
unsigned char  gizwits_data_pack(gizwits_pack* pg_pack, char* data, unsigned int len, unsigned short data_class)
{
	unsigned short temp = 0;
	
	if( (void*)0==pg_pack || (void*)0==data ) return 0;
	
	temp = data_class;
	
	/* 所有的数据有效 */ 
//	if((temp&DATA_VALID)==DATA_VALID)
//	{
//		if(len!=51) return 0;
//		gizwits_pack_char(pg_pack, MCU_SEND_DATA_MDL, (const char*)data, 51, 0);	
//		gizwits_s.mdl_reply = 1;	
//	}
//	
	return 1;

}


/* 判断模块是否成功连接服务器 */
unsigned char gizwits_is_link(void)
{
	/* 成功连接上基站和服务器 */
	if(gizwits_s.gizwits_exits && gizwits_s.base_station_exist && gizwits_s.m2m_server_exist)
	{
		return 1;
	}
	
	return 0;
}

static void gizwits_process_node(unsigned short flag, unsigned char value)
{
	if(flag&GET_ALL_DATA_VALID)
	{
		gizwits_s.data_node &= 0xFE;   					/* bit0 */
		gizwits_s.data_node |= (0x01&value);
		gizwits_s.node.data_node.get_data = (0x01&value);
	  return;
	}
	
	if(flag&RGB_VALID)
	{
		gizwits_s.data_node &= 0xF1;					  /* bit1-bit3 */
		gizwits_s.data_node |= (0x0E&(value<<1));
		gizwits_s.node.data_node.rgb_control = 0x07&value;
		return;
	}
	
	if(flag&BEEP_VALID)
	{
		gizwits_s.data_node &= 0xCF;         /* bit4-bit5 */
		gizwits_s.data_node |= (0x30&(value<<4));
		gizwits_s.node.data_node.beep_control = 0x02&value;
		return;
	}
}

