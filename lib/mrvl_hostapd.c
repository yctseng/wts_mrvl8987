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
#include "mrvl_gvars.h"

int hostapd_pmf_capabilities( char *pmf_type ) {
	ENTER( __func__ );	
	if ( !strcasecmp(pmf_type,"required") ) {
		sprintf(gCmdStr, "./%s set ieee80211w 2",mrvl_hostapd_info->hostapd_cli_bin);
		system_with_log(gCmdStr);	
	} else if ( !strcasecmp(pmf_type,"optional") ) {
		sprintf(gCmdStr, "./%s set ieee80211w 1",mrvl_hostapd_info->hostapd_cli_bin);
		system_with_log(gCmdStr);
	} else if ( !strcasecmp(pmf_type,"disabled") ) {
		/* TODO */
	} else {
		/** Do nothing */
	}
	LEAVE( __func__ );
	return TRUE;
}

int generate_hostapd_conf () {
     ENTER( __func__ );
     FILE *fp,*fp_ref;
     int ret; 
	 int ch;
     ret = access(HOSTAPD_CONF, F_OK);	
     if (ret == -1) {  				
	     fp = fopen(HOSTAPD_CONF,"w");
	     if ( fp == NULL ) {
			LEAVE( __func__ );
			return FALSE;
	     }
		 ret = access("hostapd_init.conf", F_OK);
		 if (ret == -1 ) {	 
			 fprintf(fp,"ctrl_interface=%s\n",HOSTAPD_CTRL_PATH);
			 fprintf(fp,"interface=%s\n",mrvl_dut_info->mmh_interface);
			 fprintf(fp,"driver=%s\n",mrvl_hostapd_info->hostapd_ext);
			 fprintf(fp,"ieee80211n=1\n");
			 fprintf(fp,"dtim_period=1\n");
			 fprintf(fp,"wmm_enabled=1\n");
			 fclose(fp);
		 } else {
			 printf("INITIALIZING HOSTAPD CONF FROM 'hostapd_init.conf' file...\n");
			 fp_ref = fopen("hostapd_init.conf","r");
			 while (1) {
				ch = fgetc(fp_ref);
				if (ch == EOF) {
				  putc('\n', fp);	
				  break;
				} else {
				  putc(ch, fp);
				}
			 }
			 fclose(fp);
			 fclose(fp_ref);	 
		 }		 
     } 
     LEAVE( __func__ );   
     return TRUE;
}
