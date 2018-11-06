
#ifndef GITWITS_PROTOCOL_H
#define GITWITS_PROTOCOL_H

#include "gizwits_type.h"
#include "AwesomeDataProcess.h"

gizwits_pack* get_gizwits(void);
unsigned char gizwits_init(void);
unsigned char gizwits_pack_char(gizwits_pack* pgp, unsigned char command, const char* data, unsigned short len, unsigned short flag);
GIZWITS_RESULT gizwits_parse_char(unsigned char c, gizwits_pack* pg_pack, gizwits_result* pg_result);
unsigned char gizwits_data_process(gizwits_pack* pg_pack, gizwits_pack* pg_pack_send);

unsigned char gizwits_reply_pack(gizwits_pack* pg_pack, unsigned char command);

#endif


