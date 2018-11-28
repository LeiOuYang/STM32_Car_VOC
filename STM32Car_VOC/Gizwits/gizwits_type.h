
#ifndef GIZWITS_TYPE_H
#define GIZWITS_TYPE_H

	#define GIZWITS_PRTCL_HEADER 0xFFFF   /* ֡ͷ �̶�0xFFFF */ 
	
	#define GIZWITS_PRTCL_MAX_LENGTH  65535  /* ������ */ 
	
	
	/* ָ��� */
	#define MDL_GET_DEV_MSG  0X01  			/* ͨ��ģ��(wifi����2Gģ��)��MCU�豸��ȡ��Ϣ */ 
	#define MCU_GET_DEV_MSG_REPLY  0X02     /* MCU�豸��ͨ��ģ�鴫���豸��Ϣ */ 
	
	#define MDL_HEARTBEAT 0X07				/* ͨ��ģ�鷢�͸�MCU�������� */ 
	#define MCU_HEARTBEAT 0X08				/* MCU���͸�ͨ��ģ�������� */ 
	
	#define MDL_STATUS	0X0D 				/* ͨ��ģ��״̬�����ж�ģ��ʱ�����ӷ�������APP�Ƿ����ߵ�״̬*/ 
	#define MCU_STATUS_REPLY 0X0E			/* MCUӦ���յ�ͨ��ģ��״̬ */ 
	
	#define MCU_RESTART 0X0F  				/* ͨ��ģ�鷢��MCU����ָ�� */ 
	#define MCU_RESTART_REPLY 0X10			/* MCU����ָ��Ӧ�� */
	
	#define MCU_GET_MDL_MSG 0X21			/* MCU���ͻ�ȡͨ��ģ����Ϣָ�� */ 
	#define MCU_GET_MDL_MSG_REPLY 0X22		/* ͨ��ģ��ظ�������Ϣ */	 
	
	#define MDL_READ_MCU_DATA 0X03			/* ͨ��ģ���ȡMCU״̬���� */
	#define MCU_REPLAY_MDL_DATA 0X04		/* MCU�豸Ӧ��ָ�� */
	#define MCU_SEND_DATA_MDL 0X05			/* MCU�豸���ݷ�����ͨ��ģ�� */ 
	#define MDL_REC_DATA_RETURN 0X06        /* ͨ��ģ���ȡMCU�豸����Ӧ��ָ�� */ 
	
	#define MDL_READ_DATA_ACTION_ALL 0X12	/* ͨ��ģ���ȡMCU�豸����ָ�� */ 
	#define MCU_REPLY_DATA_ACTION_ALL 0X13	/* MCU�豸���ݷ���֮ͨ��ģ�鶯��ָ�� */ 
	#define MDL_DATA_VALID_ACTION 0X11		/* ����λ��Чָ�� */ 
	#define MDL_DATA_RETURN_ACTION 0X14     /* MCU�����ϴ����ݶ��� */ 
	
	
	/* �û�������Чλ���� */
	#define ALL_DATA_BIT_VALID 	0X01FF/* ��������λ��Ч */	
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
	
	/* MCU�豸��Ϣ��Ӳ��������汾�ţ���Ʒ��Կ����Ʒ��־�� */ 
	#define GIZWITS_USART_VERSION  "00000004"   
	#define GIZWITS_PRO_VERSION    "00000004"
	#define MCU_HARDWARE_VERSION   "00000006"
	#define MCU_SOFTWARE_VERSION   "00000005"
	#define GIZWITS_PRODUCT_KEY	   "223b4b874c9f4188a0054a23a8711c5c"  
	#define MCU_DEV_VERSION GIZWITS_USART_VERSION GIZWITS_PRO_VERSION MCU_HARDWARE_VERSION MCU_SOFTWARE_VERSION GIZWITS_PRODUCT_KEY
	#define GIZWITS_PRODUCT_SECRET "cc65ef5ea00041a296a7987addd76bc5"

/* ����֡��ʽ�� 
*	֡ͷ 0xFFFF(2B) + ����(2B) + ָ��(1B) + �����(1B) + flags(2B) + ��Ч���� + У���(1B) 
*   ע�⣺
*		1�����ݴӸ�λ��ʼ���䣬У����㷨Ϊ������������Ч�����ۼӺ͵Ͱ�λ 
*		2����������ε�����1-255����Ϊ0 
*   3�����ͷ�֡ͷ���0xFF����Ҫ�������0x55������ʱɾ��0x55 
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
	
/* ����״̬���� */
	typedef struct gizwits_result
	{
		unsigned char result;
		unsigned char  parse;
	}gizwits_result;
	

/* ��������ö�ٶ��� */ 
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

/* �������ݽ��״̬ */	
	typedef enum _gizwits_result
	{
		GIZWITS_RESULT_NONE = 0,
		GIZWITS_RESULT_ERR,
		GIZWITS_RESULT_OK,
	}GIZWITS_RESULT;
	
	
/* ͨ��ģ��״̬ */
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
		unsigned char data_node;  /* ���ݽڵ㣬����MCU�豸ʹ�� */
		unsigned short atr_flag;
	}gizwits_status; 

#endif

