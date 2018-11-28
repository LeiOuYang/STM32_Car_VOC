
#ifndef GIZWITS_TYPE_H
#define GIZWITS_TYPE_H

	#define GIZWITS_PRTCL_HEADER 0xFFFF   /* 帧头 固定0xFFFF */ 
	
	#define GIZWITS_PRTCL_MAX_LENGTH  65535  /* 最大包长 */ 
	
	
	/* 指令定义 */
	#define MDL_GET_DEV_MSG  0X01  			/* 通信模组(wifi或者2G模块)向MCU设备获取信息 */ 
	#define MCU_GET_DEV_MSG_REPLY  0X02     /* MCU设备向通信模组传输设备信息 */ 
	
	#define MDL_HEARTBEAT 0X07				/* 通信模组发送给MCU的心跳包 */ 
	#define MCU_HEARTBEAT 0X08				/* MCU发送给通信模组心跳包 */ 
	
	#define MDL_STATUS	0X0D 				/* 通信模组状态，可判断模块时候连接服务器，APP是否在线等状态*/ 
	#define MCU_STATUS_REPLY 0X0E			/* MCU应答收到通信模组状态 */ 
	
	#define MCU_RESTART 0X0F  				/* 通信模组发送MCU重启指令 */ 
	#define MCU_RESTART_REPLY 0X10			/* MCU重启指令应答 */
	
	#define MCU_GET_MDL_MSG 0X21			/* MCU发送获取通信模组信息指令 */ 
	#define MCU_GET_MDL_MSG_REPLY 0X22		/* 通信模组回复基本信息 */	 
	
	#define MDL_READ_MCU_DATA 0X03			/* 通信模块读取MCU状态数据 */
	#define MCU_REPLAY_MDL_DATA 0X04		/* MCU设备应答指令 */
	#define MCU_SEND_DATA_MDL 0X05			/* MCU设备数据发送至通信模块 */ 
	#define MDL_REC_DATA_RETURN 0X06        /* 通信模块获取MCU设备数据应答指令 */ 
	
	#define MDL_READ_DATA_ACTION_ALL 0X12	/* 通信模块读取MCU设备数据指令 */ 
	#define MCU_REPLY_DATA_ACTION_ALL 0X13	/* MCU设备数据发送之通信模块动过指令 */ 
	#define MDL_DATA_VALID_ACTION 0X11		/* 数据位有效指令 */ 
	#define MDL_DATA_RETURN_ACTION 0X14     /* MCU主动上传数据动作 */ 
	
	
	/* 用户数据有效位定义 */
	#define ALL_DATA_BIT_VALID 	0X01FF/* 所有数据位有效 */	
	#define GET_ALL_DATA_VALID  (unsigned short)(0X01<<0)
	#define GPS_ERROR_VALID    	(unsigned short)(0X01<<1)
	#define HDT11_ERROR_VALID		(unsigned short)(0X01<<2)
	#define TVOC_ERROR_VALID 		(unsigned short)(0X01<<3) 
	#define RGB_VALID						(unsigned short)(0X01<<4)
	#define BEEP_VALID					(unsigned short)(0X01<<5)
	#define HR_VALID						(unsigned short)(0X01<<6)
	#define TVOC_PPM_DATA_VALID (unsigned short)(0X01<<7)
	#define TEMP_DATA_VALID 		(unsigned short)(0X01<<8)
	#define GPS_LOCATION_VALID 	(unsigned short)(0X01<<9)
	
	/* MCU设备信息，硬件、软件版本号，产品密钥，产品标志码 */ 
	#define GIZWITS_USART_VERSION  "00000004"   
	#define GIZWITS_PRO_VERSION    "00000004"
	#define MCU_HARDWARE_VERSION   "00000006"
	#define MCU_SOFTWARE_VERSION   "00000005"
	#define GIZWITS_PRODUCT_KEY	   "223b4b874c9f4188a0054a23a8711c5c"  
	#define MCU_DEV_VERSION GIZWITS_USART_VERSION GIZWITS_PRO_VERSION MCU_HARDWARE_VERSION MCU_SOFTWARE_VERSION GIZWITS_PRODUCT_KEY
	#define GIZWITS_PRODUCT_SECRET "cc65ef5ea00041a296a7987addd76bc5"

/* 数据帧格式： 
*	帧头 0xFFFF(2B) + 包长(2B) + 指令(1B) + 包序号(1B) + flags(2B) + 有效数据 + 校验和(1B) 
*   注意：
*		1、数据从高位开始传输，校验和算法为包长到所有有效数据累加和低八位 
*		2、包序号依次递增，1-255，满为0 
*   3、发送方帧头外的0xFF后需要添加数据0x55，解析时删除0x55 
*/	
	typedef struct gizwits_pack
	{
		unsigned short header;
		unsigned short data_len;
		unsigned char command;
		unsigned char seq;
		unsigned short flag;
		char data[299];
		unsigned char crc;
		unsigned short length;		
	}gizwits_pack;
	 
	union gizwits_union
	{
		gizwits_pack gizwits;
		unsigned char data[310];
	};
	
/* 过程状态保存 */
	typedef struct gizwits_result
	{
		unsigned char result;
		unsigned char  parse;
	}gizwits_result;
	

/* 解析过程枚举定义 */ 
	typedef enum gizwits_parse
	{
		GIZWITS_PARSE_NONE = 0,
		GIZWITS_PARSE_HEADER01,
		GIZWITS_PARSE_HEADER02,
		GIZWITS_PARSE_LENGTH01,
		GIZWITS_PARSE_LENGTH02,
		GIZWITS_PARSE_CMD,
		GIZWITS_PARSE_SEQ,
		GIZWITS_PARSE_FLAG01,
		GIZWITS_PARSE_FLAG02,
		GIZWITS_PARSE_CRC,
		GIZWITS_PARSE_OK
	}GIZWITS_PARSE;

/* 解析数据结果状态 */	
	typedef enum _gizwits_result
	{
		GIZWITS_RESULT_NONE = 0,
		GIZWITS_RESULT_ERR,
		GIZWITS_RESULT_OK,
	}GIZWITS_RESULT;
	
	
/* 通信模块状态 */
	typedef struct gizwits_status
	{
		unsigned char gizwits_exits: 1;
		unsigned char base_station_exist: 1;
		unsigned char m2m_server_exist: 1;
		unsigned char rssi: 3;
		unsigned char app_exist: 1;	
		unsigned char mdl_reply;
		unsigned char atr_value; 
		unsigned char action;
		unsigned char data_node;  /* 数据节点，控制MCU设备使用 */
		unsigned short atr_flag;
	}gizwits_status; 

#endif

