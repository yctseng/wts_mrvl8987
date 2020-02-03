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
#include "mrvl_common.h"
#include "mrvl_macros.h"
#include "mrvl_debug.h"
#include "mrvl_gvars.h"

typedef struct _chan_to_freq_t {
    /** Channel */
    int channel;
    /** Frequency */
    int freq;
} chan_to_freq_t;

const chan_to_freq_t chan_to_freq [] = {
    {1,         2412},
    {2,         2417},
    {3,         2422},
    {4,         2427},
    {5,         2432},
    {6,         2437},
    {7,         2442},
    {8,         2447},
    {9,         2452},
    {10,        2457},
    {11,        2462},
    {12,        2467},
    {13,        2472},
    {14,        2484},
	{36,		5180},
	{40,		5200},

};

/**
 *  @brief Get frequency for channel in given band 
 *
 *  @param channel      channel
 *
 *  @return             freq
 */
int channel_to_frequency(int channel)
{
    int i = 0;
    for (i=0; i < sizeof(chan_to_freq)/sizeof(chan_to_freq_t); i++) {
        if (channel == chan_to_freq[i].channel) {
            return chan_to_freq[i].freq;
        }
    }
    return 0;
}

int enable_mlan_reassoc_ctrl ( char *ifname ){
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s %s reassoctrl 1", APP_BIN_LOC,MLANUTL,ifname);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int check_wlan_connectivity (char *ifname) {	
	FILE *tmpfile;
       char astring[256];
       char filename[256];
    
	ENTER( __func__ );
	if (!MWIFIEX) {
		#ifdef MV_PLATFORM_ANDROID 
		sprintf(gCmdStr, "iwconfig %s | busybox grep ESSID: | busybox cut -f2- -d= > /tmp/.isConnected", ifname);
		#else
		sprintf(gCmdStr, "cat %s | grep media_state | cut -f2 -d= > /tmp/.isConnected", mrvl_dut_info->mwlan_info_path);
		#endif
		system_with_log(gCmdStr);
	
	} else {
		sprintf(gCmdStr, "iw dev %s link > /tmp/.isConnected", ifname);
		system_with_log(gCmdStr);	
	}	
    strcpy(filename,"/tmp/.isConnected");	
	tmpfile = fopen(filename, "r+");
    if(tmpfile == NULL)
    {	
		DPRINT_ERR(WFA_ERR, "file open failed\n%d", __LINE__);
		LEAVE( __func__ );
        return FALSE;
    }
	fgets(astring, 256, tmpfile);	
	LEAVE( __func__ );
	if (!MWIFIEX) {
		if(!strncmp(astring, "\"Disconnected\"", 14))
			return FALSE;
	} else {
		if(!strncmp(astring, "Not connected.", 14))
			return FALSE;	
	}	
	return TRUE;
}

int enable_mlan_association( char *ifname, char *ssid) {
	ENTER( __func__ );
	if (!MWIFIEX) {
		sprintf(gCmdStr,"ifconfig %s down",ifname);
		system_with_log(gCmdStr);	
		sprintf(gCmdStr,"iwconfig %s essid %s",ifname,ssid);
		system_with_log(gCmdStr);
		sprintf(gCmdStr,"ifconfig %s up",ifname);
		system_with_log(gCmdStr);
	} else {
		sprintf(gCmdStr, "ifconfig %s up", ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iw dev %s scan", ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iw dev %s connect %s", ifname,ssid );
		system_with_log(gCmdStr);
	}
	LEAVE( __func__ );
	return TRUE;
}

int fetch_mlan_passphrase ( char *ifname, char *passphrase ) {
    char buff[50],ch;
    char *str;
    int i;
    FILE *tmpfd;

	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s sys_cfg_wpa_passphrase > /tmp/pass.txt",APP_BIN_LOC,UAPUTL,ifname);
	system_with_log(gCmdStr);
	tmpfd = fopen("/tmp/pass.txt", "r");
	ch = fgetc(tmpfd);
	while( ch != EOF )
	{
		if( ch == '=' )
		{
			ch = fgetc(tmpfd);
			while( (ch=fgetc(tmpfd)) == ' ' );
			i=0;
			while(ch != '\n'  )
			{
				buff[i]=ch;
				ch = fgetc(tmpfd);
				i++;
			}
			buff[i]='\0';
		}
		ch=fgetc(tmpfd);
	}			
	strcpy(passphrase,buff);
	fclose(tmpfd);
	
	LEAVE( __func__ );
	return TRUE;
}

int fetch_mlan_ssid ( char *ifname, char *ssid ) {
    char buff[50],ch;
    char *str;
    int i;
    FILE *tmpfd;
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s sys_cfg_ssid > /tmp/ssid.txt",APP_BIN_LOC,UAPUTL,ifname);
	system_with_log(gCmdStr);
	tmpfd = fopen("/tmp/ssid.txt", "r");
	ch = fgetc(tmpfd);
	while( ch != EOF )
	{
		if( ch == '=' )
		{
			ch = fgetc(tmpfd);
			while( (ch=fgetc(tmpfd)) == ' ' );
			i=0;
			while(ch != '\n'  )
			{
				buff[i]=ch;
				ch = fgetc(tmpfd);
				i++;
			}
			buff[i]='\0';
		}
		ch=fgetc(tmpfd);
	}			
	strcpy(ssid,buff);
	fclose(tmpfd);
	LEAVE( __func__ );
	return TRUE;
}

int fetch_sta_rssi(char * ifname, char * rssi) {
    	  
		char *str;
		int i,cnt,ret=0;
		FILE *tmpfd;
		char string[128];
	
		ENTER( __func__ ); 
		sprintf(gCmdStr, "iwconfig %s > /tmp/rssi.txt  ",ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "cp /tmp/rssi.txt /tmp/getrssi.txt; cat /tmp/rssi.txt | grep \"Signal l\" | cut -d= -f3 | cut -d\" \" -f1 > /tmp/getrssi.txt"); 
		system_with_log(gCmdStr);

		
		tmpfd = fopen("/tmp/getrssi.txt", "r+");
			if(tmpfd == NULL) {
				LEAVE( __func__ );
				return FALSE;
			}
			if(fgets(string, 128, tmpfd) == NULL) {
				LEAVE( __func__ );
				return FALSE;
			}
			
			if ( string[strlen(string) == '\n' ] )
				string[strlen(string) -1] = '\0';	
			
			strcpy(rssi, string);
			fclose(tmpfd);
			LEAVE( __func__ );
			return TRUE;
		}


/*DD : Implementation for MBSS_SCAN -START

int fetch_mbss_scan_res (char * ifname, char * mbss_scan_res) {
    	  
		char *str;
		int i,cnt,ret=0;
		FILE *tmpfd;
		char string[512];
	
		ENTER( __func__ ); 
		sprintf(gCmdStr, "sh -x mbss-scan.sh %s ",ifname);
		system_with_log(gCmdStr);
				
		tmpfd = fopen("/tmp/MBSS_SCAN_RES.txt", "r+");
			if(tmpfd == NULL) {
				LEAVE( __func__ );
				return FALSE;
			}
			if(fgets(string, 510, tmpfd) == NULL) {
				LEAVE( __func__ );
				return FALSE;
			}
			
			if ( string[strlen(string) == '\n' ] )
				string[strlen(string) -1] = '\0';	
			
			strcpy(mbss_scan_res, string);
			fclose(tmpfd);
			LEAVE( __func__ );
			return TRUE;
		}


//DD : Implementation for MBSS_SCAN -END 
*/   

int fetch_mlan_ip_connection_info (caStaGetIpConfigResp_t *ifinfo,char * ifname, int ipType) {
	int slen, ret, i = 0;
    FILE *tmpfd;
    char string[256];
    char *str;
	
	ENTER( __func__ );
    ret = access("getipconfig.sh", F_OK);
	if ( ret == FALSE ) 
		return FALSE;
	
	strcpy(ifinfo->dns[0], "0");
    strcpy(ifinfo->dns[1], "0");
	
#ifdef MV_PLATFORM_ANDROID
	sprintf(gCmdStr, "busybox ash getipconfig.sh /tmp/ipconfig.txt %s",ifname);
#else
	sprintf(gCmdStr, "sh getipconfig.sh /tmp/ipconfig.txt %s",ifname);
#endif
	system_with_log(gCmdStr);
	
    tmpfd = fopen("/tmp/ipconfig.txt", "r+");
    if(tmpfd == NULL)
    {
		DPRINT_ERR(WFA_ERR, "File open failed\n%d", __LINE__);
		return FALSE;
    }

	
	for(;;) {
	    if(fgets(string, 256, tmpfd) == NULL)
	       break; 

	    /* check dhcp enabled */
	    if(strncmp(string, "dhcpcli", 7) ==0 && ipType == 1)
	    {
	        str = strtok(string, "=");
	        str = strtok(NULL, "=");
	        if(str != NULL)
	           ifinfo->isDhcp = 1;
	        else
	           ifinfo->isDhcp = 0;
	    }

	    /* find out the ip address */
		if(strncmp(string, "ipaddr", 6) == 0 && ipType == 1)
	    {
	        str = strtok(string, "=");
	        str = strtok(NULL, " ");
	     
			if(str != NULL)
	        {
	           wSTRNCPY(ifinfo->ipaddr, str, 15);
	           ifinfo->ipaddr[15]='\0';         
	           i=0;
			   slen = strlen(ifinfo->ipaddr);	     	    
	           while (i < slen) {
					if ( ifinfo->ipaddr[i] == '\n' )
						ifinfo->ipaddr[i] = '\0';
					i++; 	
	           }					  
			}
	        else
	           wSTRNCPY(ifinfo->ipaddr, "none", 15);
	    }

	    /* check the mask */
	    if(strncmp(string, "mask", 4) == 0 && ipType == 1)
	    {
	        char ttstr[16];
	        char *ttp = ttstr;

	        str = strtok_r(string, "=", &ttp);
	        if(*ttp != '\0')
	        {
	           strcpy(ifinfo->mask, ttp);
	           slen = strlen(ifinfo->mask);
	           ifinfo->mask[slen-1] = '\0';
	        }
	        else
	           strcpy(ifinfo->mask, "none");
	    }

	    /* find out the dns server ip address */
	    if(strncmp(string, "nameserv", 8) == 0 && ipType == 1)
	    {
	        char ttstr[16];
	        char *ttp = ttstr;
	        
	        str = strtok_r(string, " ", &ttp);
	        if(str != NULL && i < 2)
	        {
	           strcpy(ifinfo->dns[i], ttp);
	           slen = strlen(ifinfo->dns[i]);
	           ifinfo->dns[i][slen-1] = '\0';
	        }
	        else
	           strcpy(ifinfo->dns[i], "none");

	        i++;
	    }

		if(strncmp(string, "ipv6addr", 6) == 0 && ipType == 2)
	    {	
			str = strtok(string, "=");
			str = strtok(NULL, " ");
			if(str != NULL) 
			{
			   wSTRNCPY(ifinfo->ipv6addr, str, WFA_IP_V6_ADDR_STR_LEN);
			   ifinfo->ipv6addr[WFA_IP_V6_ADDR_STR_LEN - 1]='\0';
			   /* Force the kernel to send NS by re-assigning the IPv6 address*/
			   sprintf(gCmdStr, "%s ifconfig %s inet6 del %s\n",DUT_CMD_PREFIX,ifname, ifinfo->ipv6addr);
			   system_with_log(gCmdStr);
			   sprintf(gCmdStr, "%s ifconfig %s inet6 add %s\n",DUT_CMD_PREFIX,ifname, ifinfo->ipv6addr);
			   system_with_log(gCmdStr);
			}
			else {
			   wSTRNCPY(ifinfo->ipv6addr, "none", 15);
			}
		}
	}
	fclose(tmpfd);	
	LEAVE( __func__ );
	return TRUE;
}

int set_static_ip_config ( char *ifname, caStaSetIpConfig_t *ipconfig ) {
	ENTER( __func__ );
	if ( ipconfig->type == 1 ) { /** IPv4 */
	sprintf(gCmdStr, "%s ifconfig %s %s netmask %s > /dev/null 2>&1 ",DUT_CMD_PREFIX,ifname, ipconfig->ipaddr, ipconfig->mask);
	system_with_log(gCmdStr);	
#ifndef MV_PLATFORM_ANDROID
	   if(ipconfig->defGateway[0] != '\0')
	   {
		  sprintf(gCmdStr, "/sbin/route add default gw %s > /dev/null 2>&1", ipconfig->defGateway);
		  system_with_log(gCmdStr);
	   }
	   sprintf(gCmdStr, "echo nameserv %s > /etc/resolv.conf", ipconfig->pri_dns);
	   system_with_log(gCmdStr);
	   sprintf(gCmdStr, "echo nameserv %s >> /etc/resolv.conf", ipconfig->sec_dns);
	   system_with_log(gCmdStr);
#endif
	}
	if ( ipconfig->type == 2 ) { /** IPv6 */
#ifdef MV_PLATFORM_ANDROID
		strcat(ipconfig->ipv6addr,"/64");
		sprintf(gCmdStr, "%s ifconfig %s inet6 add %s > /dev/null 2>&1 ",DUT_CMD_PREFIX,ifname,ipconfig->ipv6addr);
		system_with_log(gCmdStr);
#else 
		sprintf(gCmdStr, "sysctl -w net.ipv6.conf.%s.autoconf=0",ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "sysctl -w net.ipv6.conf.%s.accept_ra=0",ifname);
		system_with_log(gCmdStr);
		strcat(ipconfig->ipv6addr,"/64");
		sprintf(gCmdStr, "ifconfig %s inet6 add %s > /dev/null 2>&1",ipconfig->intf, ipconfig->ipv6addr);
		system_with_log(gCmdStr);
#endif 
	} 
	LEAVE( __func__ );
	return TRUE;
}

int check_ping_connectivity ( char *ip, unsigned int timeout ) {
	char strout[64], *pcnt;
	FILE *tmpfile;
    int status; 
	
	ENTER( __func__ ); 
#ifdef MV_PLATFORM_ANDROID
	sprintf(gCmdStr, "/system/bin/ping -c 3 -W %u %s | busybox grep loss | busybox cut -f3 -d, 1>& /tmp/pingout.txt",ip,timeout);
#else
	sprintf(gCmdStr, "ping %s -c 3 -W %u | grep loss | cut -f3 -d, 1>& /tmp/pingout.txt",ip,timeout);
#endif
	system_with_log(gCmdStr); 

    /** scan/check the output */
    tmpfile = fopen("/tmp/pingout.txt", "r+");
    if(tmpfile == NULL)
    {		
		DPRINT_ERR(WFA_ERR, "file open failed\n%d", __LINE__);
		LEAVE( __func__ );
        return FALSE;
    }
   
    if(fscanf(tmpfile, "%s", strout) == EOF)
	   status = 0;
    else
    {
       pcnt = strtok(strout, "%"); 
       /* if the loss rate is 100%, not able to connect */
		if(atoi(pcnt) == 100)
			status = 0;
       else
			status = 1;
    }
    fclose(tmpfile);
	LEAVE( __func__ );
	return (status);
}


int fetch_mac_address ( char *ifname, char* mac) {
    
	char *str;
    int i,cnt,ret=0;
    FILE *tmpfd;
    char string[128];

	ENTER( __func__ ); 
	sprintf(gCmdStr, "%s ifconfig %s > /tmp/ipconfig.txt ",DUT_CMD_PREFIX,ifname); 
	system_with_log(gCmdStr);
	
    tmpfd = fopen("/tmp/ipconfig.txt", "r+");
    if(tmpfd == NULL) {
      DPRINT_ERR(WFA_ERR, "file open failed\n");
	  LEAVE( __func__ );
      return FALSE;
    }
	
    while( ret != -1 ) {
        ret = fscanf(tmpfd,"%s",string); 
		cnt = 0;
		i = 0;	
		while( i < strlen(string) ) {
			if ( string[i] == ':' ) 
				cnt++;	 	
			i++;		
		}
		if ( cnt == 5 && strlen(string) <= strlen("00:00:00:00:00:00") ) {
			break;	
		}		 
    }
    if ( strlen(string) != strlen("00:00:00:00:00:00") ) {
		LEAVE( __func__ );
		return FALSE;
    }	
    strcpy(mac,string);
	LEAVE( __func__ );
	return TRUE;
}

int get_kernal_release ( char *release ) {
	char buff[128] = {0} ;	
	FILE *fp;
	ENTER( __func__ );    
	sprintf(gCmdStr, "%s uname -r > /tmp/release.txt",DUT_CMD_PREFIX);		
	system_with_log(gCmdStr);
	fp = fopen("/tmp/release.txt","r");    
	fscanf(fp,"%s",buff);  
	fclose(fp); 
	if (buff[strlen(buff)] == "\n" )
		buff[strlen(buff)] = "\0";
	strcpy(release,buff);
	LEAVE( __func__ );
	return TRUE;
}


int get_mlan_driver_version ( char *ifname, char *version ) {
	char buff[128] = {0} ;	
	FILE *fp;
	ENTER( __func__ );    
	sprintf(gCmdStr, "%s %s version | %s cut -d: -f2  > /tmp/card.txt", MLANUTL,ifname,DUT_CMD_PREFIX);		
	system_with_log(gCmdStr);
	fp = fopen("/tmp/card.txt","r");    
	fscanf(fp,"%s",buff);  
	fclose(fp); 

	if (buff[strlen(buff)] == "\n" )
		buff[strlen(buff)] = "\0";
	strcpy(version,buff);
	LEAVE( __func__ );
	return TRUE;
}

int set_mlan_pmf_configuration ( char *ifname, int pmfType ) {
	char *pmfBits;
	ENTER( __func__ ); 	
	switch (pmfType) {
		case WFA_F_REQUIRED:
		pmfBits = "0 0";
		break;
		case WFA_REQUIRED:
		pmfBits = "1 1";
		break;
		case WFA_ENABLED: 
		pmfBits = "1 0";
		break;
		case WFA_F_DISABLED:
		pmfBits = "0 0";
		break;
		default:
		case WFA_DISABLED:
		pmfBits = "0 0";
		break;
	}	
	sprintf(gCmdStr,"%s/%s %s pmfcfg %s",APP_BIN_LOC,MLANUTL,ifname,pmfBits);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;	
}

int set_mlan_uapsd(char *ifname, int qoscfg) {
	ENTER( __func__ );  		
	sprintf(gCmdStr, "%s/%s %s wmmcfg 1",APP_BIN_LOC,MLANUTL,ifname);
	system_with_log(gCmdStr);

	sprintf(gCmdStr, "%s/%s %s qoscfg %d",APP_BIN_LOC,MLANUTL,ifname,qoscfg);
	system_with_log(gCmdStr);

	sprintf(gCmdStr, "%s/%s %s sleeppd 255",APP_BIN_LOC,MLANUTL,ifname);
	system_with_log(gCmdStr);

	LEAVE( __func__ );
	return TRUE;
}

int get_mlan_bssid_info ( char *ifname, char *bssid ) {
    char string[128];
    FILE *tmpfd;

	ENTER( __func__ );  
 	sprintf(gCmdStr, "%s cat %s | %s grep bssid | %s cut -f2 -d\\\" > /tmp/bssid.txt\n",DUT_CMD_PREFIX,mrvl_dut_info->mwlan_info_path,DUT_CMD_PREFIX,DUT_CMD_PREFIX);
	system_with_log(gCmdStr);
	
	tmpfd = fopen("/tmp/bssid.txt", "r+");
	if(tmpfd == NULL) {
		LEAVE( __func__ );
		return FALSE;
	}
	if(fgets(string, 256, tmpfd) == NULL) {
		LEAVE( __func__ );
		return FALSE;
	}
	
	if ( string[strlen(string) == '\n' ] )
		string[strlen(string) -1] = '\0';	
	
    strcpy(bssid, string);
    fclose(tmpfd);
	LEAVE( __func__ );
	return TRUE;
}

int get_mx_bssid_info ( char *ifname, char *bssid ) {
    char string[128];
    FILE *tmpfd;
    int i,cnt,ret=0;

	ENTER( __func__ );  	
	sprintf(gCmdStr, "iw dev %s link > /tmp/bssid.txt\n",ifname);
	system_with_log(gCmdStr);
	
	tmpfd = fopen("/tmp/bssid.txt", "r+");
	if( tmpfd == NULL ) {
		return FALSE;
	}
	while( ret != -1 ) {
       ret = fscanf(tmpfd,"%s",string); 
  	   cnt = 0;
	   i = 0;	
	   while( i < strlen(string) ) {
			if ( string[i] == ':' ) 
				cnt++;	 	
			i++;		
	   }
		if ( cnt == 5 && strlen(string) <= strlen("xx:xx:xx:xx:xx:xx") ) {
			break;	
		}		 
    }
	if ( strlen(string) != strlen("00:00:00:00:00:00") ) {
		strcpy(string,"00:00:00:00:00:00");
	}	
	strcpy(bssid, string);
	LEAVE( __func__ );
	return TRUE;
}

int get_mlan_essid_info ( char *ifname, char *essid ) {
	FILE *pipe=NULL;
	int i,j;
	ENTER( __func__ );	
	sprintf(gCmdStr, "%s cat %s | %s grep essid | %s cut -d= -f2",DUT_CMD_PREFIX,mrvl_dut_info->mwlan_info_path,DUT_CMD_PREFIX,DUT_CMD_PREFIX);			  
    pipe = popen(gCmdStr, "r");
    fgets(essid, 32, pipe);
	if ( essid[ strlen(essid) -1 ] == '\n' )
		essid[ strlen(essid) -1 ] = '\0';
	if ( essid[0] == '"' ) 
	{
		i=1;j=0;
		while( essid[i] != '"' )
		{	
			essid[j]=essid[i];
			i++;j++;
		}
		essid[j]='\0';
	} 
    pclose(pipe);	
	LEAVE( __func__ );
	return TRUE;
}

int configure_OpportunisticPowerSave ( char *ifname, int ctwindow) {
	ENTER( __func__ );  

	sprintf(gCmdStr,"%s/%s %s cfg_opp_ps 1 %d",APP_BIN_LOC,MLANUTL,ifname,ctwindow);
	system_with_log(gCmdStr);

	sprintf(gCmdStr,"%s/%s -i %s pscfg 2",APP_BIN_LOC,UAPUTL,ifname);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int configure_11k_mode (char *ifname, int mode) {
	ENTER( __func__ ); 
	sprintf(gCmdStr,"%s/%s %s 11k_enable %d",APP_BIN_LOC,MLANUTL,ifname,mode);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s %s 11k_enable",APP_BIN_LOC,MLANUTL,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;	
}

int configure_powersave_mode ( char *ifname, int mode ) {
	ENTER( __func__ );  
	switch (mode) {
		case 1:
			sprintf(gCmdStr,"iwconfig %s power on",ifname);
			break;
		case 0:
			sprintf(gCmdStr,"iwconfig %s power off",ifname);
			break;
		default:
			LEAVE( __func__ );
			return FALSE;
	}
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int configure_wmm_group (  char *ifname , int rts , int frag, int wmm ) {
	ENTER( __func__ ); 
	sprintf(gCmdStr, "iwconfig %s rts %d",ifname,rts);
	system_with_log(gCmdStr);
	
	sprintf(gCmdStr, "iwconfig %s frag %d",ifname,frag);
	system_with_log(gCmdStr);
	
	sprintf(gCmdStr, "%s/%s %s wmmcfg %d",APP_BIN_LOC,MLANUTL,ifname,wmm);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int configure_11n_settings(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
	ENTER( __func__ );

	LEAVE( __func__ );
	return TRUE;
}

int configure_vht_settings(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
	
    dutCmdResponse_t *vhtParamsResp = &gGenericResp;
    caStaSetWireless_t *staSetWireless = (caStaSetWireless_t *)caCmdBuf;
    struct setWirelessVHT5G * vhtParams = (struct setWirelessVHT5G *)&staSetWireless->progs.vht5g;
    char tmpStr[100], txsp_stream[10] = {0}, rxsp_stream[10] = {0};
    int follow_vhtcfg = 0xff;
    int htcap_changed = 0, vhtcap_changed = 0, httxcfg_changed = 0;	
    unsigned int htcapinfo = 0; /* Default value of htcapinfo */
    unsigned int httxcfg = 0;
    unsigned int vhtcapinfo = 0;
    int st = 0; // temp set to success for testing purpose.
    int is_testbed = 1;

	ENTER( __func__ );	
    if (!strcasecmp(sta_type, "Testbed"))
		is_testbed = 1;
    else
		is_testbed = 0;

    if(vhtParams->addba_reject_flag != 0 && vhtParams->addba_reject < 2) {
        if (vhtParams->addba_reject == WFA_ENABLED) {
            sprintf(gCmdStr, "%s/%s %s addbareject 1 1 1 1 1 1 1 1",APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            st = 0;
        } else if (vhtParams->addba_reject == WFA_DISABLED)  {
            sprintf(gCmdStr, "%s/%s %s addbareject 0 0 0 0 0 0 0 0", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            st = 0;
        } 
        if(st != 0) {
            vhtParamsResp->status = STATUS_ERROR;
            strcpy(vhtParamsResp->cmdru.info, "set_addba_reject failed");
            return WFA_FAILURE;
        }
    }

    if (vhtParams->ampdu_flag || vhtParams->amsdu_flag) {
        sprintf(gCmdStr, "%s/%s %s aggrpriotbl 2 2 0 0 1 1 3 3 4 4 5 5 6 6 7 7", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
        system_with_log(gCmdStr);
    }
    if (vhtParams->ampdu_flag) { 
        if (vhtParams->ampdu == WFA_ENABLED) {
            sprintf(gCmdStr, "%s/%s %s aggrpriotbl 2 2 0 0 1 1 3 3 4 4 5 5 6 5 7 7", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        }
        if (vhtParams->ampdu == WFA_DISABLED) {
            sprintf(gCmdStr, "%s/%s %s aggrpriotbl 255 2 255 0 255 1 255 3 255 4 255 5 255 5 255 7", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        }
    }

    if (vhtParams->amsdu_flag) {
        if (vhtParams->amsdu == WFA_ENABLED) {
            sprintf(gCmdStr, "%s/%s %s addbapara 65535 64 64 1 1", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        }
        if (vhtParams->amsdu == WFA_DISABLED) {
            sprintf(gCmdStr, "%s/%s %s addbapara 65535 64 64 0 0", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        }
    }

    httxcfg = get_cmd_str(MLANUTL, mrvl_dut_info->sta_interface, "httxcfg", "A band", 9);    
    if (vhtParams->greenfield_flag) {
        if (vhtParams->greenfield == WFA_ENABLED) {
            httxcfg |= (1 << 4);
        } else if (vhtParams->greenfield == WFA_DISABLED) {
            httxcfg &= ~(1 << 4);
        }
        httxcfg_changed = 1;
    }

    if (vhtParams->sgi20_flag) {
        if (vhtParams->sgi20 == WFA_ENABLED) {
            httxcfg |= (1 << 5);
        } else if (vhtParams->sgi20 == WFA_DISABLED) {
            httxcfg &= ~(1 << 5);
        }
        httxcfg_changed = 1;
    }
    
    if (vhtParams->width_flag) {
        if (!strcasecmp(vhtParams->width, "20")) {
            follow_vhtcfg = 0;
            sta_ch_width = 20;
            httxcfg &= ~(1 << 1);
            sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 0", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            httxcfg_changed = 1;
        } else if (!strcasecmp(vhtParams->width, "40")) {
            follow_vhtcfg = 0;
            sta_ch_width = 40;
            httxcfg |= (1 << 1);
            sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 0", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            httxcfg_changed = 1;
        } else if (!strcasecmp(vhtParams->width, "80")) {
            sta_ch_width = 80;
            sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 1", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        } else if (!strcasecmp(vhtParams->width, "160")) {
            printf("\n==> 160Mhz bandwidth not supported!!\n");
            vhtParamsResp->status = STATUS_ERROR;
            strcpy(vhtParamsResp->cmdru.info, "160Mhz Not supported");
            return WFA_FAILURE;
        }
    }

    /** read vhtcap info */
    vhtcapinfo = get_cmd_str(MLANUTL, mrvl_dut_info->sta_interface, "vhtcfg 2 2", "VHT Capabilities info", 8);
    printf("Read vhtcapinfo=0x%08x\n", vhtcapinfo);

    /** Bit 8 of htcapinfo is 40_intolerant */
    htcapinfo = get_cmd_str(MLANUTL, mrvl_dut_info->sta_interface, "htcapinfo", "A band", 9);
    if (vhtParams->_40_intolerant_flag) {
        if (vhtParams->_40_intolerant == WFA_ENABLED)
            htcapinfo |= (1 << 8);
        else if (vhtParams->_40_intolerant == WFA_DISABLED)
            htcapinfo &= ~(1 << 8);
        htcap_changed = 1;
    }
	
	
    if (vhtParams->stbc_rx_flag) {
	    printf("STA stbc_rx:%d\n", vhtParams->stbc_rx);
	    vhtcapinfo &= ~(7 << 8);
	    if (vhtParams->stbc_rx) {
		    vhtcapinfo |= ((vhtParams->stbc_rx) << 8);
		    htcapinfo |= (1 << 26);
	    } else {
		    htcapinfo &= ~(1 << 26);
	    }
	    htcap_changed = 1;
	    vhtcap_changed = 1;
    }

    /* sgi80 */
    if (vhtParams->sgi80_flag) {
        if (vhtParams->sgi80 == WFA_ENABLED)
            vhtcapinfo |= (1 << 5);
        else if ((vhtParams->sgi80 == WFA_DISABLED))
            vhtcapinfo &= ~(1 <<5);
        vhtcap_changed = 1;
    }

    /* TXSP_STREAM */
    if (vhtParams->nssMCSCap_flag) {
	printf("sta got nss_mcs_cap:%s\n", vhtParams->nssMCSCap);
    }
    if (!vhtParams->txsp_stream_flag) {
        sprintf(txsp_stream, ""); 
	if (vhtParams->nssMCSCap_flag) {
	    if (!strcasecmp(vhtParams->nssMCSCap, "1;0-9")) {
		    sprintf(txsp_stream, "0xfffe");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-8")) {
		    sprintf(txsp_stream, "0xfffd");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-7")) {
		    sprintf(txsp_stream, "0xfffc");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-9")) {
		    sprintf(txsp_stream, "0xfffa");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-8")) {
		    sprintf(txsp_stream, "0xfff5");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-7")) {
		    sprintf(txsp_stream, "0xfff0");
	    } else {
		    printf("Wrong configuration! nssMCSCap: %s\n",
				    vhtParams->nssMCSCap);
	    }
	    vhtcap_changed = 1;
            strcpy(sta_saved_txsp_stream, txsp_stream);
	}
    } else {
        if (vhtParams->txsp_stream == 1) {
            sprintf(txsp_stream, "0xfffe");
	    if (is_testbed)
		    sprintf(txsp_stream, "0xfffc");
	    if (vhtParams->nssMCSCap_flag) {
		    if (!strcasecmp(vhtParams->nssMCSCap, "1;0-9")) {
			    sprintf(txsp_stream, "0xfffe");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-8")) {
			    sprintf(txsp_stream, "0xfffd");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-7")) {
			    sprintf(txsp_stream, "0xfffc");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-")) {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
			    sprintf(txsp_stream, "0xfffe");
		    } else {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
		    }
	    }
            vhtcap_changed = 1;
            strcpy(sta_saved_txsp_stream, txsp_stream);
        } else if (vhtParams->txsp_stream == 2) {
            sprintf(txsp_stream, "0xfffa");
	    if (is_testbed)
		    sprintf(txsp_stream, "0xfff0");
	    if (vhtParams->nssMCSCap_flag) {
		    if (!strcasecmp(vhtParams->nssMCSCap, "2;0-9")) {
			    sprintf(txsp_stream, "0xfffa");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-8")) {
			    sprintf(txsp_stream, "0xfff5");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-7")) {
			    sprintf(txsp_stream, "0xfff0");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-")) {
			    printf("Wrong configuration!nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
			    sprintf(txsp_stream, "0xfffa");
		    } else {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
		    }
	    }
            vhtcap_changed = 1;
            strcpy(sta_saved_txsp_stream, txsp_stream);
        } else {
            printf("\n==> %d tx spatial stream not supported!!\n", vhtParams->txsp_stream);
            vhtParamsResp->status = STATUS_ERROR;
            strcpy(vhtParamsResp->cmdru.info, "Error setting tx_sp_stream supported");
            return WFA_FAILURE;
        } 
        strcpy(saved_txsp_stream, txsp_stream);
    }
    

    /* RXSP_STREAM */
    if (!vhtParams->rxsp_stream_flag) {
        sprintf(rxsp_stream, "");
	if (vhtParams->nssMCSCap_flag) {
	    if (!strcasecmp(vhtParams->nssMCSCap, "1;0-9")) {
		    sprintf(rxsp_stream, "0xfffe");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-8")) {
		    sprintf(rxsp_stream, "0xfffd");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-7")) {
		    sprintf(rxsp_stream, "0xfffc");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-9")) {
		    sprintf(rxsp_stream, "0xfffa");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-8")) {
		    sprintf(rxsp_stream, "0xfff5");
	    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-7")) {
		    sprintf(rxsp_stream, "0xfff0");
	    } else {
		    printf("Wrong configuration! nssMCSCap: %s\n",
				    vhtParams->nssMCSCap);
	    }
	    vhtcap_changed = 1;
            strcpy(sta_saved_rxsp_stream, rxsp_stream);
	}
    } else {
        if (vhtParams->rxsp_stream == 1) {
            sprintf(rxsp_stream, "0xfffe");
	    if (is_testbed)
		    sprintf(rxsp_stream, "0xfffc");
	    if (vhtParams->nssMCSCap_flag) {
		    if (!strcasecmp(vhtParams->nssMCSCap, "1;0-9")) {
			    sprintf(rxsp_stream, "0xfffe");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-8")) {
			    sprintf(rxsp_stream, "0xfffd");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-7")) {
			    sprintf(rxsp_stream, "0xfffc");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-")) {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
			    sprintf(rxsp_stream, "0xfffe");
		    } else {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
		    }
	    }
            vhtcap_changed = 1;
            strcpy(sta_saved_rxsp_stream, rxsp_stream);
        } else if (vhtParams->rxsp_stream == 2) {
            sprintf(rxsp_stream, "0xfffa");
	    if (is_testbed)
		    sprintf(rxsp_stream, "0xfff0");
	    if (vhtParams->nssMCSCap_flag) {
		    if (!strcasecmp(vhtParams->nssMCSCap, "2;0-9")) {
			    sprintf(rxsp_stream, "0xfffa");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-8")) {
			    sprintf(rxsp_stream, "0xfff5");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "2;0-7")) {
			    sprintf(rxsp_stream, "0xfff0");
		    } else if (!strcasecmp(vhtParams->nssMCSCap, "1;0-")) {
			    printf("Wrong configuration!nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
			    sprintf(rxsp_stream, "0xfffa");
		    } else {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    vhtParams->nssMCSCap);
		    }
	    }
            vhtcap_changed = 1;
            strcpy(sta_saved_rxsp_stream, rxsp_stream);
        } else if (vhtParams->rxsp_stream == 0xFF) {
            sprintf(rxsp_stream, "");
        } else {
            printf("\n==> %d rx spatial stream not supported!!\n", vhtParams->rxsp_stream);
            vhtParamsResp->status = STATUS_ERROR;
            strcpy(vhtParamsResp->cmdru.info, "Error setting rx_sp_stream");
            return WFA_FAILURE;
        }
    }

    if (vhtParams->ldpc_flag) {
        // bit 0 of httxcfg is for txldpc 
        // bit 4 of vhtcap is for rx ldpc 
        if (vhtParams->ldpc == 1) {
            httxcfg |= (1 << 0);
            vhtcapinfo |= (1 << 4);
        } else {
            httxcfg &= ~(1 << 0);
            vhtcapinfo &= ~(1 << 4);
        }
        vhtcap_changed = 1;
        httxcfg_changed = 1;
    }        

    /* tx beamformee */
    if (vhtParams->txBF_flag) {
        /* Bit 12 of vhtcap is su tx_beamformee */
        if (vhtParams->txBF == 1) {
            vhtcapinfo |= (1 << 12);
            vhtcapinfo |= (1 << 13);
            vhtcapinfo |= (1 << 14);
        } else {
            vhtcapinfo &= ~(1 << 12);
            vhtcapinfo &= ~(1 << 13);
            vhtcapinfo &= ~(1 << 14);
        }
	vhtcap_changed = 1;
    printf("changed tx bf bit\n");
    }
    
    /* read bw config info */
    follow_vhtcfg = get_bw_info(mrvl_dut_info->sta_interface,8);
    if (follow_vhtcfg == -1) {
		/** Give one more try with Id 10 */
		follow_vhtcfg = get_bw_info(mrvl_dut_info->sta_interface,10);
	}
    if (follow_vhtcfg == 0xff) {
        printf("Unable to get bw info\n");
        vhtParamsResp->status = STATUS_ERROR;
        strcpy(vhtParamsResp->cmdru.info, "Unable to get bw info");
        return WFA_FAILURE;
    }

    /* run the commands to set the htcapinfo and httxcfg we derieved above */
    if (htcap_changed) {
        sprintf(gCmdStr, "%s/%s %s htcapinfo 0x%x", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface, htcapinfo);
        system_with_log(gCmdStr);
    }
    if (httxcfg_changed) {
        sprintf(gCmdStr, "%s/%s %s httxcfg 0x%x 2", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface, httxcfg);
        system_with_log(gCmdStr);
    }
    if (vhtcap_changed) {
        sprintf(gCmdStr, "%s/%s %s vhtcfg 2 2 %d 0x%x %s %s", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface, follow_vhtcfg, vhtcapinfo, sta_saved_txsp_stream, sta_saved_rxsp_stream);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 %d 0x%x %s %s", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface, follow_vhtcfg, vhtcapinfo, sta_saved_txsp_stream, sta_saved_rxsp_stream);
        system_with_log(gCmdStr);
    }

    if (vhtParams->optMNotifIE_flag == 1) {
       if (strcasecmp(vhtParams->optMNotifIE, "1;20") == 0) {
			/** TODO*/
       } else if (strcasecmp(vhtParams->optMNotifIE, "1;40") == 0) {
			/** TODO*/
       } else if (strcasecmp(vhtParams->optMNotifIE, "1;80") == 0) {
			/** TODO*/
       } else if (strcasecmp(vhtParams->optMNotifIE, "2;20") == 0) {
			/** TODO*/
       } else if (strcasecmp(vhtParams->optMNotifIE, "2;40") == 0) {
			/** TODO*/
       } else if (strcasecmp(vhtParams->optMNotifIE, "2;80") == 0) {
			/** TODO*/
       }
    }

    if (vhtParams->zero_crc_flag) {
        if (vhtParams->zero_crc == WFA_ENABLED) {
			/** TODO*/
        } else if (vhtParams->zero_crc == WFA_DISABLED) {
			/** TODO*/
		}
    }

    if (vhtParams->dyn_bw_signal_flag == 1) {
        printf("===> Dyn BW signal flag 1\n");
	    if (vhtParams->dyn_bw_signal) {
		    printf("dynamic BW enabled, sta_ch_width=%d\n", sta_ch_width);
	    } else {
		    printf("dynamic BW disable, sta_ch_width=%d\n", sta_ch_width);
	    }
    }

    if (vhtParams->vht_tkip) {
        if (vhtParams->vht_tkip == WFA_ENABLED) {
            is_vht_tkip = 1;
        } else if (vhtParams->vht_tkip == WFA_DISABLED) {
			/** TODO*/
		}	
    }
	LEAVE( __func__ );
	return TRUE;
}

/* Divesh: Actual HE Parameter Handling based on the flags parsed/received from UCC */
/* The flag pasring was done in cmdproc and this function handles the param setting */

int configure_he_settings(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
	
    dutCmdResponse_t *heParamsResp = &gGenericResp;
    caStaSetWireless_t *staSetWireless = (caStaSetWireless_t *)caCmdBuf;
    struct setWirelessHE * heParams = (struct setWirelessHE *)&staSetWireless->progs.he;
    char tmpStr[100], txsp_stream[10] = {0}, rxsp_stream[10] = {0};
    int follow_vhtcfg = 0xff;
    int htcap_changed = 0, vhtcap_changed = 0, httxcfg_changed = 0;	
    unsigned int htcapinfo = 0; /* Default value of htcapinfo */
    unsigned int httxcfg = 0;
    unsigned int vhtcapinfo = 0;
    int st = 0; // temp set to success for testing purpose.
    int is_testbed = 1;

	ENTER( __func__ );	
    if (!strcasecmp(sta_type, "Testbed"))
		is_testbed = 1;
    else
		is_testbed = 0;
	
    if (heParams->width_flag) {

        if (!strcasecmp(heParams->width, "20")) {
           /* follow_vhtcfg = 0;
            sta_ch_width = 20;
            httxcfg &= ~(1 << 1); */
            sprintf(gCmdStr, "%s/SH-STA-HE-2.4G-20-2x2.sh", APP_BIN_LOC,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            /*httxcfg_changed = 1;*/
            DPRINT_INFO(WFA_OUT,"Divesh:SET 20 MHz");
        } else if (!strcasecmp(heParams->width, "40")) {
           /* follow_vhtcfg = 0;
            sta_ch_width = 40;
            httxcfg |= (1 << 1);
            sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 0", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            httxcfg_changed = 1; */
			DPRINT_INFO(WFA_OUT,"Divesh:SET 40 MHz");
	     sprintf(gCmdStr, "%s/SH-STA-HE-40-2x2.sh", APP_BIN_LOC,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        } else if (!strcasecmp(heParams->width, "80")) {
            /*sta_ch_width = 80;
            sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 1", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);*/
            DPRINT_INFO(WFA_OUT,"Divesh:SET 80 MHz");
	     sprintf(gCmdStr, "%s/SH-STA-HE-80-2x2.sh", APP_BIN_LOC,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        } /*else if (!strcasecmp(heParams->width, "160")) {
            printf("\n==> 160Mhz bandwidth not supported!!\n");
            heParamsResp->status = STATUS_ERROR;
            strcpy(heParamsResp->cmdru.info, "160Mhz Not supported");
            return WFA_FAILURE;
        }*/

			
    }
	if ( (heParams->OMControl_flag ||  heParams->UPH_flag) && (heParams->OMControl ==WFA_ENABLED ||  heParams->UPH == WFA_ENABLED) ) {
       DPRINT_INFO(WFA_OUT,"Divesh:OM-Control and UPH field settings logic if required");
	sprintf(gCmdStr, "%s/%s %s OMC_UPH_commands", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);	 
		}
    if (heParams->ADDBAReq_BufSize_flag) {

        if (!strcasecmp(heParams->ADDBAReq_BufSize, "gt64")) {

            sprintf(gCmdStr, "%s/%s %s SET_Super_BA_Command-TX", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
               DPRINT_INFO(WFA_OUT,"Divesh:ENABLE SUPER BA-TX ");
        } 
	else {
		DPRINT_INFO(WFA_OUT,"Divesh:NOT SUPER_BA...");
	      sprintf(gCmdStr, "%s/%s %s Normal BA Enabled", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        	}
    	}

    if (heParams->ADDBAResp_BufSize_flag) {

        if (!strcasecmp(heParams->ADDBAResp_BufSize, "gt64")) {

            sprintf(gCmdStr, "%s/%s %s SET_Super_BA_Command-RX", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);

            DPRINT_INFO(WFA_OUT,"Divesh:ENABLE SUPER BA-RX ");
        	} 
	else {
		DPRINT_INFO(WFA_OUT,"Divesh:NOT SUPER_BA...");
	      sprintf(gCmdStr, "%s/%s %s Normal BA Enabled", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        	}
    	}

    if (heParams->mcs_fixedrate_flag) {

        if (!strcasecmp(heParams->mcs_fixedrate, "7")) {

		if (heParams->ldpc_flag)
			{           
	     sprintf(gCmdStr, "%s/%s %s txratecfg 3 7 2 0x2220", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);

            DPRINT_INFO(WFA_OUT,"Divesh:ENABLE Fixed MCS RATE 7 With LDPC ");
			}
		if (heParams->bcc_flag)
			{            
	     sprintf(gCmdStr, "%s/%s %s txratecfg 3 7 2 0x2020", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);

            DPRINT_INFO(WFA_OUT,"Divesh:ENABLE Fixed MCS RATE 7 With BCC ");
			}


        }
	else {
		DPRINT_INFO(WFA_OUT,"Divesh:Default rates...");
	      sprintf(gCmdStr, "%s/%s %s Default rates", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
        	}
    	}


    if (heParams->amsdu_flag) {
	if(heParams->amsdu == WFA_ENABLED)	
          { sprintf(gCmdStr, "%s/%s %s aggrpriotbl 0 0 1 1 2 2 3 3 4 4 5 5 6 6 7 7", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            DPRINT_INFO(WFA_OUT,"Divesh:ENABLE AMSDU ");
        } else {
		DPRINT_INFO(WFA_OUT,"Divesh:Disable AMSDU");
	      sprintf(gCmdStr, "%s/%s %s aggrpriotbl 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
       
        	}
    	}
	
	if (heParams->ldpc_flag) {
		if (heParams->ldpc == WFA_ENABLED)
	{		   sprintf(gCmdStr, "%s/%s %s LDPC-ENABLE", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
				system_with_log(gCmdStr);
				DPRINT_INFO(WFA_OUT,"Divesh:ENABLE LDPC ");
			} else {
			DPRINT_INFO(WFA_OUT,"Divesh: LDPC-Disable");
			  sprintf(gCmdStr, "%s/%s %s  LDPC-DISABLED", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
				system_with_log(gCmdStr);
		   
			}
		}
	if (heParams->bcc_flag) {
		if (heParams->bcc == WFA_ENABLED)
	{	sprintf(gCmdStr, "%s/%s %s BCC-ENABLE", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
		system_with_log(gCmdStr);
		DPRINT_INFO(WFA_OUT,"Divesh:ENABLE BCC ");
		} else {
		DPRINT_INFO(WFA_OUT,"Divesh: BCC-Disable");
		sprintf(gCmdStr, "%s/%s %s  BCC-DISABLED", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
		system_with_log(gCmdStr);
			}
		}

	
	 if ((heParams->txsp_stream_flag) || (heParams->rxsp_stream_flag)) {

        if ((!strcasecmp(heParams->txsp_stream, "1SS")) || (!strcasecmp(heParams->rxsp_stream, "1SS"))) {
           /* follow_vhtcfg = 0;
            sta_ch_width = 20;
            httxcfg &= ~(1 << 1); */
            DPRINT_INFO(WFA_OUT,"Divesh:SET TX-NSS =1 and RX-NSS=1");
            sprintf(gCmdStr, "%s/%s %s TX-1SS -RX-1SS", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            /*httxcfg_changed = 1;*/

        } else if ((!strcasecmp(heParams->txsp_stream, "2SS")) || (!strcasecmp(heParams->rxsp_stream, "2SS")))  {
           /* follow_vhtcfg = 0;
            sta_ch_width = 40;
            httxcfg |= (1 << 1);
            sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 0", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
            system_with_log(gCmdStr);
            httxcfg_changed = 1; */
	     DPRINT_INFO(WFA_OUT,"Divesh:SET TX-NSS =2 and RX-NSS=2");
	     sprintf(gCmdStr, "%s/%s %s TX-2SS -RX-2SS", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
	     system_with_log(gCmdStr);
        } else {
        DPRINT_INFO(WFA_OUT,"Divesh:Handle the  TX-NSS  and RX-NSS settings or SET to OOB");
        sprintf(gCmdStr, "%s/%s %s Set TX_RX_NSS to OOB", APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
        system_with_log(gCmdStr);
	 	}
	 }
	LEAVE( __func__ );
	return TRUE;
}


int preset_vht_parameters(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
	caStaResetDefault_t *reset = (caStaResetDefault_t *)caCmdBuf;
	dutCmdResponse_t *ResetResp = &gGenericResp;
	ENTER( __func__ ); 
	
	FILE *pipe;
    int is_testbed_sta = 0;
    unsigned int vhtcapinfo = 0, httxcfg = 0, htcapinfo = 0;
	strcpy(prog_name,reset->prog);	
	if (reset->type) {
	    printf("%s: type=%s\n", __func__, reset->type);
		strcpy(sta_type, reset->type);
		
	} else {
	    printf("%s: set to default type=Testbed\n", __func__);
		strcpy(sta_type, "Testbed");
	}

    if (strcasecmp(sta_type, "testbed") == 0)
        is_testbed_sta = 1;
    else
        is_testbed_sta = 0;

	runWpswfd = 0;
	runCmdsAll = 0;
	autoGO = 0;

	ap_mcs_fixedrate = 0xff;
	ap_saved_txmcsmap = 0x0;
    ap_sgi80_hack = 0; 
	ap_vht_bw = 1;
	ap_ch_width = 80;
	sta_ch_width = 80;
	useSupplicantEnt = 0;
    is_vht_tkip = 0;
	memset(sta_saved_ssid, 0, 33);

    strcpy(sta_saved_txsp_stream, "0xfffa");
    strcpy(sta_saved_rxsp_stream, "0xfffa");

    strcpy(ap_saved_txsp_stream, "0xfffa");
    strcpy(ap_saved_rxsp_stream, "0xfffa");
	
	sprintf(gCmdStr, "iwconfig");
	system_with_log(gCmdStr);

	sprintf(gCmdStr, "iwconfig %s power off", mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);
    
    printf("Disable addba reject on both AP and STA\n");
    sprintf(gCmdStr, "%s/%s %s addbareject 0 0 0 0 0 0 0 0",APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
    system_with_log(gCmdStr);

    printf("Disable reassociation control\n");
	sprintf(gCmdStr, "iwpriv %s reassoctrl 0",mrvl_dut_info->sta_interface);
    system_with_log(gCmdStr);
 
    sprintf(gCmdStr,"iwpriv %s deauth",mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);
    
	sprintf(gCmdStr, "iwconfig %s key off", mrvl_dut_info->sta_interface);
    system_with_log(gCmdStr);

    /** default vhtcfg */
    vhtcapinfo = 0x33c179b0;
    sprintf(gCmdStr, "%s/%s %s vhtcfg 2 2 1 0x%x 0xfffa 0xfffa",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, vhtcapinfo);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 1 0x%x 0xfffa 0xfffa",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, vhtcapinfo);
    system_with_log(gCmdStr);

    /** default httxcfg */
    httxcfg = 0x00000062;
    sprintf(gCmdStr, "%s/%s %s httxcfg 0x%x",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, httxcfg);
    system_with_log(gCmdStr);

    /** default htcapinfo */
    htcapinfo = 0x05820000;
    sprintf(gCmdStr, "%s/%s %s htcapinfo 0x%x",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, htcapinfo);
    system_with_log(gCmdStr);

    /** Reset Custom IE index 5 */
    sprintf(gCmdStr, "%s/%s %s customie 5 0",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface);
    system_with_log(gCmdStr);

    /** Reset STA settings to default testbed device settings if the device is
     * testbed device */
    if (is_testbed_sta) {
        /** set vhtconfig */
        vhtcapinfo &= ~(1<<20); //disable MU beamformee
        vhtcapinfo &= ~(1<<19); //disable MU beamformer
        vhtcapinfo &= ~(7<<16); 
        vhtcapinfo &= ~(7<<13); 
        vhtcapinfo &= ~(1<<12); //disable SU beamformee
        vhtcapinfo &= ~(1<<11); //disable SU beamformer
        vhtcapinfo &= ~(7<<8); //disable Rx STBC
        vhtcapinfo &= ~(1<<7); //disable Tx STBC
        vhtcapinfo &= ~(1<<5); //disable SGI
        vhtcapinfo &= ~(1<<4); //disable LDPC
        printf("Changed vhtcapinfo=0x%08x\n", vhtcapinfo);
        /** MCS 0-7, 2 streams for testbed */
        sprintf(gCmdStr, "%s/%s %s vhtcfg 2 2 1 0x%x 0xfff0 0xfff0",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, vhtcapinfo);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s %s vhtcfg 2 1 1 0x%x 0xfff0 0xfff0",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, vhtcapinfo);
        system_with_log(gCmdStr);

        /** set httxcfg */
        httxcfg = get_cmd_str(MLANUTL, mrvl_dut_info->sta_interface, "httxcfg", "A band", 9);
        httxcfg &= ~(1<<0); //disable LDPC
        sprintf(gCmdStr, "%s/%s %s httxcfg 0x%x 2",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, httxcfg);
        system_with_log(gCmdStr);

        /** set htcapinfo */
        htcapinfo = get_cmd_str(MLANUTL, mrvl_dut_info->sta_interface, "htcapinfo", "A band", 9);
        htcapinfo &= ~(1 << 26); //disable STBC
        sprintf(gCmdStr, "%s/%s %s htcapinfo 0x%x",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface, htcapinfo);
        system_with_log(gCmdStr);

        /** default turn off ampdu+amsdu rx */
        printf("Turning of AMPDU + AMSDU Rx\n");
        sprintf(gCmdStr,"%s/%s %s addbapara 65535 64 32 0 0",APP_BIN_LOC,MLANUTL, mrvl_dut_info->sta_interface);
        system_with_log(gCmdStr);
    }
	LEAVE( __func__ );
	return TRUE;
}

int set_vht_nss_config (char *iface, int nss, int width) {
	ENTER( __func__ ); 
	if (1 == nss) {
		if (20 == width) {
			/** TODO*/
		} else if (40 == width) {
			/** TODO*/
		} else if (80 == width) {
			/** TODO*/
		}
	} else if (2 == nss) {
		if (20 == width) {
			/** TODO*/
		} else if (40 == width) {
			/** TODO*/
		} else if (80 == width) {
			/** TODO*/
		}
	}
	LEAVE( __func__ );
	return TRUE;
}

int set_mlan_deauth ( char *ifname ) {
	ENTER( __func__ ); 
	//DD: HE- SAE-Deauth for embedded requires passphrase flush
	/* if (!strcasecmp(SIGMA_PROG_NAME,MRVL_HE_PROG))
		{
	sprintf(gCmdStr, "%s %s deauth",IWPRIV,ifname);
       system_with_log(gCmdStr);
		}
else */
	{
	sprintf(gCmdStr, "%s %s deauth",IWPRIV,ifname);
       system_with_log(gCmdStr);
}
	   
	LEAVE( __func__ );
	return TRUE;
}

int sta_mlan_associate ( char *ifname, char *ssid ) {
	int cnt=0;
	ENTER( __func__ );
	
	if (!MWIFIEX) {
		if(is_vht_tkip) {
			/** Set Pairwise and group cipher as TKIP */
			sprintf(gCmdStr, "%s/%s %s esuppmode 9 4 4", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		}
		sprintf(gCmdStr, "iwconfig %s essid \"%s\"", ifname,ssid);
		system_with_log(gCmdStr);
	} else {
		sprintf(gCmdStr, "ifconfig %s up", ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iw dev %s scan", ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iw dev %s connect %s", ifname,ssid );
		system_with_log(gCmdStr);	
	}
	
	while (cnt<10) {
		sleep(2);
		if ( check_wlan_connectivity(ifname)  == TRUE )
			break;
		cnt++;
	}	
	LEAVE( __func__ );
	return TRUE;
}

int set_mlan_txrate( char *ifname, char *mcs, char *nss ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s %s txratecfg 2 %s %s", APP_BIN_LOC,MLANUTL,ifname, mcs, nss);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_mlan_open_configuration( char *ifname  ) {
	ENTER( __func__ ); 
	sprintf(gCmdStr, "iwconfig %s key off",ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_mlan_wep_configuration( char *key1, char*key2, char *key3, char *key4, int activeKeyIdx  ) {
	ENTER( __func__ );
	
	sprintf(gCmdStr, "iwconfig %s key off", mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);
						
	if (key1[0] != '\0') {
		sprintf(gCmdStr, "iwconfig %s key [1] open", mrvl_dut_info->sta_interface);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iwconfig %s key [1] %s", mrvl_dut_info->sta_interface, key1);
		system_with_log(gCmdStr);
	}
	if (key2[0] != '\0') {
		sprintf(gCmdStr, "iwconfig %s key [2] open", mrvl_dut_info->sta_interface);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iwconfig %s key [2] %s", mrvl_dut_info->sta_interface,key2);
		system_with_log(gCmdStr);
	}
	if (key3[0] != '\0') {
		sprintf(gCmdStr, "iwconfig %s key [3] open", mrvl_dut_info->sta_interface);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iwconfig %s key [3] %s", mrvl_dut_info->sta_interface, key3);
		system_with_log(gCmdStr);		
	}
	if (key4[0] != '\0') {
		sprintf(gCmdStr, "iwconfig %s key [4] open", mrvl_dut_info->sta_interface);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "iwconfig %s key [4] %s", mrvl_dut_info->sta_interface, key4);
		system_with_log(gCmdStr);	
	}	
	sprintf(gCmdStr, "iwconfig %s key ON", mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;		
}

int set_mlan_passphrase( char* ifname, char* ssid, char* passphrase) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s %s passphrase \"1;ssid=%s;passphrase=%s\"", IWPRIV,ifname, ssid, passphrase);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;	
}

int set_mlan_channel ( char *ifname, int channel ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "iwconfig %s channel %d", ifname, channel);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_mlan_mode( char *ifname, int mode ) {
	ENTER( __func__ );
	
	switch (mode) {
		case 0: 
		sprintf(gCmdStr, "iwconfig %s mode Managed", ifname);
		system_with_log(gCmdStr);
		break;
		case 1: 
		sprintf(gCmdStr, "iwconfig %s mode Ad-Hoc", ifname);
		system_with_log(gCmdStr);
		break;
		default:
			LEAVE( __func__ );
			return FALSE;
	}
	LEAVE( __func__ );
	return TRUE;
}


int mrvlAPSetSecurityPMF(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
    caAPSetSecurity_t *apSecurity = (caAPSetSecurity_t *) caCmdBuf;
    dutCmdResponse_t *apSetSecurityResp = &gGenericResp;
    char *ifname = apSecurity->intf;
    strcpy(ifname,mrvl_dut_info->mmh_interface);	
    ENTER( __func__ );     

	/** Configure Security */
	sprintf(gCmdStr, "%s/%s sys_cfg_auth 0",APP_BIN_LOC,UAPUTL);
	system_with_log(gCmdStr);
	if (strncasecmp(apSecurity->keymgnt, "NONE", 5) == 0) {
		sprintf(gCmdStr, "%s/%s sys_cfg_protocol 1",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	} else if (strncasecmp(apSecurity->keymgnt, "WPA-PSK", 8) == 0) {
       sprintf(gCmdStr, "%s/%s sys_cfg_protocol 8",APP_BIN_LOC,UAPUTL); 
       system_with_log(gCmdStr);   
       sprintf(gCmdStr, "%s/%s sys_cfg_cipher 4 4 ",APP_BIN_LOC,UAPUTL); 
       system_with_log(gCmdStr);
       sprintf(gCmdStr, "%s/%s sys_cfg_wpa_passphrase \"%s\"",APP_BIN_LOC,UAPUTL,apSecurity->psk );
       system_with_log(gCmdStr);
    } else if (strncasecmp(apSecurity->keymgnt, "WPA2-PSK", 9)== 0) {
		if (!strcasecmp(apSecurity->pmf,"required") && !strcasecmp(apSecurity->sha256ad,"enabled")) {
			sprintf(gCmdStr, "%s/%s sys_cfg_protocol 32 0x100",APP_BIN_LOC,UAPUTL);
		} else if (!strcasecmp(apSecurity->pmf,"optional") && !strcasecmp(apSecurity->sha256ad,"enabled")) {
			sprintf(gCmdStr, "%s/%s sys_cfg_protocol 32 0x102",APP_BIN_LOC,UAPUTL);
		} else if (!strcasecmp(apSecurity->pmf,"required")) {
			sprintf(gCmdStr, "%s/%s sys_cfg_protocol 32 0x100",APP_BIN_LOC,UAPUTL);
		} else { 
     	   sprintf(gCmdStr, "%s/%s sys_cfg_protocol 32",APP_BIN_LOC,UAPUTL);
		}	 		
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s sys_cfg_cipher 8 8 ",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s sys_cfg_wpa_passphrase \"%s\"",APP_BIN_LOC,UAPUTL,apSecurity->psk );
		system_with_log(gCmdStr);
    } else if (strncasecmp(apSecurity->keymgnt, "WPA2-PSK-MIXED", 14) == 0) {
       sprintf(gCmdStr, "%s/%s sys_cfg_protocol 40",APP_BIN_LOC,UAPUTL); 
       system_with_log(gCmdStr);
       sprintf(gCmdStr, "%s/%s sys_cfg_cipher 12 4 ",APP_BIN_LOC,UAPUTL); 
       system_with_log(gCmdStr);
       sprintf(gCmdStr, "%s/%s sys_cfg_wpa_passphrase \"%s\"",APP_BIN_LOC,UAPUTL,apSecurity->psk );
       system_with_log(gCmdStr);
    } else {
       sprintf(gCmdStr, "%s/%s sys_cfg_protocol 1",APP_BIN_LOC,UAPUTL); 
       system_with_log(gCmdStr);
    }
	if ( !strcasecmp(apSecurity->pmf,"required") ) {
		sprintf(gCmdStr, "%s/%s sys_cfg_pmf 1 1",APP_BIN_LOC,UAPUTL,ifname); 
		system_with_log(gCmdStr);
	} else if ( !strcasecmp(apSecurity->pmf,"optional") ) {
		sprintf(gCmdStr, "%s/%s sys_cfg_pmf 1 0",APP_BIN_LOC,UAPUTL, ifname); 
		system_with_log(gCmdStr);
	} else if ( !strcasecmp(apSecurity->pmf,"disabled") ) {
		sprintf(gCmdStr, "%s/%s sys_cfg_pmf 0 0",APP_BIN_LOC,UAPUTL, ifname); 
		system_with_log(gCmdStr);
	} else {
		/** Do nothing */
	}
	LEAVE( __func__ );
	return TRUE;
}

int mrvlAPSetSecurityVHT(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
    caAPSetSecurity_t *apSecurity = (caAPSetSecurity_t *) caCmdBuf;
    dutCmdResponse_t *apSetSecurityResp = &gGenericResp;
    char *ifname = apSecurity->intf;
    strcpy(ifname,mrvl_dut_info->mmh_interface);	
    ENTER( __func__ );     
    
	sprintf(gCmdStr, "%s/%s sys_cfg_auth 0",APP_BIN_LOC,UAPUTL);
	system_with_log(gCmdStr);

	/** Set key management */
	if (strncasecmp(apSecurity->keymgnt, "NONE", 5) == 0) {
		sprintf(gCmdStr, "%s/%s sys_cfg_protocol 1",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	} else if (strncasecmp(apSecurity->keymgnt, "WPA-PSK", 8) == 0) {
		sprintf(gCmdStr, "%s/%s sys_cfg_protocol 8",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s sys_cfg_cipher 4 4 ",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		invalid_for_11ac = 1;
	} else if (strncasecmp(apSecurity->keymgnt, "WPA2-PSK", 9)== 0) {
		sprintf(gCmdStr, "%s/%s sys_cfg_protocol 32",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);   
		sprintf(gCmdStr, "%s/%s sys_cfg_cipher 8 8 ",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	} else if (strncasecmp(apSecurity->keymgnt, "WPA2-PSK-MIXED", 14) == 0) {
		sprintf(gCmdStr, "%s/%s sys_cfg_protocol 40",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);   
		sprintf(gCmdStr, "%s/%s sys_cfg_cipher 12 4 ",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	} else if (strncasecmp(apSecurity->keymgnt, "WPA2-Ent", 9) == 0) {
        if (ap_use_hostapd == 0) {
            ap_use_hostapd = 1;
        }
	} else {
		sprintf(gCmdStr, "%s/%s sys_cfg_protocol 1",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	}
   
	if ( apSecurity->encry_flag == 1) {
		sprintf(gCmdStr, "%s/%s sys_cfg_protocol 2",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	}
	/** Set PSK */
	if (apSecurity->psk_flag == 1) {
		sprintf(gCmdStr, "%s/%s sys_cfg_wpa_passphrase \"%s\"",APP_BIN_LOC,UAPUTL, apSecurity->psk );
		system_with_log(gCmdStr);
	}

    /** Set WEP key.
       CAPI spec doesnt set default key index. Set to zero */
	if (apSecurity->wep_flag == 1) {
		sprintf(gCmdStr, "%s/%s sys_cfg_wep_key 0 1 %s",APP_BIN_LOC,UAPUTL,apSecurity->wepkey);
		system_with_log(gCmdStr); 
		invalid_for_11ac = 1;
	}
	LEAVE( __func__ );
	return TRUE;
} 


int mrvlAPSetWirelessVHT (int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
	caAPSetWireless_t *set_wireless_params = (caAPSetWireless_t *) caCmdBuf;
    char *ifname = set_wireless_params->intf;
    dutCmdResponse_t *apSetWirelessResp = &gGenericResp;
    ENTER( __func__ );
	
	int chan_ext = 2;
	char tmpStr[100], txsp_stream[10] = {0}, rxsp_stream[10] = {0};
	int follow_vhtcfg = 0xff;
	int htcap_changed = 0, vhtcap_changed = 0, httxcfg_changed = 0;
	char spatial_rx_stream[10] = {0}, spatial_tx_stream[10] ={0};
	int nss = 2; /* default */

	/** Read back the existing vhtcapinfo */
	unsigned int vhtcapinfo = 0;
	int is_testbed = 1;
	unsigned int httxcfg = 0;
	unsigned int htcapinfo = 0;
	short ap_mode = 0;
	strcpy(ifname,mrvl_dut_info->mmh_interface); 
	 
    /* Read vhtcap info */
    vhtcapinfo = get_cmd_str(MLANUTL, ifname, "vhtcfg 2 3", "VHT Capabilities info", 8);
    DPRINT_INFO(WFA_OUT,"Read vhtcapinfo for AP=0x%08x\n", vhtcapinfo);
   
    if (strcasecmp(ap_type, "Testbed") == 0) {
		is_testbed = 1;
	} else
		is_testbed = 0;

	if (set_wireless_params->width_flag)
		width = set_wireless_params->width;
	if (set_wireless_params->bw_signal_flag) {
		bw_signal = set_wireless_params->bw_signal;
		bw_signal_flag = set_wireless_params->bw_signal_flag;
	}
	if (set_wireless_params->dyn_bw_signal_flag) {
		dyn_bw_signal = set_wireless_params->dyn_bw_signal;
		dyn_bw_signal_flag = set_wireless_params->dyn_bw_signal_flag;
	}

	httxcfg = get_cmd_str(MLANUTL,ifname, "httxcfg", "A band", 9);
	DPRINT_INFO(WFA_OUT,"\nAP read httxcfg=0x%08x\n", httxcfg);

	htcapinfo = get_cmd_str(MLANUTL,ifname, "htcapinfo", "A band", 9);
	DPRINT_INFO(WFA_OUT,"AP read htcapinfo=0x%08x\n", htcapinfo);

	sprintf(gCmdStr, "%s/%s bss_stop ",APP_BIN_LOC,UAPUTL); 
	system_with_log(gCmdStr);

	if (set_wireless_params->ssid_flag) {
		sprintf(gCmdStr, "%s/%s sys_cfg_ssid \"%s\"",APP_BIN_LOC,UAPUTL,set_wireless_params->ssid ); 
		system_with_log(gCmdStr);       
	}
   
	if (set_wireless_params->rts_flag) {
		sprintf(gCmdStr, "%s/%s sys_cfg_rts_threshold %d",APP_BIN_LOC,UAPUTL,set_wireless_params->rts); 
		system_with_log(gCmdStr);
	}

	if (set_wireless_params->frgmnt_flag) {
		sprintf(gCmdStr, "%s/%s sys_cfg_frag_threshold %d",APP_BIN_LOC,UAPUTL,set_wireless_params->frgmnt); 
		system_with_log(gCmdStr);
	}

	if (set_wireless_params->mode_flag) {
		if(strncmp(set_wireless_params->mode, "11g",3)==0)
			ap_mode = 0;
		else if(strncmp(set_wireless_params->mode, "11a",3)==0)
			ap_mode = 1;
	}

	if (set_wireless_params->bcnint_flag) {
		sprintf(gCmdStr, "%s/%s sys_cfg_beacon_period %s", APP_BIN_LOC,UAPUTL,set_wireless_params->bcnint);
		system_with_log(gCmdStr);
	}

	if (set_wireless_params->channel_flag) {
		switch (set_wireless_params->channel) {
		   case 36: 
		   case 44: 
		   case 52: 
		   case 60: 
		   case 100: 
		   case 108: 
		   case 116: 
		   case 124: 
		   case 132: 
		   case 149: 
		   case 157:
			   chan_ext = 2;
			   break;
		   case 40: 
		   case 48: 
		   case 56: 
		   case 64: 
		   case 104: 
		   case 112: 
		   case 120: 
		   case 128: 
		   case 136: 
		   case 153: 
		   case 161: 
			   chan_ext = 4;
			   break;
		   default:
			   break;
       }
       sprintf(gCmdStr, "%s/%s sys_cfg_channel %d %d",APP_BIN_LOC,UAPUTL,set_wireless_params->channel, chan_ext);
       system_with_log(gCmdStr);
       if (set_wireless_params->channel > 14) {
           sprintf(gCmdStr, "%s/%s sys_cfg_rates 0x8c 0x12 0x98 0x24 0xb0 0x48 0x60 0x6c",APP_BIN_LOC,UAPUTL);
       } else {
           sprintf(gCmdStr, "%s/%s sys_cfg_rates 0x82 0x84 0x8b 0x96 0x0c 0x12 0x18 0x24 0x30 0x48 0x60 0x6c",APP_BIN_LOC,UAPUTL);
       }
       system_with_log(gCmdStr);
    }

	if (set_wireless_params->addba_reject_flag) {
		if (set_wireless_params->addba_reject == WFA_ENABLED)
		   sprintf(gCmdStr, "%s/%s addbareject 1 1 1 1 1 1 1 1",APP_BIN_LOC,UAPUTL); 
		else
		   sprintf(gCmdStr, "%s/%s addbareject 0 0 0 0 0 0 0 0",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	}
   
    if (set_wireless_params->tdls_prohibit_flag && (set_wireless_params->tdls_prohibit == WFA_ENABLED)) {
         sprintf(gCmdStr, "%s/%s sys_cfg_tdls_ext_cap /sbin/tdls_proh.conf",APP_BIN_LOC,UAPUTL);
         system_with_log(gCmdStr);
    } 
	if (set_wireless_params->tdls_ch_switch_prohibit_flag  && (set_wireless_params->tdls_ch_switch_prohibit == WFA_ENABLED) ) {
        sprintf(gCmdStr, "%s/%s sys_cfg_tdls_ext_cap /sbin/tdls_csproh.conf",APP_BIN_LOC,UAPUTL);
        system_with_log(gCmdStr);
	}
	if (set_wireless_params->mcs_fixedrate_flag) {
		ap_mcs_fixedrate = set_wireless_params->mcs_fixedrate;
	}
	if (set_wireless_params->ampdu_flag || set_wireless_params->amsdu_flag) {
		sprintf(gCmdStr, "%s/%s %s aggrpriotbl 2 2 0 0 1 1 3 3 4 4 5 5 6 6 7 7", APP_BIN_LOC,MLANUTL,ifname);
		system_with_log(gCmdStr);
	}
	if (set_wireless_params->ampdu_flag) { 
		if (set_wireless_params->ampdu == WFA_ENABLED) {
			sprintf(gCmdStr, "%s/%s %s aggrpriotbl 2 2 0 0 1 1 3 3 4 4 5 5 6 5 7 7", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		}
		if (set_wireless_params->ampdu == WFA_DISABLED) {
			sprintf(gCmdStr, "%s/%s %s aggrpriotbl 255 2 255 0 255 1 255 3 255 4 255 5 255 5 255 7", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		}
	}
	if (set_wireless_params->amsdu_flag) {
		if (set_wireless_params->amsdu == WFA_ENABLED) {
			sprintf(gCmdStr, "%s/%s %s addbapara 65535 64 64 1 1", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		}
		if (set_wireless_params->amsdu == WFA_DISABLED) {
			sprintf(gCmdStr, "%s/%s %s addbapara 65535 64 64 0 0", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		}
	}

	/** Greenfield enable */
	if ( set_wireless_params->gf_flag && set_wireless_params->gf ) {
		sprintf(gCmdStr, "%s/%s sys_cfg_11n 1 0x111C 3", APP_BIN_LOC,UAPUTL);
		system_with_log(gCmdStr);	
	}
	
	/** SGI20 */
	if ( set_wireless_params->sgi20_flag && set_wireless_params->sgi20 ) {
		sprintf(gCmdStr, "%s/%s sys_cfg_11n 1 0x112C 3", APP_BIN_LOC,UAPUTL);
		system_with_log(gCmdStr);	
	}
	
	/** MCS32 */
	if ( set_wireless_params->mcs32_flag && set_wireless_params->mcs32 ) {
		sprintf(gCmdStr, "%s/%s sys_cfg_11n 1 0x117e 3", APP_BIN_LOC,UAPUTL);
		system_with_log(gCmdStr);	
	}
		
    /** set tx/rx mcs map to 0xfffa, 1X1/2X2/0-9 */
    if (ap_saved_txsp_stream[0] == 0 || ap_saved_rxsp_stream[0] == 0) {
        strcpy(ap_saved_txsp_stream, "0xfffa");
        strcpy(ap_saved_rxsp_stream, "0xfffa");
    }
	
    /* TXSP_STREAM */
    if (set_wireless_params->nssMCSCap_flag) {
		printf("ap got nss_mcs_cap:%s\n", set_wireless_params->nssMCSCap);
    }
    if (!set_wireless_params->spatial_tx_stream_flag) {
        sprintf(spatial_tx_stream, ""); 
		if (set_wireless_params->nssMCSCap_flag) {
			if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-9")) {
				sprintf(spatial_tx_stream, "0xfffe");
			} else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-8")) {
				sprintf(spatial_tx_stream, "0xfffd");
			} else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-7")) {
				sprintf(spatial_tx_stream, "0xfffc");
			} else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-9")) {
				sprintf(spatial_tx_stream, "0xfffa");
			} else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-8")) {
				sprintf(spatial_tx_stream, "0xfff5");
			} else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-7")) {
				sprintf(spatial_tx_stream, "0xfff0");
			} else {
				printf("Wrong configuration! nssMCSCap: %s\n",
						set_wireless_params->nssMCSCap);
			}
			vhtcap_changed = 1;
            strcpy(ap_saved_txsp_stream, spatial_tx_stream);
		}
    } else {
        printf("==> Got tx stream %d\n", set_wireless_params->spatial_tx_stream);
        if (set_wireless_params->spatial_tx_stream == 1) {
            sprintf(spatial_tx_stream, "0xfffe");
	    // for testbed: mcs8/9 off
	    if (is_testbed)
		    sprintf(spatial_tx_stream, "0xfffc");
	    if (set_wireless_params->nssMCSCap_flag) {
		    if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-9")) {
			    sprintf(spatial_tx_stream, "0xfffe");
		    } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-8")) {
			    sprintf(spatial_tx_stream, "0xfffd");
		    } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-7")) {
			    sprintf(spatial_tx_stream, "0xfffc");
		    } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-")) {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    set_wireless_params->nssMCSCap);
			    sprintf(spatial_tx_stream, "0xfffe");
		    } else {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    set_wireless_params->nssMCSCap);
		    }
	    }
	    vhtcap_changed = 1;
            strcpy(ap_saved_txsp_stream, spatial_tx_stream);
        } else if (set_wireless_params->spatial_tx_stream == 2) {
            sprintf(spatial_tx_stream, "0xfffa");
	    //For testbed: mcs8/9 off
	    if (is_testbed)
		    sprintf(spatial_tx_stream, "0xfff0");
	    if (set_wireless_params->nssMCSCap_flag) {
		    if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-9")) {
			    sprintf(spatial_tx_stream, "0xfffa");
		    } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-8")) {
			    sprintf(spatial_tx_stream, "0xfff5");
		    } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-7")) {
			    sprintf(spatial_tx_stream, "0xfff0");
		    } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-")) {
			    printf("Wrong configuration!nssMCSCap: %s\n",
					    set_wireless_params->nssMCSCap);
			    sprintf(spatial_tx_stream, "0xfffa");
		    } else {
			    printf("Wrong configuration! nssMCSCap: %s\n",
					    set_wireless_params->nssMCSCap);
		    }
	    }
	    vhtcap_changed = 1;
            strcpy(ap_saved_txsp_stream, spatial_tx_stream);
        } else {
            printf("\n==> %d tx spatial stream not supported!!\n", set_wireless_params->spatial_tx_stream);
            apSetWirelessResp->status = STATUS_ERROR;
            strcpy(apSetWirelessResp->cmdru.info, "Error setting spatial_tx_stream supported");
            return WFA_FAILURE;
        } 
    }
    printf("Ap tx mcs map=%s\n", ap_saved_txsp_stream);
    ap_saved_txmcsmap = (unsigned int)strtoul(ap_saved_txsp_stream, NULL, 16);
    printf("Ap tx mcs map=0x%08x\n", ap_saved_txmcsmap);
   
    /* RXSP_STREAM */
    if (!set_wireless_params->spatial_rx_stream_flag) {
        sprintf(spatial_rx_stream, "");
        if (set_wireless_params->nssMCSCap_flag) {
            if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-9")) {
                sprintf(spatial_rx_stream, "0xfffe");
            } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-8")) {
                sprintf(spatial_rx_stream, "0xfffd");
            } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-7")) {
                sprintf(spatial_rx_stream, "0xfffc");
            } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-9")) {
                sprintf(spatial_rx_stream, "0xfffa");
            } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-8")) {
                sprintf(spatial_rx_stream, "0xfff5");
            } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-7")) {
                sprintf(spatial_rx_stream, "0xfff0");
            } else {
                printf("Wrong configuration! nssMCSCap: %s\n",
                        set_wireless_params->nssMCSCap);
            }
            vhtcap_changed = 1;
            strcpy(ap_saved_rxsp_stream, spatial_rx_stream);
        }
    } else {
        printf("==> Got rx stream %d\n", set_wireless_params->spatial_rx_stream);
        if (set_wireless_params->spatial_rx_stream == 1) {
            sprintf(spatial_rx_stream, "0xfffe");
            if (is_testbed)
                sprintf(spatial_rx_stream, "0xfffc");
            if (set_wireless_params->nssMCSCap_flag) {
                if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-9")) {
                    sprintf(spatial_rx_stream, "0xfffe");
                } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-8")) {
                    sprintf(spatial_rx_stream, "0xfffd");
                } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-7")) {
                    sprintf(spatial_rx_stream, "0xfffc");
                } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-")) {
                    printf("Wrong configuration! nssMCSCap: %s\n",
                            set_wireless_params->nssMCSCap);
                    sprintf(spatial_rx_stream, "0xfffe");
                } else {
                    printf("Wrong configuration! nssMCSCap: %s\n",
                            set_wireless_params->nssMCSCap);
                }
            }
            vhtcap_changed = 1;
            strcpy(ap_saved_rxsp_stream, spatial_rx_stream);
        } else if (set_wireless_params->spatial_rx_stream == 2) {
            sprintf(spatial_rx_stream, "0xfffa");
            if (is_testbed)
                sprintf(spatial_rx_stream, "0xfff0");
            if (set_wireless_params->nssMCSCap_flag) {
                if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-9")) {
                    sprintf(spatial_rx_stream, "0xfffa");
                } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-8")) {
                    sprintf(spatial_rx_stream, "0xfff5");
                } else if (!strcasecmp(set_wireless_params->nssMCSCap, "2;0-7")) {
                    sprintf(spatial_rx_stream, "0xfff0");
                } else if (!strcasecmp(set_wireless_params->nssMCSCap, "1;0-")) {
                    printf("Wrong configuration!nssMCSCap: %s\n",
                            set_wireless_params->nssMCSCap);
                    sprintf(spatial_rx_stream, "0xfffa");
                } else {
                    printf("Wrong configuration! nssMCSCap: %s\n",
                            set_wireless_params->nssMCSCap);
                }
            }
            vhtcap_changed = 1;
            strcpy(ap_saved_rxsp_stream, spatial_rx_stream);
        } else {
            printf("\n==> %d rx spatial stream not supported!!\n", set_wireless_params->spatial_rx_stream);
            apSetWirelessResp->status = STATUS_ERROR;
            strcpy(apSetWirelessResp->cmdru.info, "Error setting spatial rx stream");
            return WFA_FAILURE;
        }
    }
    printf("Ap rx mcs map=%s\n", ap_saved_rxsp_stream);
    if (set_wireless_params->vht_tkip_flag == 1) {
		/* TODO */	
    }
    if (set_wireless_params->vht_wep_flag == 1) {
		/* TODO */
    }
	
    if (set_wireless_params->stbc_tx_flag == 1) {
        /* Bit 7 of vhtcapinfo is tx_stbc */
         vhtcapinfo |= (1 << 7);
         vhtcap_changed = 1;
    }
    if (set_wireless_params->tx_bf_flag == 1) {
        if (set_wireless_params->tx_bf == 1) {
            /* bit 11 of vhtcapinfo is su tx beamformer */
            vhtcapinfo |= (1 << 11);

            /* Bit 18-16: Number of sounding dimensions (set to maximum-1
			   if Bit 11 is 1. Otherwise, reserved and set to 0) */
            vhtcapinfo |= (1 << 16);

        } else {
            vhtcapinfo &= ~(1 << 11);
            vhtcapinfo &= ~(1 << 16);
        }
        vhtcap_changed = 1;
    }
    if (set_wireless_params->htc_vht_flag == 1) {
        printf("HTC-VHT capable flag:%s\n", set_wireless_params->htc_vht ? "enable" : "disable");
        if (set_wireless_params->htc_vht) {
            vhtcapinfo |= (1 << 22);
        } else {
            vhtcapinfo &= ~(1 << 22);
        }
        vhtcap_changed = 1;
    }
    if (set_wireless_params->width_flag == 1) {
	    printf("set_wireless_params->width=%d\n",
			    set_wireless_params->width);
	    if (set_wireless_params->width != 80 &&
			    (set_wireless_params->width == 40 ||
			     set_wireless_params->width == 20)) {
		    ap_vht_bw = 0;
		    ap_ch_width = set_wireless_params->width;
		    printf("Set VHT width: set_wireless_params->width=%d, ap_vht_bw=%d\n",
				    set_wireless_params->width, ap_vht_bw);
	    }
    }
    if (set_wireless_params->dyn_bw_signal_flag == 1) {
        printf("===> Dyn BW signal flag 1\n");
	    if (set_wireless_params->dyn_bw_signal) {
		    printf("dynamic BW enabled, ap_ch_width=%d\n", ap_ch_width);			
	    } else {
		    printf("dynamic BW disable, ap_ch_width=%d\n", ap_ch_width);
	    }
    }
    if (set_wireless_params->ldpc_flag) {
	    printf("AP LDPC setting:%s\n", set_wireless_params->ldpc ? "enable" : "disable");
	    if (set_wireless_params->ldpc) {
		    vhtcapinfo |= (1 << 4);
		    httxcfg |= 0x1;
	    } else {
		    vhtcapinfo &= ~(1 << 4);
		    httxcfg &= ~(0x1);
	    }
	    vhtcap_changed = 1;
	    httxcfg_changed = 1;
    }
    if (set_wireless_params->country_code_flag) {
	    printf("Set country code:%s\n", set_wireless_params->country_code);
	    sprintf(gCmdStr, "%s/%s sys_cfg_80211d state 1 country %s",APP_BIN_LOC,UAPUTL,set_wireless_params->country_code);
	    system_with_log(gCmdStr);
    } 
    /* sgi80 */
    if (set_wireless_params->sgi80_flag) {
        if (set_wireless_params->sgi80 == WFA_ENABLED) {
            vhtcapinfo |= (1 << 5);
            ap_sgi80_hack = 1;
        } else if ((set_wireless_params->sgi80 == WFA_DISABLED)) {
            vhtcapinfo &= ~(1 <<5);
        }
        vhtcap_changed = 1;
    }
    if (httxcfg_changed) {
        sprintf(gCmdStr, "%s/%s %s httxcfg 0x%x", APP_BIN_LOC,MLANUTL,ifname, httxcfg);
        system_with_log(gCmdStr);
    }
    if (vhtcap_changed) {
        sprintf(gCmdStr, "%s/%s %s vhtcfg 2 3 %d 0x%x %s %s", APP_BIN_LOC,MLANUTL,ifname, ap_vht_bw, vhtcapinfo, ap_saved_txsp_stream, ap_saved_rxsp_stream);
        system_with_log(gCmdStr);
        if (!ap_vht_bw) {
            /* enalbe 40M */
            if (ap_ch_width == 40) {
                httxcfg |= 0x62;
                sprintf(gCmdStr, "%s/%s %s httxcfg 0x%x", APP_BIN_LOC,MLANUTL,ifname, httxcfg);
                system_with_log(gCmdStr);
                htcapinfo |= 0x1820000;
                sprintf(gCmdStr, "%s/%s %s htcapinfo 0x%x",APP_BIN_LOC,MLANUTL,
                        ifname, htcapinfo);
                system_with_log(gCmdStr);
            }
        }
    }
    if (set_wireless_params->radio_flag) {
        printf("set_wireless-params->radio:%d\n", set_wireless_params->radio);
        if (set_wireless_params->radio == WFA_ENABLED) {
            sprintf(gCmdStr,"%s/%s bss_start",APP_BIN_LOC,UAPUTL);
			system_with_log(gCmdStr);
        } else {
            sprintf(gCmdStr,"%s/%s bss_stop",APP_BIN_LOC,UAPUTL);
			system_with_log(gCmdStr);
        }
    }
	
	LEAVE( __func__ );
	return TRUE;
}

int mrvlAPResetDefaultVHT(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
	caAPResetDefault_t *apResetDefault = (caAPResetDefault_t *) caCmdBuf;
	char *ifname = apResetDefault->intf;
	dutCmdResponse_t *apResetDefaultResp = &gGenericResp;
	int is_testbed_ap = 0;
	unsigned int vhtcapinfo = 0, httxcfg = 0, htcapinfo = 0;
   
	ENTER( __func__ );	
	strcpy(ifname,mrvl_dut_info->mmh_interface);
	if (apResetDefault->type) {
		printf("%s: type=%s\n", __func__, apResetDefault->type);	
		strcpy(ap_type, apResetDefault->type);
	} else {
		printf("%s: set to default type=Testbed\n", __func__);
		strcpy(ap_type, "Testbed");
	}

	if (strcasecmp(ap_type, "testbed") == 0)
	   is_testbed_ap = 1;
	else
	   is_testbed_ap = 0;

	/* put a program check to set this variable */
	running_vht = 1;
	invalid_for_11ac = 0;

	set_wmm_default(&vi, 2, 3, 2, 47); 
	set_wmm_default(&vo, 3, 4, 2, 94); 
	set_wmm_default(&be, 4, 10, 3, 0); 
	set_wmm_default(&bk, 4, 10, 7, 0); 

	ap_mcs_fixedrate = 0xff;
	ap_saved_txmcsmap = 0x0;
	ap_sgi80_hack = 0; /* reset ap sgi80 hack - need a good fix in firmware*/
	ap_vht_bw = 1;
	ap_ch_width = 80;

	ap_use_hostapd = 0;
	sprintf(gCmdStr, "killall hostapd");
	system_with_log(gCmdStr);

	sprintf(gCmdStr, "sh reload.sh");
	system_with_log(gCmdStr);

	/* default vhtcfg */
	vhtcapinfo = 0x33c179b0; // for dut default value
	sprintf(gCmdStr, "%s/%s %s vhtcfg 2 3 %d 0x%x 0xfffa 0xfffa",APP_BIN_LOC,MLANUTL, ifname, ap_vht_bw, vhtcapinfo);
	system_with_log(gCmdStr);

	/* default httxcfg */
	httxcfg = 0x0;
	sprintf(gCmdStr, "%s/%s %s httxcfg 0x%x",APP_BIN_LOC,MLANUTL, ifname, httxcfg);
	system_with_log(gCmdStr);

	/* default htcapinfo */
	htcapinfo = 0x05820000;
	sprintf(gCmdStr, "%s/%s %s htcapinfo 0x%x",APP_BIN_LOC,MLANUTL, ifname, htcapinfo);
	system_with_log(gCmdStr);

	/** Reset AP settings to default testbed device settings if the device is
	* testbed */
	if (is_testbed_ap) {
	   /* default vhtcfg */
	   vhtcapinfo = 0x33c00000; // for testbed: SGI off, LDPC off, STBC off, SU/MU beamforer/ee off, 2 stream MCS0 - MCS7
	   sprintf(gCmdStr, "%s/%s %s vhtcfg 2 3 %d 0x%x 0xfff0 0xfff0",APP_BIN_LOC,MLANUTL, ifname, ap_vht_bw, vhtcapinfo);
	   system_with_log(gCmdStr);

	   /* degfault httxcfg */
	   httxcfg = get_cmd_str(MLANUTL, "uap0", "httxcfg", "A band", 9);
	   printf("\nAP read httxcfg=0x%08x\n", httxcfg);
	   httxcfg &= ~(1<<0); //disable LDPC
	   sprintf(gCmdStr, "%s/%s %s httxcfg 0x%x",APP_BIN_LOC,MLANUTL, ifname, httxcfg);
	   system_with_log(gCmdStr);

	   /* default htcapinfo */
	   htcapinfo = get_cmd_str(MLANUTL, ifname, "htcapinfo", "A band", 9);
	   printf("AP read htcapinfo=0x%08x\n", htcapinfo);
	   htcapinfo &= ~(1<<26); //disable Rx STBC
	   sprintf(gCmdStr, "%s/%s %s htcapinfo 0x%x",APP_BIN_LOC,MLANUTL, ifname, htcapinfo);
	   system_with_log(gCmdStr);

	   /* default turn off ampdu+amsdu rx */
	   printf("Turning of AMPDU + AMSDU Rx\n");
	   sprintf(gCmdStr,"%s/%s %s addbapara 65535 64 32 0 0",APP_BIN_LOC,MLANUTL, ifname);
	   system_with_log(gCmdStr);
	}	
	LEAVE( __func__ );
	return TRUE;
}

int enable_mlan_tdls_capabilities ( char *ifname ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s %s tdls_config 1",APP_BIN_LOC,MLANUTL,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s %s host_tdls_config config/host_tdls.conf",APP_BIN_LOC,MLANUTL,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int send_mlan_tdls_cs_frame ( char *ifname, char *peer, int channel, int offset, int switchtime ) {
	FILE *tmpfile=NULL;
	ENTER( __func__ );
	
	tmpfile = fopen("tdls.conf","w");
	if ( tmpfile == NULL ) {
		DPRINT_INFO(WFA_OUT,"File creat error !\n");
		return FALSE;
	}	
	fprintf(tmpfile, "tdls_cs_params={\n" );
	fprintf(tmpfile, "\tUnitTime=2\n");
	fprintf(tmpfile, "\tThresholdOtherLink=10\n");
	fprintf(tmpfile, "\tThresholdDirectLink=10\n");
	fprintf(tmpfile, "}\n");
	fclose(tmpfile);
	console_dump_file("tdls.conf");
	sprintf(gCmdStr, "%s/%s %s tdls_cs_params tdls.conf ",APP_BIN_LOC,MLANUTL,ifname);
	system_with_log(gCmdStr);
	
	tmpfile = NULL;
	tmpfile = fopen("tdls.conf","w");
	if (tmpfile == NULL) {
		DPRINT_INFO(WFA_OUT,"File create error !\n");
		return FALSE;
	}   
	fprintf(tmpfile, "tdls_channel_switch={\n" );
	fprintf(tmpfile, "\tPeerMAC=%s\n",peer);
	if(channel >= MIN_A_CHANNEL && channel <= MAX_A_CHANNEL) {
		fprintf(tmpfile, "\tBand=1\n");
	} else {
		fprintf(tmpfile, "\tBand=0\n");
	}
	fprintf(tmpfile, "\tPrimaryChannel=%d\n",channel);
	fprintf(tmpfile, "\tSecondaryChannelOffset=%d\n",offset);
	fprintf(tmpfile, "\tPeriodicity=1\n");
	sprintf(tmpfile, "\tChannelSwitchTime=%d\n",switchtime); 
	sprintf(tmpfile, "\tChannelSwitchTimeOut=100\n"); 
	fprintf(tmpfile,  "}\n");
	fclose(tmpfile); 
	console_dump_file("tdls.conf");
	sprintf(gCmdStr, "%s/%s %s tdls_channel_switch tdls.conf ",APP_BIN_LOC,MLANUTL,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int wfaStaSetRFeatureTDLS(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf) {
	dutCommand_t *dutCmd = (dutCommand_t *)caCmdBuf;	
	caStaRFeat_t *rfeat = (caStaRFeat_t *)caCmdBuf;
	dutCmdResponse_t *caResp = &gGenericResp;
	
	
    char *ifname = dutCmd->intf;
    char strout[256];

    char zero_mac_str[]="000000000000";
    FILE *tmpfile = NULL;
    unsigned short i = 0;
    memset(&strout,0,256);
	strcpy(ifname,mrvl_dut_info->sta_interface);
	ENTER( __func__ );
#define MAX_BG_CHANNEL 14
    printf("Rfeature: prog %d \n",rfeat->prog);
	
	if(rfeat->uapsd == eEnable) {
		printf("Rfeature: TDLS uAPSD ON\n");
		sprintf(strout, "%s", rfeat->peer); 
		if(strncmp(strout, zero_mac_str, sizeof(zero_mac_str))) {
			tmpfile = fopen("tdls.conf", "w");
			if(tmpfile == NULL) {
				return FALSE;
			}
			fprintf(tmpfile, "tdls_powermode={\n" );
			fprintf(tmpfile, "\tPeerMAC=%s\n",strout);
			fprintf(tmpfile, "\tPowerMode=1\n");
			fprintf(tmpfile, "}\n");
			fclose(tmpfile);
			console_dump_file("tdls.conf");	
			sprintf(gCmdStr, "%s/%s %s tdls_powermode tdls.conf ", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
			sprintf(gCmdStr, "iwconfig  %s power on", ifname);
			system_with_log(gCmdStr);    				
		} 
	} else if(rfeat->uapsd == eDisable) {
		printf("Rfeature: TDLS uAPSD OFF\n");
		sprintf(gCmdStr, "iwconfig  %s power off", ifname);
		system_with_log(gCmdStr);
		if(strncmp(strout, zero_mac_str, sizeof(zero_mac_str))) {
			tmpfile = fopen("tdls.conf", "w");
			if(tmpfile == NULL) {
				return FALSE;
			}
			sprintf(strout, "%s", rfeat->peer);
			fprintf(tmpfile, "tdls_powermode={" );
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "PeerMAC=%s",strout);
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "PowerMode=0");
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "}");
			fprintf(tmpfile, "\n" );
			fclose(tmpfile); 
			console_dump_file("tdls.conf");			
			sprintf(gCmdStr, "%s/%s %s tdls_powermode tdls.conf ",APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		}
	} else if(rfeat->tpktimer == eEnable) {
		printf("Rfeature: TDLS TPKTimer ON\n");
		sprintf(gCmdStr,"%s/%s %s tdls_debug ignore_key_expiry 0",APP_BIN_LOC,MLANUTL,ifname);
		system_with_log(gCmdStr);
	} else if(rfeat->tpktimer == eDisable) {
		printf("Rfeature: TDLS TPKTimer OFF\n");
		sprintf(gCmdStr,"%s/%s %s tdls_debug ignore_key_expiry 1",APP_BIN_LOC,MLANUTL,ifname);
		system_with_log(gCmdStr);
	} else if(rfeat->rfeaType.tdls.chsw  == eChanEnable) {
		printf("Rfeature: TDLS Channel switch ON\n");      
		sprintf(strout, "%s", rfeat->peer); 
		if(strncmp(strout, zero_mac_str, sizeof(zero_mac_str))) {
			tmpfile = fopen("tdls.conf", "w");
			if(tmpfile == NULL){
				return FALSE;
			}
			fprintf(tmpfile, "tdls_channel_switch={" );
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "PeerMAC=%s",strout);
			fprintf(tmpfile, "\n" );
			if( rfeat->rfeaType.tdls.offchnum > MAX_BG_CHANNEL ) {
				fprintf(tmpfile, "Band=1");
			}else {
				fprintf(tmpfile, "Band=0");
			}
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "PrimaryChannel=%d", rfeat->rfeaType.tdls.offchnum);
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "SecondaryChannelOffset=%d", rfeat->rfeaType.tdls.secChOffset);
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "ChannelSwitchTime=10");
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "ChannelSwitchTimeout=14");
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "Periodicity=1");
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "RegulatoryClass=%s",ChannelOffsetToRegClass (rfeat->rfeaType.tdls.offchnum ,rfeat->rfeaType.tdls.secChOffset));
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "}");
			fprintf(tmpfile, "\n" );
			fclose(tmpfile);
			console_dump_file("tdls.conf");	
			sprintf(gCmdStr, "%s/%s %s tdls_channel_switch tdls.conf ", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		} 
	} else if(rfeat->rfeaType.tdls.chsw  ==eChanDisable ) {
		printf("Rfeature: TDLS Channel switch OFF\n");
		sprintf(strout, "%s", rfeat->peer); 
		if(strncmp(strout, zero_mac_str, sizeof(zero_mac_str))) {
			tmpfile = fopen("tdls.conf", "w");
			if(tmpfile == NULL) {
				return FALSE;
			}
			fprintf(tmpfile, "tdls_stop_channel_switch={" );
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "PeerMAC=%s",strout);
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "}");
			fclose(tmpfile); 
			console_dump_file("tdls.conf");
			sprintf(gCmdStr, "%s/%s %s tdls_stop_channel_switch tdls.conf ", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
			tmpfile = fopen("tdls.conf", "w");
			if(tmpfile == NULL) {
				return FALSE;
			}
			fprintf(tmpfile, "tdls_cs_params={" );
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "UnitTime=2");
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "ThresholdOtherLink=10");
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "ThresholdDirectLink=0");
			fprintf(tmpfile, "\n" );
			fprintf(tmpfile, "}");
			fprintf(tmpfile, "\n" );
			fclose(tmpfile);
			console_dump_file("tdls.conf");
			sprintf(gCmdStr, "%s/%s %s tdls_cs_params tdls.conf ", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
			sprintf(gCmdStr, "%s/%s %s tdls_disable_cs 0", APP_BIN_LOC,MLANUTL,ifname);
			system_with_log(gCmdStr);
		} 
	} else if(rfeat->rfeaType.tdls.chsw  == eRejectReq) {
		sprintf(gCmdStr, "%s/%s %s tdls_disable_cs 1", APP_BIN_LOC,MLANUTL,ifname);
		system_with_log(gCmdStr);
	} else if(rfeat->rfeaType.tdls.chsw  == eUnSolResp) {
		sprintf(strout, "%s", rfeat->peer); 
		tmpfile = fopen("tdls.conf", "w");
		if(tmpfile == NULL){
			return FALSE;
		}
		fprintf(tmpfile, "tdls_cs_params={" );
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "UnitTime=2");
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "ThresholdOtherLink=10");
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "ThresholdDirectLink=0");
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "}");
		fprintf(tmpfile, "\n" );
		fclose(tmpfile);
		console_dump_file("tdls.conf");	
		sprintf(gCmdStr, "%s/%s %s tdls_cs_params tdls.conf ", APP_BIN_LOC,MLANUTL,ifname);
		system_with_log(gCmdStr);		
		sprintf(gCmdStr, "%s/%s %s tdls_debug im_return 1 ", APP_BIN_LOC,MLANUTL,ifname);
		system_with_log(gCmdStr);		
		tmpfile = fopen("tdls.conf", "w");
		if(tmpfile == NULL){
			return FALSE;
		}
		fprintf(tmpfile, "tdls_channel_switch={" );
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "PeerMAC=%s",strout);
		fprintf(tmpfile, "\n" );
		if( rfeat->rfeaType.tdls.offchnum > MAX_BG_CHANNEL ) {
			fprintf(tmpfile, "Band=1");
		} else {
			fprintf(tmpfile, "Band=0");
		}
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "PrimaryChannel=%d", rfeat->rfeaType.tdls.offchnum);
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "SecondaryChannelOffset=%d", rfeat->rfeaType.tdls.secChOffset);
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "ChannelSwitchTime=10");
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "ChannelSwitchTimeout=14");
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, " Periodicity=1");
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "RegulatoryClass=%s",ChannelOffsetToRegClass (rfeat->rfeaType.tdls.offchnum ,rfeat->rfeaType.tdls.secChOffset));
		fprintf(tmpfile, "\n" );
		fprintf(tmpfile, "}");
		fprintf(tmpfile, "\n" );
		fclose(tmpfile); 
		console_dump_file("tdls.conf");
		sprintf(gCmdStr, "%s/%s %s tdls_channel_switch tdls.conf ", APP_BIN_LOC,MLANUTL,ifname);
		system_with_log(gCmdStr);
	}
	LEAVE( __func__ );
	return TRUE;
}

int reset_ip_tables ( char *ifname ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s iptables -F INPUT",DUT_CMD_PREFIX);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s ip nei flush dev %s",DUT_CMD_PREFIX,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s ifconfig %s 0.0.0.0",DUT_CMD_PREFIX,ifname);
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int invoke_dhcp_client ( char *ifname, int type  ) {
	ENTER( __func__ );

	if ( type == 1 ) { /** IPv4 */	
#ifdef MV_PLATFORM_ANDROID 
		sprintf(gCmdStr, "netcfg %s dhcp",ifname);
		system_with_log(gCmdStr);	
#else 
		sprintf(gCmdStr, "killall udhcpc");
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "udhcpc -i %s",ifname);
		system_with_log(gCmdStr);	
#endif
	} 
	
	if ( type == 2 ) { /** IPv6 */
#ifdef MV_PLATFORM_ANDROID
		/** TODO */
		
#else 
		sprintf(gCmdStr, "sysctl -w net.ipv6.conf.%s.autoconf=1",ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "sysctl -w net.ipv6.conf.%s.accept_ra=1",ifname);
		system_with_log(gCmdStr);
#endif 
	}
	LEAVE( __func__ );
	return TRUE;
}

int mlan_network_scan ( char *ifname) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s %s setuserscan",APP_BIN_LOC,MLANUTL,mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int send_arp_ping ( char *ifname, char *srcip, char *dest, char *destip) {
	ENTER( __func__ );
	sprintf(gCmdStr, "arping  -i %s -H -S %s -t %s  %s -c 5 -D > /tmp/pingResult.txt ",ifname,srcip,dest,destip);
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int set_noa_interval (char *ifname, int noa_count, int noa_duration, int noa_interval) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s %s cfg_noa 1 0 %d %d %d",APP_BIN_LOC,MLANUTL,ifname,noa_count,noa_duration,noa_interval);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s -i %s pscfg 2",APP_BIN_LOC,UAPUTL,ifname);
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int fetch_ipv6_info ( char *ifname, char *ipv6addr ) {
	FILE *tmpfd;
	char string[256];
	char *str;
	ENTER( __func__ );

	sprintf(gCmdStr,"%s/getipconfig.sh",APP_BIN_LOC);
	if(access(gCmdStr, F_OK) == -1)
		return FALSE;	        
	sprintf(gCmdStr, "%s sh %s/getipconfig.sh /tmp/ipconfig.txt %s\n",DUT_CMD_PREFIX,APP_BIN_LOC,mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);

	tmpfd = fopen("/tmp/ipconfig.txt", "r+");
	if(tmpfd == NULL)
		return FALSE;
	
	for(;;)
	{
		if(fgets(string, 256, tmpfd) == NULL)
			break;
		if(strncmp(string, "ipv6addr", 6) == 0)
		{
			str = strtok(string, "=");
			str = strtok(NULL, " ");
			if(str != NULL)
			{
			   wSTRNCPY(ipv6addr, str, WFA_IP_V6_ADDR_STR_LEN);
			   ipv6addr[40]='\0';		 
			}
		}
	}
	fclose(tmpfd);
	LEAVE( __func__ );
	return TRUE;
}

int reassign_ipv6_addr ( char *ifname, char *ipv6addr) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s ifconfig %s inet6 del %s\n",DUT_CMD_PREFIX,ifname,ipv6addr);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s ifconfig %s inet6 add %s\n",DUT_CMD_PREFIX,ifname,ipv6addr);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int fetch_ipv4_info ( char *ifname, char *ipv4addr ) {
	FILE *tmpfd;
	char string[256];
	char *str;
	ENTER( __func__ );
					
	sprintf(gCmdStr,"%s/getipconfig.sh",APP_BIN_LOC);
	if(access(gCmdStr, F_OK) == -1)
		return FALSE;
	sprintf(gCmdStr, "%s sh %s/getipconfig.sh /tmp/ipconfig.txt %s\n",DUT_CMD_PREFIX,APP_BIN_LOC,ifname);
	system_with_log(gCmdStr);

	tmpfd = fopen("/tmp/ipconfig.txt", "r+");
	if(tmpfd == NULL)
		return FALSE;

	for(;;)
	{	
		if(fgets(string, 256, tmpfd) == NULL)
			break;
		if(strncmp(string, "ipaddr", 6) == 0) {
			str = strtok(string, "=");
			str = strtok(NULL, " ");
			if(str != NULL && str[0] < '9' && str[0] > '0') {
				wSTRNCPY(ipv4addr, str, 15);
				int length = strlen(ipv4addr);
				if(ipv4addr[length - 1] == '\n') {
					ipv4addr[length - 1] = '\0';
				} 
			}
		}
	}
	fclose(tmpfd);
	LEAVE( __func__ );
	return TRUE;
}

int configure_mlan_ageout_timer (char *ifname, int timer) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s sys_cfg_sta_ageout_timer %d",APP_BIN_LOC,UAPUTL,ifname,timer);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s sys_cfg_ps_sta_ageout_timer %d",APP_BIN_LOC,UAPUTL,ifname,timer);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "iwpriv %s htstreamcfg 0x11",ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "iwpriv %s antcfg 1 1",ifname);
	system_with_log(gCmdStr);		
	LEAVE( __func__ );
	return TRUE;
}

int configure_pmf_capabilities ( char *ifname, char *pmf_type) {
	ENTER( __func__ );	
	if ( !strcasecmp(pmf_type,"required") ) {
		sprintf(gCmdStr, "%s/%s bss_stop",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s sys_cfg_pmf 1 1",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s bss_start",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);		
	} else if ( !strcasecmp(pmf_type,"optional") ) {
		sprintf(gCmdStr, "%s/%s bss_stop",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s sys_cfg_pmf 1 0", APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s bss_start",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	} else if ( !strcasecmp(pmf_type,"disabled") ) {
		sprintf(gCmdStr, "%s/%s bss_stop",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s sys_cfg_pmf 0 0", APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s bss_start",APP_BIN_LOC,UAPUTL); 
		system_with_log(gCmdStr);
	} else {
		/** Do nothing */
	}
	LEAVE( __func__ );
	return TRUE;
}
