/*************************************************************************************
***
*** 		GNSS NEMA0183���ݽ�����֧�ֶԣ�RMC��GSV��GLL��TVS��GGA��GSA���ݰ��Ľ���	
***			�⺯������ʾ�����£�
***       char buff[];  //�û������д���Э������
***       char nmea_buff[]; //�洢һ֡NMEA���� 
***       unsigned int count = 0;              //����洢֡���ݵĸ���
***		  init_gps_message(get_gps_message()); //��ʼ�����ڲ������gps_message
***       for(int i=0; i<count; ++i)  		   //count��ʾ����������Ч���ݸ���
***		  {			
***     	if(get_nmea_frame(buff[i], &nmea_buff[0], &count) //���ַ�����������õ�һ֡���ݣ���Ϊ��
***       	{
***				nema_message_parse(nmea_buff[], get_gps_message(), count); //��һ֡���ݽ��н���
***			}
***		  }
***       �����ɹ��󣬿�ͨ���������õ�gpsMsg��gpgsv��gbgsv��gbgsv����ȡ��λ��Ϣ������״̬
*** 
***
***     BY Awesome OYL     2017-11-05 
***
**************************************************************************************/


#include "NMEA0183.h"
#include <stdio.h>

char nmea_buff[NMEA0183_FRAME_MAX_LEN] = {0};   //һ֡���ݻ����������ⲿ����
gps_message gpsMsg; //ȫ��gps������Ϣ
gps_gsv gpgsv[12];  //gps
gps_gsv glgsv[12];  //������˹
gps_gsv gbgsv[12];  //����

/* �ж��ַ��Ƿ�Ϊ�����ַ� */
bool is_digital(char d) 
{
	if(d>='0'&& d<='9'||'.'==d) 
		return true;
	else 
		return false;
}
//end function

/* �ַ���ת��Ϊ�������� len��Ҫת���ַ����ĳ��� */
#if IMPORT_FLOAT==1
double string_to_float(char* str,unsigned int len)
{
	unsigned char num[20];
	double d = 0.0;
	unsigned char flag = 1; 
	unsigned char count;
	unsigned int i = 0;
	unsigned int l = 0;
	unsigned char fuhao = 0;
	
	if(0==str) return 0.0;
	if('-'==*str) //ʶ�����
	{
		fuhao = 1;
		++str;
	}
	
	while('\0'!=*str)
	{
		if(!is_digital(*str)) //ʶ�������ַ�
			break;
		if(l==len)
		{
			if(1==flag)
			{
				while(count)
				{
					d += num[i]*pow(10,--count); 
					++i;
				}
			}
			break;
		}
		if('.'==*str)
		{
			flag=0;
			while(count)
			{
				d += num[i]*pow(10,--count); 
				++i;
			}
			if(l==len) 
			{
				break;
			}
		}
		if(1==flag)
		{
			num[count] = hex_to_uint(*str);
			++count;
		}else if(0==flag)
		{
			d += (double)hex_to_uint(*str)*(double)pow(10,-count); 
			++count;
		}
		++str;
		++l;
	}
	if(1==flag) 
	{
		while(count)
		{
			d += num[i]*pow(10,--count); 
			++i;
		}
	}
	if(1==fuhao) d = -d;
	return d;
}
#endif
//end function

/* ʮ�������ַ�ת��Ϊ���� */
unsigned char hex_to_uint(char c)
{
	if(c>='0'&& c<='9')
	{
		return (unsigned int)(c-48);
	}else if(c>='A' && c<='F')
	{
		return (unsigned int)(c-55);
	}
	return 0;
}
//end funciton

/* �����ַ���  ���ƽ���������'\0'��ָ������*/
static void copy_string(char* dest,char *src,unsigned int num) 
{ 
	if(dest!=0&&src!=0)
	{ while(num--&&'\0'!=*src)
		{
			*dest = *src;
			++dest;
			++src;
		} 
		*dest='\0'; 
	}
} 
//end function

/* ָ�����Ⱥͳ�ʼ��ֵ��ʼ���ַ����������ɹ�����true */
bool init_char_buff(char* buff, unsigned int num, char value)
{
	if(0==buff&&0==num)  return false;
	
	while(num--)
	{
		*buff = value;
		++buff;
	}
	return true;
}
//end function

/* ��ȡȫ��gps_message�ṹ�� */
gps_message* get_gps_message(void)
{
	return &gpsMsg;  //�����ȫ�ֱ����������б���gps����Ϣ������ṹ��gps_message @ublox_NMEA0183.h
}
//end function

/* ָ����ʼ��һ���ṹ��gps_message */
void init_gps_message(gps_message* gpsMsg)
{
	if(0!=gpsMsg)
	{
		gpsMsg->data_val = false;
		gpsMsg->fix_mode = '0';
		gpsMsg->lat_direc = 'N';
		gpsMsg->scan_satellite_count = 0;
		gpsMsg->gps_per = 0;
		gpsMsg->long_direc = 'N';
		gpsMsg->use_satellite_count =0;
		gpsMsg->mode = 0;
		gpsMsg->sttl_class = 0x00;
		gpsMsg->gp_scan_count = 0;       //ɨ��
		gpsMsg->gb_scan_count = 0;
		gpsMsg->gl_scan_count = 0;
		gpsMsg->val = 0;
		gpsMsg->groundSpeed = 0.0;
	#if IMPORT_FLOAT==1
		gpsMsg->altitude = 0.0;
		gpsMsg->latitude = 0.0;
		gpsMsg->longitude = 0.0;
		gpsMsg->altitude = 0.0;
		gpsMsg->hdop = 0.0;
		gpsMsg->vdop =0.0;
		gpsMsg->pdop = 0.0;
	#endif
	}
}
//end function

/* ����ַ�����NMEA0183�������������frame���淵�ؽ����ɹ�������֡��
		����$, ��'*'���� �ɹ�����һ֡���ݷ��� �� true*/
bool get_nmea_frame(char usartC, char* frame, unsigned int* cou)
{
	static unsigned char flag = NMEA_NO;	
	static unsigned int count = 0;				
	static char crc[2];
	static unsigned int crcSum = 0;
	
	if(0==frame) return false;
		
	switch(flag)
	{
		case NMEA_NO:
		{
			if('$'==usartC)    //ʶ�𵽿�ʼ��'$'    
				flag = NMEA_START;	
			break;
		}
		case NMEA_START:
		{
			if('*'==usartC && count>0)	//ʶ�������'*'
			{
				*(frame+count) = '*';			//����������������������
				++count;
				flag = NMEA_CRC1;					//���½�����һ��У���ַ�
				*cou = count;							//���ػ������еĸ���
			}else if(count<NMEA0183_FRAME_MAX_LEN)	//����֡���Ƿ񳬳�����
			{
				*(frame+count) = usartC;  
				++count;
				crcSum ^= usartC;		//У���Ϊ'$'��'*'�е������ַ����ṹ���ַ���ʾ������','			 
			}else
			{
				flag = NMEA_NO;
				count = 0;
				crc[0] = crc[1] = 0;
				crcSum = 0;
				*cou = 0;
			}
			break;
		}
		case NMEA_CRC1:	//��һ��У��λ
		{
			crc[1] = usartC;
			flag = NMEA_CRC2;
			break;
		}
		case NMEA_CRC2:	//�ڶ���У��λ
		{
			crc[0] = usartC;
			flag = NMEA_END;
			break;
		}
		default: break;
	}
	if(NMEA_END==flag)  //У��λ��ȡ��ɣ�Ҳ�����������ݶ��������
	{
		flag = NMEA_NO;
		count = 0;
		if(crcSum==(hex_to_uint(crc[0])+hex_to_uint((crc[1]))*16)) //�жϼ����
		{
			crc[0] = crc[1] = 0;
			crcSum = 0;
			return true;
		}
	}		
	return false;
}
//end function


/* ����ͨ�gget_nmea_frame������ȡ����ȷ֡����  */
void nema_message_parse(char* frame, gps_message* pos, unsigned int len)
{
	char msg[6];
	if(0==frame) return;
	
	pos->val = 0;
	copy_string(msg,frame,2);
	
	switch(msg[1])   //��ȡ��ϵ
	{
		case 'P':
		{
			pos->sttl_class = pos->sttl_class | 0x01;
			break;
		}
		case 'L':
		{
			pos->sttl_class = pos->sttl_class | 0x02;
			break;
		}
		case 'B':
		case 'D':
		{
			pos->sttl_class = pos->sttl_class | 0x04;
			break;
		}
		default: break;
	}
	
	copy_string(msg,frame,5); //��ȡID(GPGSA���ַ�)
	
	if(!strcmp(msg,GNGSA)|| !strcmp(msg,GPGSA) || !strcmp(msg,GLGSA) || !strcmp(msg,GBGSA) )
	{
		gsa_parse(frame, pos, len); //GSA
		
	}else if(!strcmp(msg,GNGGA) || !strcmp(msg,GPGGA) || !strcmp(msg,GLGGA) || !strcmp(msg,GBGGA) )	
	{
		gga_parse(frame, pos, len); //GGA
		
	}else if(!strcmp(msg,GPGSV) || !strcmp(msg,GNGSV) || !strcmp(msg,GLGSV) || !strcmp(msg,GBGSV ))
	{
//		gsv_parse(frame, pos, len); //GSV,���Է������� 
		
	}else if(!strcmp(msg,GPRMC) || !strcmp(msg,GNRMC) || !strcmp(msg,GLRMC) || !strcmp(msg,GBRMC))	
	{
		rmc_parse(frame, pos, len); //RMC
		
	}else if(!strcmp(msg,GPVTG) || !strcmp(msg,GNVTG) || !strcmp(msg,GLVTG) || !strcmp(msg,GBVTG) )	
	{
		vtg_parse(frame, pos, len);  //VTG
		
	}else if(!strcmp(msg,GPGLL) || !strcmp(msg,GNGLL) || !strcmp(msg,GLGLL) || !strcmp(msg,GBGLL))	
	{
		gll_parse(frame, pos, len); //GLL
		
	}else
	{
		
	}	
	
	pos->scan_satellite_count = pos->gb_scan_count+pos->gp_scan_count+pos->gl_scan_count;
	pos->val = 1;
}
//end function


/* ����RMC �ƽ鶨λ��Ϣ UTCʱ�䣬��γ�ȣ��ٶȣ���λ����Ϣ*/
void rmc_parse(char* frame, gps_message* pos, unsigned int len)
{
	char* fr;
	unsigned char count = 0;
	unsigned int index = 0;
	char buff[15];
	unsigned int t = 0;
	
	if(0==frame&&0==pos) return;
	
	fr = frame+6;  //����ID�ַ�����ǰ���',' 
	while('*'!=*fr&&t<5000) //ʶ�������
	{
		++t;
		if(','!=*fr) //��������Ĳ���','�����ַ������ڻ�������
		{
			buff[index] = *fr;
			++index;
		}else if(','==*fr) //֪������','���ٶԻ������е����ݽ��д���
		{

			buff[index] = '\0'; //�ַ���������־�������������
			++count; 
			if(index>0) //�������е����ݶ�����Ч
			{
				if(1==count)  //�ֶ�1��utc-time
				{
					set_gps_utc_time_str(buff); //utc time hhmmss.sss

				}else if(2==count)   //�ֶ�2����λ״̬
				{
					if(buff[0]=='A')
					{
						pos->data_val = true;				

					}else{
						pos->data_val = false;

					}
				}else if(3==count) //�ֶ�3 γ��
				{
					set_gps_latitude_str(buff);
				#if IMPORT_FLOAT==1					
					pos->latitude = string_to_float(buff, index); //stm32f103��������̫�������ƽ�ʹ��
				#endif
				}else if(4==count) //�ֶ�4 γ�����
				{
					pos->lat_direc = buff[0];//N or S

				}else if(5==count) //�ֶ�5 ����
				{
					set_gps_longitude_str(buff);
				#if IMPORT_FLOAT==1		
					pos->longitude = string_to_float(buff, index);
				#endif	

				}else if(6==count) //�ֶ�6 �������
				{
					pos->long_direc = buff[0];// E or W

				}else if(7==count) //�ֶ�7 �ٶ� ��knots
				{

				}else if(8==count) //�ֶ�8 ��λ�� ��
				{

				}else if(9==count) //�ֶ�9 utc-����
				{
					set_gps_utc_date_str(buff);

				}else if(10==count) //�ֶ�10 ��ƫ��
				{

				}else if(11==count) //�ֶ�11 ��ƫ�Ƿ���  E-��  W-��
				{

				}
		  }
			index = 0;	//��ȡһ���ֶν���������������־��0	
		}
		++fr; //��ȡ�¸�������������
	}
	if(count>0)//�ֶ�12 ģʽ A-�Զ�  D-���  E-����  N-������Ч
	{
		
	}
}
//end function

/* ����GSV  �ɼ�������Ϣ ��ǰ�ɼ���������PRN���룬�������ǣ����Ƿ�λ�ǣ������
*  ������Ϣ�����ڶ���Ļ�������,����12��Ԫ�ص�gps_gsv���黺���С�
*/
void gsv_parse(char* frame, gps_message* pos, unsigned int len)
{
	if(0!=frame&&0!=pos)
	{
		char* fr;
		unsigned char count = 0;
		unsigned int index = 0;
		char buff[15];
		unsigned int t = 0;
		unsigned int j = 0;
		unsigned int i = 0;
		unsigned char prn = 0;
		unsigned char DBH = 0;
		char msg[6];
		static unsigned char gsv_count = 0;
		static unsigned char gsv_current_count = 0;
			
		
		if(0==frame&&0==pos) return;
		
		fr = frame+6;  

		while('*'!=*fr&&t<5000)
		{
			++t;
			if(','!=*fr)
			{
				buff[index] = *fr;
				++index;
			}else if(','==*fr) 
			{
				buff[index] = '\0';
				++count; 
				if(index>0)
				{
					if(1==count)
					{   
						gsv_count = (buff[0]-0x30)*10+buff[1]-0x30;  //������ 
						reset_gps_gsv(gpgsv);
						reset_gps_gsv(glgsv);
						reset_gps_gsv(gbgsv);
					}else if(2==count)
					{
						gsv_current_count = (buff[0]-0x30)*10+buff[1]-0x30;   //�����
						if(gsv_current_count==gsv_count) 
						{
							gsv_count = 0;
							gsv_current_count = 0;
						}
					}else if(3==count)  //�ֶ�3  �ɼ����Ǹ���
					{
						unsigned char tmp = 0;
						tmp = (buff[0]-0x30)*10+buff[1]-0x30;  						
						copy_string(msg,frame,2);
						
						switch(msg[1])   //��ȡ��ϵ
						{
							case 'P':
							{
								pos->gp_scan_count = tmp;
								break;
							}
							case 'L':
							{
								pos->gl_scan_count = tmp;
								break;
							}
							case 'B':
							case 'D':
							{
								pos->gb_scan_count = tmp;
								break;
							}
							default: break;
						}
						while('*'!=*fr) 
						{
							for(i=0,j=0; i<13; ++i) //��һ�����ǵ�������Ϣȥ��',',��˳��洢����������
							{
								if(','!=*fr) 
								{
									buff[j] = *fr;
									++j;
								}
								++fr;
							}
							prn = (buff[0]-0x30)*10+buff[1]-0x30;  //��ȡPRN����
							DBH = (buff[7]-0x30)*10+buff[8]-0x30;  //��ȡ�����
							switch(*(frame+1))
							{
								case 'P': //GP
								{								
									update_stll_msg(&gpgsv[0], 12, prn, DBH); //��������Ϣ���浽�����ȫ��gps_msg������									
									break;
								}
								case 'B': //GB
								{
									update_stll_msg(&gbgsv[0], 12, prn, DBH);
									break;
								}
								case 'L': //GL
								{
									update_stll_msg(&glgsv[0], 12, prn, DBH);
									break;
								}
								default: break;							
							}
						}
						break;  //��ȡ���
					}
				}
				index = 0;		
			}
			++fr;
		}
	}
}
//end function

//��λgps_gsvȫ�ֱ���
void reset_gps_gsv(gps_gsv* gsv)
{
	unsigned char i = 0;
	if(0==gsv) return;
	for( ; i<12; ++i)
	{
		gsv->enable = 0;
	}
}
//end function

//������Ч��־
unsigned char get_gps_msg_val(void)
{
	return gpsMsg.val;
}

//��ȡ�ɼ�����ƽ�������
unsigned char get_db_average(void)
{
	unsigned char count = 0;
	unsigned int sum = 0;
	unsigned char avr = 0;
	unsigned char i = 0;
	
	for(i=0; i<12; ++i)
	{
		if(gpgsv[i].enable)
		{
			sum += gpgsv[i].db;
			++count;
		}
	}
	
	for(i=0; i<12; ++i)
	{
		if(gbgsv[i].enable)
		{
			sum += gbgsv[i].db;
			++count;
		}
	}
	
	for(i=0; i<12; ++i)
	{
		if(glgsv[i].enable)
		{
			sum += glgsv[i].db;
			++count;
		}
	}
	avr = sum/count;
	return avr;
}

/* ����������Ϣ */
static void update_stll_msg(gps_gsv* gsvMsg, unsigned char len, unsigned char prn, unsigned char dbh)  
{
	if(0!=gsvMsg&&0!=len) 
	{
		while(len--)
		{
			if(gsvMsg->prn_number==prn) //���������Ѿ���������Ϣ
			{
				gsvMsg->db = dbh;
				gsvMsg->enable = 1;
				return;
			}
					
			if(gsvMsg->prn_number!=prn&&0==gsvMsg->enable) //��������δ�б�����Ϣ���Ҵ��ڿɴ�����Ϣ�Ļ�����
			{
				gsvMsg->enable = 1; //���������Ѿ��������ݱ�־
				gsvMsg->prn_number = prn;
				gsvMsg->db = dbh;
				return;
			}
			++gsvMsg;
		}
	}		
}
//end function

/*����GGA ��λ��Ϣ UTC-time,��γ�ȣ�GPS״̬������ʹ�õ���������ˮƽ���ȣ����θ߶�*/
void gga_parse(char* frame, gps_message* pos, unsigned int len)
{
	char* fr;
	unsigned char count = 0;
	unsigned int index = 0;
	char buff[15];
	unsigned int t = 0;
	char msg[6];
	
	if(0==frame&&0==pos) return;
	
	fr = frame+6;  

	while('*'!=*fr&&t<5000)
	{
		++t;
		if(','!=*fr)
		{
			buff[index] = *fr;
			++index;
		}else if(','==*fr) 
		{
			buff[index] = '\0';
			++count; 
			if(index>0)
			{
				if(1==count) //�ֶ�1 utc time
				{
					set_gps_utc_time_str(buff);	
				}else if(2==count) //�ֶ�2 γ��
				{
					set_gps_latitude_str(buff);
				#if IMPORT_FLOAT==1		
					pos->latitude = string_to_float(buff, 9); 
				#endif		
				}else if(3==count)
				{
					pos->lat_direc = buff[0];//N or S
				}else if(4==count) //�ֶ�4 ����
				{
					set_gps_longitude_str(buff);
				#if IMPORT_FLOAT==1		
					pos->longitude = string_to_float(buff, index);
				#endif
				}else if(5==count) 
				{
					pos->long_direc = buff[0];// E or W

				}else if(6==count) //�ֶ�6 GPS״̬ 0-������ 1-���㶨λ 2-��ֶ�λ 
													//	3-��ЧPPS 4-ʵʱ��ֶ�λ  5-RTK FLOAT 6-���ڹ���
				{
					pos->gps_per = buff[0];  

				}else if(7==count) //�ֶ�7 ����ʹ�õ�������
				{
					unsigned char tmp = 0;
					tmp = (buff[0]-0x30)*10+buff[1]-0x30;  						
					copy_string(msg,frame,2);
					
					switch(msg[1])   //��ȡ��ϵ
					{
						case 'P':
						{
							//pos->use_satellite_count = (buff[0]-0x30)*10+buff[1]-0x30;
							break;
						}
						case 'L':
						{
							break;
						}
						case 'B':
						case 'D':
						{
							break;
						}
						case 'N':
						{
							pos->use_satellite_count = (buff[0]-'0')*10+buff[1]-'0';  
							break;
						}
						default: break;
					}												
						
				}else if(8==count) //�ֶ�8 ˮƽ����
				{
					set_gps_hdop_str(buff, index+1);
				#if IMPORT_FLOAT==1		
					pos->hdop = string_to_float(buff, index);
				#endif
				}else if(9==count) //�ֶ�9 ���θ߶�
				{
					set_gps_altitude_str(buff, index); //�߶��ַ����ǿɱ䳤�ȵ�
				#if IMPORT_FLOAT==1		
					pos->altitude = string_to_float(buff,index);
				#endif

				}else if(10==count) //�ֶ�10  ���ε�λ
				{
					
				}else if(11==count)
				{
				}
		  }
			index = 0;		
		}
		++fr;
	}
	if(count>0) //�ֶ�12
	{
	}
}
//end function

/* ����GSA ��ǰ������Ϣ  ��λģʽ����λ���ͣ��ֶ�3-14Ϊ12��������Ϣ��PRN���룬λ�á�ˮƽ����ֱ����  */
void gsa_parse(char* frame, gps_message* pos, unsigned int len)
{
	char* fr;
	unsigned char count = 0;
	unsigned int index = 0;
	char buff[15];
	unsigned int t = 0;
	
	if(0==frame&&0==pos) return;
	
	fr = frame+6;  

	while('*'!=*fr&&t<5000)
	{
		++t;
		if(','!=*fr)
		{
			buff[index] = *fr;
			++index;
		}else if(','==*fr) 
		{

			buff[index] = '\0';
			++count; 
			if(index>0)
			{
				if(1==count) //�ֶ�1 ��λģʽ A-�Զ�  M-�ֶ�
				{
					pos->mode = buff[0];  
				}else if(2==count) //�ֶ�2 ��λ����  1-δ��λ  2-2D��λ  3-3D��λ
				{
					pos->fix_mode = buff[0];  
				}else if(3==count)
				{

				}else if(4==count)
				{

				}else if(5==count)
				{

				}else if(6==count)
				{

				}else if(7==count)
				{

				}else if(8==count)
				{

				}else if(9==count)
				{

				}else if(10==count)
				{
					
				}else if(11==count)
				{
					
				}else if(12==count)
				{
				}else if(13==count)
				{
				}else if(14==count)
				{
				}else if(15==count) //�ֶ�15 λ�þ��� 0.5-99.9
				{
					set_gps_pdop_str(buff, index+1);
				#if IMPORT_FLOAT==1		
					pos->pdop = string_to_float(buff, index);	
				#endif
				}else if(16==count) //�ֶ�16 ˮƽ����
				{
					set_gps_hdop_str(buff, index+1);
				#if IMPORT_FLOAT==1		
					pos->hdop = string_to_float(buff, index);
				#endif
				}
		  }
			index = 0;		
		}
		++fr;
	}
	if(count>0) //�ֶ�17 ��ֱ����
	{
		set_gps_vdop_str(buff, index+1);
	#if IMPORT_FLOAT==1		
		pos->vdop = string_to_float(buff, index);
	#endif
	}
}
//end function

/* ����GLL ����λ��Ϣ ��γ�ȣ�UTC-time����λ״̬ */
void gll_parse(char* frame, gps_message* pos, unsigned int len)
{
	char* fr;
	unsigned char count = 0;
	unsigned int index = 0;
	char buff[15];
	unsigned int t = 0;
	
	if(0==frame&&0==pos) return;
	
	fr = frame+6;  

	while('*'!=*fr&&t<5000)
	{
		if(','!=*fr)
		{
			buff[index] = *fr;
			++index;
		}else if(','==*fr) 
		{
			buff[index] = '\0';
			++count; 
			if(index>0)
			{
				if(1==count) //�ֶ�1 γ��
				{
					set_gps_latitude_str(buff);
				#if IMPORT_FLOAT==1		
					pos->latitude = string_to_float(buff, 9); 
				#endif
				}else if(2==count)
				{
					pos->lat_direc = buff[0];//N or S
				}else if(3==count) //�ֶ�3 ����
				{
					set_gps_longitude_str(buff);
				#if IMPORT_FLOAT==1		
					pos->longitude = string_to_float(buff, index);
				#endif
				}else if(4==count)
				{
					pos->long_direc = buff[0];// E or W
				}else if(5==count) //�ֶ�5 utc time
				{
					set_gps_utc_time_str(buff);	
				}
		  }
			index = 0;		
		}
		++fr;
	}
	
	if(count>0) //�ֶ�6 ��λ״̬ A-��Ч  V-δ��λ
	{
		if(buff[0]=='A')
		{
			pos->data_val = true;
		}else{
			pos->data_val = false;
		}
	}
}
//end function

/* ����VTG �����ٶ���Ϣ  �˶��Ƕȣ�����ϵ��ˮƽ�˶��ٶ�*/
void vtg_parse(char* frame, gps_message* pos, unsigned int len)
{
		char* fr;
	unsigned char count = 0;
	unsigned int index = 0;
	char buff[15];
	unsigned int t = 0;
	
	if(0==frame&&0==pos) return;
	
	fr = frame+6;  
	while('*'!=*fr&&t<5000)
	{
		if(','!=*fr)
		{
			buff[index] = *fr;
			++index;
		}else if(','==*fr) 
		{
			buff[index] = '\0';
			++count; 
			if(index>0)
			{
				if(1==count) //�ֶ�1 �˶��Ƕ� 000-359
				{
					
				}else if(2==count) //�ֶ�2 T �汱����ϵ
				{
					
				}else if(3==count) //�ֶ�3 �˶��Ƕ�
				{
					
				}else if(4==count) //�ֶ�4 �ű�����ϵ
				{
					
				}else if(5==count) //�ֶ�5 ˮƽ�˶��ٶ�0.00
				{
					
				}else if(6==count) //�ֶ�6 �ٶȵ�λ N=�� Knots
				{
					
				}else if(7==count) //�ֶ�7 ˮƽ�˶��ٶ�0.00
				{
					unsigned char t = 0;
					char y = -1;
					unsigned char dotflag = 0;
					float xiaoshu = 0.0;
					pos->groundSpeed = 0;
					for( t=0; '\0'!=buff[t]; ++t)
					{
						if('.'==buff[t])
						{
							dotflag = 1;
							continue;
						}
						if(dotflag)
						{
							xiaoshu += (buff[t]-'0')*pow(10,y);
							--y;
							if(y<-3)
								break;
							continue;
						}
						pos->groundSpeed += (buff[t]-'0')*pow(10,t);
					}
					pos->groundSpeed += xiaoshu;					
				}
		  }
			index = 0;		
		}
		++fr;
	}
	if(count>0) //�ֶ�8 �˶��ٶȵ�λ K=����/ʱ
	{
	}
}
//end function

/* GPS��Ϣ��ȡ���� */

//���Ȼ�ȡ  �ַ����ܳ�12 ��Ч����λΪ11
char* get_gps_longitude_str(void) 
{
	static char longitudeStr[] ={"dddmm.mmmmm"};
	
	return &longitudeStr[0];	
}
void set_gps_longitude_str(char* str)   
{
	init_char_buff(get_gps_longitude_str(), 12, '!');
	copy_string(get_gps_longitude_str(),str,11);
}
//���� end

//γ�Ȼ�ȡ  �ַ����ܳ�11 ��Ч����λΪ10
char* get_gps_latitude_str(void)
{
	static char latitude[]={"ddmm.mmmmm"};
	
	return &latitude[0];
}
void set_gps_latitude_str(char* str)
{
	init_char_buff(get_gps_latitude_str(), 11, '!');
	copy_string(get_gps_latitude_str(),str,10);
}
//γ�� end

//�߶Ȼ�ȡ  �ַ����ܳ�8 ��Ч����λΪ7
char* get_gps_altitude_str(void)
{
	static char altitude[]={7,'9','9','9','9','9','.','9','\0'};  //��һ���ַ�Ϊ����
	static unsigned char len = 0;
	
	return &altitude[0];
}
void set_gps_altitude_str(char* str, unsigned int len)
{
	init_char_buff(get_gps_altitude_str(), 9, '!');
	*(get_gps_altitude_str()) = len;
	copy_string(get_gps_altitude_str()+1,str,len);
}
//�߶� end

//UTCʱ�� ��������Ч����6  ʱ�䳤��10
char* get_gps_utc_date_str(void)
{
	static char date[]={"DDMMYY"};
	//static char date[]={"211018"};
	
	return &date[0];
}
char* get_gps_utc_time_str(void)
{
	static char time[]={"hhmmss.sss"};
	
	return &time[0];
}
void set_gps_utc_date_str(char* str)
{
	init_char_buff(get_gps_utc_date_str(), 6, '!');
	copy_string(get_gps_utc_date_str(),str,6);
}
void set_gps_utc_time_str(char* str)
{
	init_char_buff(get_gps_utc_time_str(), 10, '!');
	copy_string(get_gps_utc_time_str(),str,10);
}

//char* get_utc(char* buff, unsigned int len)
//{
//	char* pc;
//	char* date;
//	char* time;
//	unsigned int i;
//	if(0==buff&&0==len) return 0;
//	
//	date = get_gps_utc_date_str() + 5;
//	time = get_gps_utc_time_str();	
//	if(0==date&&0==time) return 0;
//	
//	pc = buff;
//	
//	
//}
//���ڡ�ʱ�� end

//λ�þ���  ��Ч����3
char* get_gps_pdop_str(void)
{
	static char pdop[] = {"99.9"};
	
	return &pdop[0];
}
void set_gps_pdop_str(char* str, unsigned char len)
{
	init_char_buff(get_gps_pdop_str(), 5, '-');
	copy_string(get_gps_pdop_str(),str,4);   //�ַ������Ƶĳ������ַ����������������õĳ��Ⱦ��� 
}
//λ�� end

//ˮƽ����  ��Ч����3
void set_gps_hdop_str(char* str, unsigned char len)
{
	init_char_buff(get_gps_hdop_str(), 4, '-');
	copy_string(get_gps_hdop_str(),str,5);   //�ַ������Ƶĳ������ַ����������������õĳ��Ⱦ���
}
char* get_gps_hdop_str(void)
{
	static char hdop[] = {"99.9"};

	return &hdop[0];
}
//ˮƽ���� end

//��ֱ���� ��Ч����3
char* get_gps_vdop_str(void)
{
	static char vdop[] = {"99.9"};
	
	return &vdop[0];
}
void set_gps_vdop_str(char* str,unsigned char len)
{
	init_char_buff(get_gps_vdop_str(), 5, '-');
	copy_string(get_gps_vdop_str(),str,4);   //�ַ������Ƶĳ������ַ����������������õĳ��Ⱦ���
}

/* �ж�GPS�Ƿ��Ѿ���λ */
bool gps_valid(gps_message* gps)
{
	if(gps->data_val && gps->fix_mode>'0')
		return true;
	return false;
}
//��ֱ����   end

unsigned char char_to_int(char c)
{
	if(c>='0' && c<='9')
		return c-'0';
	else return 0;
}

/* ʱ��ת�� */
void local_time(char* ddmmyy, char* hhmmss, unsigned char localTimeArea)
{
	unsigned char hour = 0;
	unsigned char dayadd = 0;
	unsigned char yearflag = 0; 
	unsigned char monthflag = 0;
	unsigned char day = 0;
	unsigned char month = 0;
	unsigned int year = 0;
	
	if(0==hhmmss || 0==ddmmyy) return;
	
	hour = 	10*char_to_int(hhmmss[0]) + char_to_int(hhmmss[1]) + localTimeArea;
	
	if(hour>=24)
		dayadd = 1;
	
	hour %= 24;	
	year = 2000 + 10*char_to_int(ddmmyy[4]) + char_to_int(ddmmyy[5]);
	month = 10*char_to_int(ddmmyy[2]) + char_to_int(ddmmyy[3]);
	day = 10*char_to_int(ddmmyy[0]) + char_to_int(ddmmyy[1]);
	
	if( (0==year%4 && 0!=year%400) || 0==year%400 )
	{
		yearflag = 1;
	}		
		
	if(day>=28 && day<=31 && dayadd)
	{
		switch(month)
		{
			/* 31���·� */ 
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				if(31==day)
				{
					monthflag = 1;
				}					 
				break;
			
			/* ����29�죬����28��  */
			case 2:
				if(29==day || (28==day && !yearflag))
				{
					monthflag = 1;
				}
				break;
				
			/* 30���·� */ 
			default:
				if(30==day)
				{
					monthflag = 1;
				}
				break; 
		}
		if(monthflag)
		{
			month += 1;
			if(month>12) 
			{
				year+=1;
				month = 1;
			}
			day = 1;
		}else
		{
			day += 1;
		}
	}
	
	hhmmss[0] = hour/10 + '0';
	hhmmss[1] = hour%10 + '0';
	ddmmyy[0] = day/10 + '0';
	ddmmyy[1] = day%10 + '0';
	ddmmyy[2] = month/10 + '0';
	ddmmyy[3] = month%10 + '0';
	ddmmyy[4] = (year%100)/10 + '0';
	ddmmyy[5] = year%10 + '0';
} 
/* END  */
