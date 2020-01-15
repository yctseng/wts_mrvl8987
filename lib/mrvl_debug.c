#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <poll.h>
#include <ctype.h>
#include <time.h>

#include "wfa_portall.h"
#include "wfa_debug.h"
#include "wfa_ver.h"
#include "wfa_main.h"
#include "wfa_types.h"
#include "wfa_ca.h"
#include "wfa_tlv.h"
#include "wfa_sock.h"
#include "wfa_tg.h"
#include "wfa_cmds.h"
#include "wfa_rsp.h"
#include "wfa_utils.h"
#include "mrvl_macros.h"

void ENTER ( char const * caller_name  ) {
	char tstamp[100] = {0};
	if ( getenv("MV_DEBUG") != NULL ) {
		timestamp(tstamp);
		printf("[%s]\tEntering %s ... \n",tstamp,caller_name);
	}
}

void LEAVE ( char const * caller_name  ) {
	char tstamp[100] = {0};
	if ( getenv("MV_DEBUG") != NULL ) {
		timestamp(tstamp);	
		printf("[%s]\tLeaving %s ... \n",tstamp,caller_name);
	}
}