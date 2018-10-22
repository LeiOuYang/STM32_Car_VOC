
#ifndef UBLOX_NMEA0183_H
#define UBLOX_NMEA0183_H
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define IMPORT_FLOAT 0   //֧�ָ������� 1֧��

	#define GPGSA	 "GPGSA"
	#define GPGGA  "GPGGA"
	#define GPGSV  "GPGSV"
	#define GPRMC  "GPRMC"
	#define GPVTG  "GPVTG"
	#define GPGLL  "GPGLL"
	
	#define GNGSA	 "GNGSA"
	#define GNGGA  "GNGGA"
	#define GNGSV  "GNGSV"
	#define GNRMC  "GNRMC"
	#define GNVTG  "GNVTG"
	#define GNGLL  "GNGLL"
	
	#define GLGSA	 "GLGSA"
	#define GLGGA  "GLGGA"
	#define GLGSV  "GLGSV"
	#define GLRMC  "GLRMC"
	#define GLVTG  "GLVTG"
	#define GLGLL  "GLGLL"
	
	#define GBGSA	 "GBGSA"
	#define GBGGA  "GBGGA"
	#define GBGSV  "GBGSV"
	#define GBRMC  "GBRMC"
	#define GBVTG  "GBVTG"
	#define GBGLL  "GBGLL"
	
	#define BDGSA	 "BDGSA"
	#define BDGGA  "BDGGA"
	#define BDGSV  "BDGSV"
	#define BDRMC  "BDRMC"
	#define BDVTG  "BDVTG"
	#define BDGLL  "BDGLL"
	
	#define NMEA_NO	0
	#define NMEA_START 1 
	#define NMEA_CRC1 2
	#define NMEA_CRC2 3
	#define NMEA_END 4
	
//define enum
	typedef enum _gps_message_flag
	{
		GPS_MSG_NO = 0,
		GPS_MSG_GGA, 	//��λ��Ϣ
		GPS_MSG_GSA,  //��ǰ������Ϣ
		GPS_MSG_GSV,	//�ɼ�������Ϣ
		GPS_MSG_RMC,	//�Ƽ���λ��Ϣ���ݸ�ʽ	
		GPS_MSG_VTG,  //�����ٶ���Ϣ
		GPS_MSG_GLL,  //����λ��Ϣ
	}gps_message_flag;
//end enum define 
	
//define the gps message struct	
	typedef struct _gps_message
	{
	#if IMPORT_FLOAT==1  //֧�ָ�������
		double longitude;  //����
		double latitude;   //γ��
		double altitude;	 //�߶�
		float pdop; //λ�þ���
		float hdop;	//ˮƽ����
		float vdop;	//��ֱ����
	#endif
		float groundSpeed;
		char lat_direc;
		char long_direc;
		/*
		*  B  L  P
		*	 3  2  1
		*  0  0  1  - GPS
		*  0  1  1  - GPS+������˹ 
		*  1  0  1  - GPS+BD
		*  0  1  1  - BD+������˹ 
		*  0  1  0  - ������˹ 
		*  1  1  0  - BD+������˹ 
		*  1  0  0  - BD
		*  1  1  1  - GPS+������˹+BD
		*/
		unsigned char sttl_class;           //��ǰʹ�õ����� 
		unsigned char scan_satellite_count; //�յ������źŵ�����
		unsigned char use_satellite_count; //����ʹ�õ����Ǹ���
		unsigned char gp_scan_count;       //ɨ��
		unsigned char gb_scan_count;
		unsigned char gl_scan_count;
//		unsigned char gp_use_count;				//ʹ��
//		unsigned char gb_use_count;
//		unsigned char gl_use_count;
		unsigned char gps_per; 		/*GPS״̬ 0-������fix not��1-���㶨λGPS FIX
															*2-��ֶ�λDGPS��3-��ЧPPS��4-ʵʱ��ֶ�λRTK FIX
															*5-RTK FLOAT��6-���ڹ���*/
		bool data_val;   //��λ������Ч
		unsigned char mode; //��λģʽ���ֶ������Զ� A  M
		unsigned char fix_mode; //��λ���� 1 = δ��λ, 2 = ��ά��λ, 3 = 3ά��λ
		unsigned char val;     //������Ч
	}gps_message;
//end gps_message struct

//���屣��������Ϣ�Ľṹ��	
	typedef struct _gps_gsv
	{
		unsigned char enable;  //ռ��״̬  1-ռ��  0-û��ռ�ã���ʾ�µ�������Ϣ���Ա���������
		unsigned char prn_number;
		unsigned char db;		
	}gps_gsv;
//end gps_gsv define 
		
	/* �����ӿ� */
	bool is_digital(char d);
#if IMPORT_FLOAT==1  //�ַ���ת�����㺯��
	double string_to_float(char* str, unsigned len);
#endif
	static void copy_string(char* dest,char *src,unsigned int num);
	bool init_char_buff(char* buff, unsigned int num, char value);
	bool get_nmea_frame(char usartC, char* frame, unsigned int* cou);//��ȡһ֡���� ��ʼ����'$' ��������'*' ���Ϊ�����ֽ�У��λ
	unsigned char hex_to_uint(char c); 			 //�ַ�ת��Ϊ����
	unsigned char char_to_int(char c);
	void nema_message_parse(char* frame, gps_message* pos, unsigned int len);		 //�����õ���Ϣ��
	
	void rmc_parse(char* frame, gps_message* pos, unsigned int len);  //����RMC���ݣ�������С��λ��Ϣ
	void gsv_parse(char* frame, gps_message* pos, unsigned int len);  //����GSV���ݣ��������Ǹ�ʽ
	void gsa_parse(char* frame, gps_message* pos, unsigned int len);  //����GSA���ݣ�GPS����ָ�뼰ʹ�����Ǹ�ʽ
	void gll_parse(char* frame, gps_message* pos, unsigned int len);  //����GLL����
	void gga_parse(char* frame, gps_message* pos, unsigned int len);  //����GGA���ݣ��̶��������
	void vtg_parse(char* frame, gps_message* pos, unsigned int len);  //����VTG���ݣ������ٶ���Ϣ
	
	static void update_stll_msg(gps_gsv* gsvMsg, unsigned char len, unsigned char prn, unsigned char dbh);  //�������ǵ���Ϣ
	
	gps_message* get_gps_message(void);
	void init_gps_message(gps_message* gpsMsg);
	
		/*��ȡGPS��Ϣ���ȡ�γ�ȡ��߶ȵ�*/
	char* get_gps_longitude_str(void);  
	char* get_gps_latitude_str(void);
	char* get_gps_altitude_str(void);
	char* get_gps_utc_date_str(void);
	char* get_gps_utc_time_str(void);
	char* get_gps_utc_str(void);
	char* get_utc(char* buff);
	char* get_gps_pdop_str(void);
	char* get_gps_hdop_str(void);
	char* get_gps_vdop_str(void);
	unsigned char get_db_average(void);
	
	void set_gps_longitude_str(char* str);  
	void set_gps_latitude_str(char* str);
	void set_gps_altitude_str(char* str, unsigned int len);
	void set_gps_utc_date_str(char* str);
	void set_gps_utc_time_str(char* str);
	void set_gps_pdop_str(char* str, unsigned char len);
	void set_gps_hdop_str(char* str, unsigned char len);
	void set_gps_vdop_str(char* str, unsigned char len);
	
	void local_time(char* ddmmyy, char* hhmmss, unsigned char localTimeArea);
	
	void reset_gps_gsv(gps_gsv* gsv);
	unsigned char get_gps_msg_val(void);
	
	bool gps_valid(gps_message* gps);
	
	#define NMEA0183_FRAME_MAX_LEN		100   //NMEA����֡����󳤶�
	extern char nmea_buff[NMEA0183_FRAME_MAX_LEN];//�ⲿ���ã��ɽ������ɹ���һ֡���ݱ����ڸû�������get_nmea_frame()��nema_message_parse()��������
	extern gps_message gpsMsg;
	extern gps_gsv gpgsv[12];  
	extern gps_gsv glgsv[12];  
	extern gps_gsv gbgsv[12];
	
#endif /* UBLOX_NMEA0183_H */

