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
#include "mrvl_miscs.h"
#include "mrvl_macros.h"
#include "mrvl_gvars.h"

/**
 *  @brief convert char to hex integer
 *
 *  @param chr      char
 *  @return         hex integer
 */
char hexc2bin(char chr)
{
    if (chr >= '0' && chr <= '9')
        chr -= '0';
    else if (chr >= 'A' && chr <= 'F')
        chr -= ('A' - 10);
    else if (chr >= 'a' && chr <= 'f')
        chr -= ('a' - 10);

    return chr;
}

/**
 *  @brief convert string to hex integer
 *
 *  @param s        A pointer string buffer
 *  @return         hex integer
 */
unsigned int a2hex(char *s)
{
   unsigned int val = 0;
    if (!strncasecmp("0x", s, 2)) {
        s += 2;
    }

    while (*s && isxdigit(*s)) {
        val = (val << 4) + hexc2bin(*s++);
    }

    return val;
}

unsigned int get_bw_info(char *ifname, int field) {
    FILE *pipe = NULL;
    char tmpStr[100], cmdStr[255] = {0};
    unsigned int htinfo = 0;
    sprintf(cmdStr, "%s/%s %s vhtcfg 2 1 | grep -i \"BW\" | cut -d' ' -f %d", APP_BIN_LOC,MLANUTL,ifname,field);
    printf("\n=> %s\n", cmdStr);
    pipe = popen(cmdStr, "r");
    fgets(tmpStr, 100, pipe);
    printf("==> ---%s----\n", tmpStr);
    pclose(pipe);
    if (tmpStr[strlen(tmpStr) - 1] == '\n')
        tmpStr[strlen(tmpStr) - 1] = '\0';
    if (!strcasecmp(tmpStr, "11n"))
        return 0;
    else if (!strcasecmp(tmpStr, "VHT"))
        return 1;
    else
        return -1;
}

int change_wifidirect_conf(char *filepath , char *label , char * string )
{

	FILE *fp1 , *fp2 ;                       
	char buffer[500],ch,t,temp[500];
	int i,j,flag=0;
	fp1 = fopen(filepath,"r");
	fp2=  fopen("temp.txt" , "w" );
	if( fp2 == NULL )
	{
		printf("\n Error in creating temporary file \n");
		exit(0);                        
	}
	while( ch != EOF )
	{
		ch = fgetc(fp1);        
		i=0;
		while(  (  ch >=65 && ch <=90 ) || ( ch >=97 && ch <= 122 )   )
		{                     
			buffer[i]=ch;
			ch=fgetc(fp1);
			i++;                             
		}
		buffer[i]='\0'; 
		i=0;j=0;
		while( buffer[i] != '\0'  )
		{
			t= buffer[i]; 
			if( ( t >= 65 && t <= 90 )  || ( t >= 97 && t <= 122 ) )
			{
				temp[j]=t;
				j++;
			}
			i++;
		}
		temp[j]='\0';
		if( !strcmp(temp,label )  ) {
			fprintf(fp2,"%s",buffer);
			fprintf(fp2,"%c",ch);
			fprintf(fp2,"%s\n",string);

			ch = fgetc(fp1);
			while( ch != ' ' && ch != '\n' && ch != EOF )
				ch = fgetc(fp1);  
		} else {
			fprintf(fp2,"%s",buffer);
			if( ch != EOF  )
				fprintf(fp2,"%c",ch);
		}                                                              
	}

	fclose(fp1);
	fclose(fp2);                                                     
	sprintf(buffer,"rm -f %s",filepath);
	system(buffer);
	sprintf(buffer,"cp temp.txt %s",filepath);
	system(buffer);
	sprintf(buffer,"rm -f temp.txt");
	system(buffer);                                          
}

unsigned int get_cmd_str(char *cmd, char *ifname, char *str, char *filter_str,
                        int field_pos)
{
    FILE *pipe = NULL;
    char tmpStr[100], cmdStr[255] = {0};
    unsigned int htinfo = 0;

    //memset(g_cmd_str_ret, 0, sizeof(g_cmd_str_ret));
    sprintf(cmdStr, "%s/%s %s %s | grep -i \"%s\" | cut -d' ' -f %d",
        APP_BIN_LOC,cmd, ifname, str, filter_str, field_pos);
    printf("\n=> %s\n", cmdStr);
    pipe = popen(cmdStr, "r");
    fgets(tmpStr, 100, pipe);
    printf("==> %s\n", tmpStr);
    pclose(pipe);
    if (tmpStr[strlen(tmpStr)] == '\n')
        tmpStr[strlen(tmpStr)] = '\0';
    htinfo = a2hex(tmpStr);
    //strncpy(g_cmd_str_ret, tmpStr, sizeof(tmpStr));
	strcpy(g_cmd_str_ret, tmpStr);	
    printf("\n==> Get %s: 0x%x\n", str, htinfo);
    return htinfo;
}

void timestamp(char *tstamp)
{
	struct tm *tmp;
	time_t t;
	t = time(NULL);
	tmp = localtime(&t);
	sprintf(tstamp, "%02d.%02d.%04d %02d:%02d:%02d",
	tmp->tm_mday, tmp->tm_mon+1, tmp->tm_year+1900,
	tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}

void update_splChar_ssid ( char *ssid ) { 
 	char tbuff[64];
	sprintf(tbuff,"%s", SUPPLICANT_CONF);

	FILE *fp; 
	fp = fopen(tbuff,"a+");
	if (fp)
	     fprintf(fp,"ssid=\"%s\"\n",ssid);

	fclose(fp);	
}


int system_with_log(char *cmd) {
	FILE *fp;
	int cmd_status;
	char tstamp[100] = {0};
	fp = fopen("cmdhistory", "a+");
	timestamp(tstamp);
	
	if (mrvl_debug_info->console_logging) {
		DPRINT_INFO(WFA_OUT,"[%s][CMD]%s\n",tstamp,cmd);
	}
	
	cmd_status = system(cmd);
	fprintf(fp,"%s\t%d\t%s\n", tstamp, cmd_status, cmd);
	fclose(fp);
  
    /** Rotate the log file 
       Maintain history of last 100 commands */
	
	#ifdef MV_PLATFORM_ANDROID        
	system("busybox tail -100 cmdhistory > temp; mv temp cmdhistory");
	#else
	system("tail -100 cmdhistory > temp; mv temp cmdhistory -f");	
	#endif		
	return cmd_status;
}

int read_role()
{
    char string[10] = "";
    int i=0, retval;

    while (i < 2) {
        if (read_config("wfd_init_temp.conf",
				"GO_NEG_STATUS", string) == FALSE) {
            printf ("file open failed - try again\n");
            sleep (5);
            i++;
            continue;
        } 
		if (!strcmp(string, "")) {
			printf("No GO_NEG_STATUS specified yet - try again\n");
			sleep (5);
			i++;
		} else {
			break;
		}             
    }

	printf("\nIn wfd_init.conf, GO_NEG_STATUS is (%s)\n", string);
    if (strncmp(string,"CLIENT",6) == 0)  retval = 1;
    else if (strncmp(string,"GO",2) == 0) retval = 2;
    else retval = 0;

    return retval;
}

char* ChannelOffsetToRegClass (int primaryChannel, int secChannelOffset) 
{

    if(secChannelOffset == 0) {
        switch(primaryChannel) {
        case 36:
        case 40:
        case 44:
        case 48:
            return "1";
            break;
        case 52:     
        case 56:     
        case 60:     
        case 64:
            return "2";
            break;
        case 149:     
        case 153:     
        case 157:     
        case 161:
            return "3";
            break;
        case 100:     
        case 104:     
        case 108:     
        case 112:
        case 116:     
        case 120:     
        case 124:     
        case 128:
        case 132:     
        case 136:     
        case 140:     
            return "4";
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return "12";
            break;
        }
    } 
    else {
        switch(primaryChannel) {
        case 36:
        case 44:
            return "22";
            break;
        case 52:     
        case 60:     
            return "23";
            break;
        case 100:     
        case 108:     
        case 116:     
        case 124:     
        case 132:     
            return "24";
            break;
        case 149:     
        case 157:     
            return "25";
            break;
        case 40:
        case 48:
            return "27";
            break;
        case 56:     
        case 64:     
            return "28";
            break;
        case 104:     
        case 112:     
        case 120:     
        case 128:     
        case 136:     
            return "29";
            break; 
        case 153:     
        case 161:     
            return "30";
            break; 
        case 1:
        case 2:
        case 3:
        case 4:
            return "32";
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return "33";
            break;
        }

    }
}

int run_iwconfig(char *ifname, char *ssid, char *bssid)
{ 
	int ret, slen;
	char string[128];
	FILE *tmpfd = NULL;	

        /*get ssid*/
	memset(string, 0, sizeof(string));
 	sprintf(gCmdStr, "cat %s | grep essid | cut -f2 -d\" > /tmp/essid.txt\n",mrvl_dut_info->mwlan_info_path);
	system_with_log(gCmdStr);
	tmpfd = fopen("/tmp/essid.txt", "r+");
	if(tmpfd == NULL)
	{
		printf ("\n===== unable to open file essid.txt =====\n");
		return FALSE;
	}
	if(fgets(string, 256, tmpfd) == NULL)
		return FALSE;
	strcpy(ssid, string);
	slen = strlen(ssid);
	if (slen)
		ssid[slen-1] = '\0';
	else
	{
		printf("\n=============ssid is NULL, not connected===========\n");
		goto out;
	}
	fclose(tmpfd);
        tmpfd = NULL;

        /*get bssid*/
	memset(string, 0, sizeof(string));
 	sprintf(gCmdStr, "cat %s | grep bssid | cut -f2 -d\" > /tmp/bssid.txt\n",mrvl_dut_info->mwlan_info_path);
	system_with_log(gCmdStr);
	tmpfd = fopen("/tmp/bssid.txt", "r+");
	if(tmpfd == NULL)
	{
		printf ("\n===== unable to open file bssid.txt =====\n");
		return FALSE;
	}
	if(fgets(string, 256, tmpfd) == NULL)
		return FALSE;
	strcpy(bssid, string);
	slen = strlen(bssid);
	bssid[slen-1] = '\0';
	fclose(tmpfd);
out:
	printf("\n***** ssid = %s, bssid = %s\n", ssid, bssid);
	if (strcmp(ssid, "") && strcmp(bssid, ""))
		return TRUE;
	else 
		return FALSE;
}

 void edit_config(char *fileName, char *currLabel, char *newLabel, char *value)
{
    char cmdStr[200];

    printf ("\nfileName=%s\ncurrLabel=%s\nnewLabel=%s\nvalue=%s\n",fileName, currLabel, newLabel, value);
    
    if (strcmp(newLabel, "")) {
        sprintf(cmdStr, "sed \"s/^%s[ \t]*.*/%s=/g\" %s > temp.conf", currLabel, newLabel, fileName);
        system_with_log(cmdStr);
        sprintf(cmdStr, "sed \"s/^%s=/%s=%s/g\" temp.conf > %s", currLabel, newLabel, value, fileName);
        system_with_log(cmdStr);
    } else {
        sprintf(cmdStr, "sed \"s/^%s[ \t]*.*/%s/g\" %s > temp.conf", currLabel, newLabel, fileName);
        system_with_log(cmdStr);
        sprintf(cmdStr, "cp -f temp.conf %s", fileName);
        system_with_log(cmdStr);
    }
}


 void edit_config_1(char *fileName, char *currLabel, char *newLabel, char *value)
{
    char cmdStr[200];

    printf ("\nfileName=%s\ncurrLabel=%s\nnewLabel=%s\nvalue=%s\n",fileName, currLabel, newLabel, value);
    
    if (strcmp(newLabel, "")) {
        sprintf(cmdStr, "sed \"s/^%s[ \t]*.*/%s=/g\" %s > temp.conf", currLabel, newLabel, fileName);
        system_with_log(cmdStr);
        sprintf(cmdStr, "sed \"s/^%s=/%s=\\\"%s\\\"/g\" temp.conf > %s", currLabel, newLabel, value, fileName);
        system_with_log(cmdStr);
    } else {
        sprintf(cmdStr, "sed \"s/^%s[ \t]*.*/%s/g\" %s > temp.conf", currLabel, newLabel, fileName);
        system_with_log(cmdStr);
        sprintf(cmdStr, "cp -f temp.conf %s", fileName);
        system_with_log(cmdStr);
    }
}

int read_config (char *fileName, char *label, char *value)
{
    char *str;
    FILE *tmpfd;
    char string[256];

    tmpfd = fopen(fileName, "r");
    if(tmpfd == NULL)
    {
		printf("file open failed\n%d", __LINE__);
        return FALSE;
    }

    value[0] = '\0';

    for(;;)
    {
        if(fgets(string, 256, tmpfd) == NULL)
           break; 

        /* check label */
        if(strncmp(string, label, strlen(label)) ==0)
        {
            str = strtok(string, "=");
            str = strtok(NULL, "=");
            if(str != NULL)
                strcpy(value, str);
            else
                strcpy(value, "");

            if (value[strlen(value)-1] == '\n') {
                value[strlen(value)-1] = '\0';
            }
	    break;
        }
    }

    fclose(tmpfd);

    return TRUE;
}

int chk_ret_status()
{
    char *ret = getenv(WFA_RET_ENV);

    if(*ret == '1')
       return WFA_SUCCESS;
    else
       return WFA_FAILURE;
}

int useOpenSupplicant()
{
	int retVal;
    char *retStr;
	retStr = getenv("MV_USE_SUPPLICANT");
	if (retStr == NULL)
		 retVal = 0;
	else 
		retVal = atoi(retStr);
	printf("ENV(MV_USE_SUPPLICANT): %d\n", retVal);
	if (retVal == 1) 
		return 1;
	else
		return 0;
}

char * update_GrpCipher ( char *cipher , char *ssid ) {

   return cipher;
  
#if 0   
   char buffer[128],ch, *retStr;	
   char gCipher[64]= "NULL" ;	  
   int i,retVal;
   FILE *fp;
   
   retStr = getenv("MV_SCAN_NETWORK");
   if (retStr == NULL)
		 retVal = 0;
   else 
       retVal = atoi(retStr);
   
   printf("ENV(MV_SCAN_NETWORK): %d\n", retVal);
   if ( retVal == 1 ) {
		sprintf(buffer,"iwlist %s scan > scan.log",sta_interface);
   		system(buffer);	
   		fp = fopen("scan.log","r");
		
   		do {
      		ch = fgetc(fp);
      		i=0;
      		if ( ch == '"' ) {
				while( (ch = fgetc(fp)) != '"' )
					buffer[i++]=ch; 
				buffer[i]='\0';				
				if ( !strcmp(buffer,ssid) ) {  	
		    		while ( fgets ( buffer, 128 , fp )) {
						if ( strstr(buffer,"Cell") != NULL )
			    			break;
						if ( strstr(buffer,"Group Cipher :") != NULL  ) {
			    			i=0;
			    			while ( buffer[i++] != ':' );
							strcpy(gCipher,buffer + ++i);
							if ( gCipher[ strlen(gCipher) -1 ] == '\n' )
				    			gCipher[ strlen(gCipher) -1 ] = '\0';					
						}
			   		}	 		
				} 		
		    }	
		} while ( ch != EOF && ch != 255 );   
   		
		fclose(fp);
   		if ( !strcmp(gCipher,"NULL") ) {
			return cipher;
   		} else {				
			memcpy(grpCipher[2],gCipher,32);		
			return grpCipher[2];
	   	}	
	} else {
		return cipher;
	}
#endif
	
}

int wfaExecuteCLI(char *CLI)
{
   char *retstr;
   system_with_log(CLI);

   retstr = getenv("WFA_CLI_STATUS");
   printf("cli status %s\n", retstr);
   return atoi(retstr);
}

int read_ini_config (char *fileName, char *header, char *label, char *value)
{
    char *str;
    FILE *tmpfd;
    char string[256];
    char *saveptr;	
    char header_filter[256];
    tmpfd = fopen(fileName, "r");
    if(tmpfd == NULL)
    {
	printf("File open failed\n%d", __LINE__);
        return FALSE;
    }
    value[0] = '\0';
    sprintf(header_filter,"[%s]",header);
	
    for(;;)
    {
        if(fgets(string, 256, tmpfd) == NULL)
           break; 		
		if(strncmp(string, header_filter, strlen(header_filter)) ==0)
		{
			for(;;) {
				if(fgets(string, 256, tmpfd) == NULL)
            		break; 		
            	if (strstr(string,"[") && strstr(string,"]"))
					break;				
				
				if(strncmp(string, label, strlen(label)) ==0)
		        {
		            str = strtok_r(string, "=",&saveptr); 
		            if(str != NULL)
		                strcpy(value, saveptr);
		            else
		                strcpy(value, "");
		            if (value[strlen(value)-1] == '\n') {
		                value[strlen(value)-1] = '\0';
		            }
			    	break;
		        }	
            }
		}
    }
    fclose(tmpfd);
    return TRUE;
}

void set_wmm_default(struct wmm_ac *q, int a, int b, int c, int d) {
   q->cwmin = a;
   q->cwmin_flag = 0;
   q->cwmax = b;
   q->cwmax_flag = 0;
   q->aifs = c;
   q->aifs_flag = 0;
   q->txop = d;
   q->txop_flag = 0;
}    


int change_supplicant_conf(char *file , char *label , char * string )
{
	FILE *fp1 , *fp2 ;
	char buffer[500],t,temp[500];
	int i,j,ch = 0 , flag=0;
	char filepath[255] = {0};
	ENTER( __func__ );
	
	sprintf(filepath, "%s/%s", APP_BIN_LOC, file);
	fp1 = fopen(filepath,"r");
	fp2=  fopen("temp.txt" , "w" );
	if( fp2 == NULL )
	{
		printf("Error in creating temporary file \n");
		exit(0);
	}
	while( ch != EOF )
	{
		ch = fgetc(fp1);
		i=0;
		while(  (  ch >=65 && ch <=90 ) || ( ch >=97 && ch <= 122 ) || ( ch>=48 && ch<=57 ) || (ch == 95)   )
		{
			buffer[i]=ch;
			ch=fgetc(fp1);
			i++;
		}
		buffer[i]='\0';
		i=0;j=0;
		while( buffer[i] != '\0'  )
		{
			t= buffer[i];
			if( ( t >= 65 && t <= 90 )  || ( t >= 97 && t <= 122 ) || ( t>=48 && t<=57 ) || (t == 95) )
			{
				temp[j]=t;
				j++;
			}
			i++;
		}
		temp[j]='\0';
		if( !strcmp(temp,label )  ) {
			fprintf(fp2,"%s",buffer);
			fprintf(fp2,"%c",ch);
			fprintf(fp2,"%s\n",string);
			ch = fgetc(fp1);
			while( ch != ' ' && ch != '\n' && ch != EOF )
				ch = fgetc(fp1);
		} else {
			fprintf(fp2,"%s",buffer);
			if( ch != EOF  )
				fprintf(fp2,"%c",ch);
		}
	}
	fclose(fp1);
	fclose(fp2);
	sprintf(buffer,"%s rm -f %s", DUT_CMD_PREFIX, filepath);
	system(buffer);
	sprintf(buffer,"cp temp.txt %s",filepath);
	system(buffer);
	sprintf(buffer,"%s rm -f temp.txt", DUT_CMD_PREFIX);
	system(buffer);
	LEAVE( __func__ );
	return TRUE;
}



int sigma_health_check() {
	int ret;
	char version[128] = {0} ;
	char kernel[128] = {0};
	
	ENTER( __func__ );
	/** Load WLAN Build */
	system_with_log(mrvl_dut_info->build_unload);
	system_with_log(mrvl_dut_info->build_load);
	
	#if 0	
	ret = fetch_mac_address(mrvl_dut_info->p2p_interface,mrvl_dut_info->p2p_dev_address);
	if ( ret == FALSE ) {
		DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
		DPRINT_INFO(WFA_OUT,"1. Unable to Fetch '%s' mac address. Please check WLAN driver status\n",mrvl_dut_info->p2p_interface);
		DPRINT_INFO(WFA_OUT,"2. Please check load_build/unload_build parameters in '%s' file...\n",SIGMA_USER_CONFIG);
		return FALSE;
	}
	ret = fetch_mac_address(mrvl_dut_info->sta_interface,mrvl_dut_info->sta_dev_address);
	if ( ret == FALSE ) {
		DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
		DPRINT_INFO(WFA_OUT,"1. ERROR: Unable to Fetch '%s' mac address. Please check WLAN driver status\n",mrvl_dut_info->sta_interface);
		DPRINT_INFO(WFA_OUT,"2. Please check load_build/unload_build parameters in '%s' file...\n",SIGMA_USER_CONFIG);		
		return FALSE;
	}
	
	/** Check for Build utility */
	ret = access(MLANUTL, F_OK);	
	if ( ret == FALSE ) {
		DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
		DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' utility in current directory\n",MLANUTL);
		return FALSE;
	}
	ret = access(UAPUTL, F_OK);	
	if ( ret == FALSE ) {
		DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
		DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' utility  in current directory\n",UAPUTL);
		return FALSE;
	}
	#endif 
	
	if ( !HOSTAPD && mrvl_dut_info->use_supplicant ) {
		ret = access(mrvl_WS_info->supplicant_bin, F_OK);	
		if ( ret == FALSE ) {
			DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
			DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' utility. Please copy this utility in current directory\n",mrvl_WS_info->supplicant_bin);
			return FALSE;
		}
		
		ret = access(mrvl_WS_info->supplicant_cli_bin, F_OK);	
		if ( ret == FALSE ) {
			DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
			DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' utility. Please copy this utility in current directory\n",mrvl_WS_info->supplicant_cli_bin);
			return FALSE;
		}	
	}

	if (HOSTAPD) {
		ret = access(mrvl_hostapd_info->hostapd_bin, F_OK);	
		if ( ret == FALSE ) {
			DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
			DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' utility. Please copy this utility in current directory\n",mrvl_hostapd_info->hostapd_bin);
			return FALSE;
		}
		
		ret = access(mrvl_hostapd_info->hostapd_cli_bin, F_OK);	
		if ( ret == FALSE ) {
			DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
			DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' utility. Please copy this utility in current directory\n",mrvl_hostapd_info->hostapd_cli_bin);
			return FALSE;
		}		
	}
	
	if ( !strcasecmp(SIGMA_PROG_NAME,MRVL_HS2_PROG) ) {
		/** TODO Health check for HS2 */
		
	}
	
	if ( !strcasecmp(SIGMA_PROG_NAME,MRVL_P2P_PROG) || !strcasecmp(SIGMA_PROG_NAME,MRVL_WFD_PROG) ) {
		if ( !strcasecmp(SIGMA_PROG_UTILITY,MWUD) ) {
			ret = access(mrvl_mwud_info->mwu_bin, F_OK);	
			if ( ret == FALSE ) {
				DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
				DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' build. Please copy '%s' build in current directory\n",mrvl_mwud_info->mwu_bin,mrvl_mwud_info->mwu_bin);
				return FALSE;
			}
			
			ret = access(mrvl_mwud_info->mwu_conf_wifidir, F_OK);	
			if ( ret == FALSE ) {
				DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
				DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' file. Please copy '%s' file in current directory\n",mrvl_mwud_info->mwu_conf_wifidir,mrvl_mwud_info->mwu_conf_wifidir);
				return FALSE;
			}
			
			ret = access(mrvl_mwud_info->mwu_conf_wifidirect_defaults, F_OK);	
			if ( ret == FALSE ) {
				DPRINT_INFO(WFA_OUT,"\n| ERROR REPORT |\n");
				DPRINT_INFO(WFA_OUT,"1. Unable to find '%s' file. Please copy '%s' file in current directory\n",mrvl_mwud_info->mwu_conf_wifidirect_defaults,mrvl_mwud_info->mwu_conf_wifidirect_defaults);
				return FALSE;
			}
		}
	}
	#if 0
	ret = get_mlan_driver_version(mrvl_dut_info->sta_interface,version);
	ret = get_kernal_release(kernel);
	DPRINT_INFO(WFA_OUT,"\n-----------------------------------------------------------------------------\n");
	sprintf(gCmdStr," %-25s: %s","Sigma Version",M_VERSION);
	DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);
	sprintf(gCmdStr," %-25s: %s","Test Mode",SIGMA_PROG_NAME);
	DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);	
	if ( !strcasecmp(SIGMA_PROG_NAME,MRVL_P2P_PROG) || !strcasecmp(SIGMA_PROG_NAME,MRVL_WFD_PROG)  ) {
		sprintf(gCmdStr," %-25s: %s","Wireless Utility",SIGMA_PROG_UTILITY);		
		DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);
	}	
	sprintf(gCmdStr," %-25s: %s","WLAN Build",version);		
	DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);	
	sprintf(gCmdStr," %-25s: %s \(%s\)","STA Interface",mrvl_dut_info->sta_interface,mrvl_dut_info->sta_dev_address);
	DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);
	sprintf(gCmdStr," %-25s: %s \(%s\)","P2P Interface",mrvl_dut_info->p2p_interface,mrvl_dut_info->p2p_dev_address);
	DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);
	sprintf(gCmdStr," %-25s: %d \(0 = NO|1 = YES\)","Use Supplicant",mrvl_dut_info->use_supplicant);
	DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);
	sprintf(gCmdStr," %-25s: %s","Kernel Release",kernel);
	DPRINT_INFO(WFA_OUT,"|%-75s|\n",gCmdStr);
	DPRINT_INFO(WFA_OUT,"-----------------------------------------------------------------------------\n");
	#endif 
	LEAVE( __func__ );
	return TRUE;
}

int read_sigma_configuration_file() {
	int ret; 
	char temp[256];
	
	ENTER( __func__ );
	ret = access(SIGMA_USER_CONFIG, F_OK);
	if (ret == -1) {
		LEAVE( __func__ );
		return FALSE;
	}
	
	mrvl_dut_info = (struct mrvl_sigma_dut_info*) malloc(sizeof(struct mrvl_sigma_dut_info));
	mrvl_WS_info = (struct mrvl_sigma_WS_info*) malloc(sizeof(struct mrvl_sigma_WS_info));	
	mrvl_dhcp_info = (struct mrvl_sigma_dhcp_info*) malloc(sizeof(struct mrvl_sigma_dhcp_info));
	mrvl_mwud_info = (struct mrvl_sigma_mwud_info*) malloc(sizeof(struct mrvl_sigma_mwud_info));
	mrvl_hostapd_info = (struct mrvl_sigma_hostapd_info*) malloc(sizeof(struct mrvl_sigma_hostapd_info));
	mrvl_debug_info = (struct mrvl_sigma_debug_info*) malloc(sizeof(struct mrvl_sigma_debug_info));
	runtime_test_data = (struct sigma_runtime_test_data*) malloc(sizeof(struct sigma_runtime_test_data));
	
	/** DUT_Settings */	
	read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","sta_interface",mrvl_dut_info->sta_interface);
	read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","p2p_interface",mrvl_dut_info->p2p_interface);
	read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","mmh_interface",mrvl_dut_info->mmh_interface);
    read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","useOpenSupplicant",temp);
	mrvl_dut_info->use_supplicant = atoi(temp);
	read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","load_build",mrvl_dut_info->build_load);
    read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","unload_build",mrvl_dut_info->build_unload);
    read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","config_methods",mrvl_dut_info->config_methods);
    read_ini_config(SIGMA_USER_CONFIG,"DUT_Settings","mwlan_info_path",mrvl_dut_info->mwlan_info_path);
		
	/** Wpa_Supplicant */		
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","supplicant_bin",mrvl_WS_info->supplicant_bin);
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","supplicant_cli_bin",mrvl_WS_info->supplicant_cli_bin);
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","supplicant_ext",mrvl_WS_info->supplicant_ext);
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","supplicant_conf",mrvl_WS_info->supplicant_conf);
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","p2p_ctrl_interface",mrvl_WS_info->p2p_ctrl_interface);
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","sta_ctrl_interface",mrvl_WS_info->sta_ctrl_interface);
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","p2p_virtual_interface",mrvl_WS_info->p2p_virtual_interface);

	mrvl_WS_info->network_index = 0;
	mrvl_WS_info->credential_index = 0;
	mrvl_WS_info->supplicant_config_approach = CONF_FILE_APPROACH;

	strcpy(mrvl_WS_info->p2p_interface,mrvl_dut_info->p2p_interface);

	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_P2P_PROG))
		mrvl_WS_info->supplicant_config_approach = WPA_CLI_APPROACH;
	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_WFD_PROG))
		mrvl_WS_info->supplicant_config_approach = WPA_CLI_APPROACH;
	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_HS2_PROG))
		mrvl_WS_info->supplicant_config_approach = WPA_CLI_APPROACH;
	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_MBO_PROG))
		mrvl_WS_info->supplicant_config_approach = WPA_CLI_APPROACH;
	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_WPA3_PROG))
		mrvl_WS_info->supplicant_config_approach = WPA_CLI_APPROACH;
	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_HE_PROG))
		mrvl_WS_info->supplicant_config_approach = WPA_CLI_APPROACH;
	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_V_E_PROG))	
		mrvl_WS_info->supplicant_config_approach = WPA_CLI_APPROACH;
    /** MWUD */		
	read_ini_config(SIGMA_USER_CONFIG,"MWUD","mwu_bin",mrvl_mwud_info->mwu_bin);
	read_ini_config(SIGMA_USER_CONFIG,"MWUD","mwu_cli_bin",mrvl_mwud_info->mwu_cli_bin);
	read_ini_config(SIGMA_USER_CONFIG,"MWUD","mwu_conf_wifidir",mrvl_mwud_info->mwu_conf_wifidir);
	read_ini_config(SIGMA_USER_CONFIG,"MWUD","mwu_conf_wifidirect_defaults",mrvl_mwud_info->mwu_conf_wifidirect_defaults);
	read_ini_config(SIGMA_USER_CONFIG,"MWUD","mwud_service",mrvl_mwud_info->mwud_service);
	
	/** Hostapd */
	read_ini_config(SIGMA_USER_CONFIG,"Hostapd","hostapd_bin",mrvl_hostapd_info->hostapd_bin);
	read_ini_config(SIGMA_USER_CONFIG,"Hostapd","hostapd_cli_bin",mrvl_hostapd_info->hostapd_cli_bin);
	read_ini_config(SIGMA_USER_CONFIG,"Hostapd","hostapd_ext",mrvl_hostapd_info->hostapd_ext);

    /** Dhcp_Server */
	read_ini_config(SIGMA_USER_CONFIG,"Dhcp_Server","dhcp_srv_ip",mrvl_dhcp_info->dhcp_srv_ip);
	read_ini_config(SIGMA_USER_CONFIG,"Dhcp_Server","dhcp_range",mrvl_dhcp_info->dhcp_range);
	read_ini_config(SIGMA_USER_CONFIG,"Dhcp_Server","dhcpd_lease_file",mrvl_dhcp_info->dhcpd_lease_file);
	read_ini_config(SIGMA_USER_CONFIG,"Dhcp_Server","dhclient_lease_file",mrvl_dhcp_info->dhclient_lease_file);

	/** Debug */ 
	read_ini_config(SIGMA_USER_CONFIG,"Debug","console_logging",temp);
	mrvl_debug_info->console_logging = atoi(temp);
	
	LEAVE( __func__ );
	return TRUE;
}


void console_dump_file ( char *filename ) {
	ENTER( __func__ );	
	sprintf(gCmdStr,"%s cat %s",DUT_CMD_PREFIX,filename);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_dut_date ( int month, int date, int year) {
	ENTER( __func__ );
	char date_index[4];
	char month_index[4];		
	
	if (month < 10) 
		sprintf(month_index,"0%d",month);
	else 
		sprintf(month_index,"%d",month);
	
	if (date < 10) 
		sprintf(date_index,"0%d",date);
	else 
		sprintf(date_index,"%d",date);
	 
	sprintf(gCmdStr, "date -s %d%s%s",year,month_index,date_index);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int set_dut_time ( int hours, int min, int sec) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s date -s %d:%d:%d",DUT_CMD_PREFIX,hours,min,sec);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int process_generic_mlanutl_cmd ( char *ifname, char* label, int val) {
	ENTER( __func__ );
	printf("\nin mrvlcomon_c\n---the value for the label->BANDCFG is ---%d---",val);
	sprintf(gCmdStr, "%s/%s %s %s %d",APP_BIN_LOC,MLANUTL,ifname,label,val);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int enable_driver_debug ( char *ifname, char *debugCode ) {
	ENTER( __func__ );
    sprintf(gCmdStr, "%s %s %s drvdbg %s",DUT_CMD_PREFIX,IWPRIV,ifname,debugCode);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int kill_wlan_utilities() {
	ENTER( __func__ );
	
	sprintf(gCmdStr, "iwpriv %s deauth",mrvl_dut_info->sta_interface);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "killall wpa_supplicant");
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "killall hostapd");
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "killall dhcpd3");
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "killall dhclient");
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "killall dnsmasq");
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "killall udhcpc");
	system_with_log(gCmdStr);

	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_WFD_PROG)) {
		sprintf(gCmdStr, "killall MarvellWFDServer");
		system_with_log(gCmdStr);

		sprintf(gCmdStr, "killall MarvellWFDClient");
		system_with_log(gCmdStr);
	}
	LEAVE( __func__ );
	return TRUE;
}

int reload_wlan_build() {
	ENTER( __func__ );
	system_with_log(mrvl_dut_info->build_unload);
	system_with_log(mrvl_dut_info->build_load);
	LEAVE( __func__ );
	return TRUE;
}

char * update_client_certificate_name ( char *cert , char *ext ) {
	ENTER( __func__ );
	if (strstr(cert,ext)) {
		LEAVE( __func__ );
		return cert;
	} else {
		strcat(cert,".");
		strcat(cert,ext);
		LEAVE( __func__ );
		return cert;
	}
}

int launch_dhcp_server(char *ifname) {
	ENTER( __func__ );
	pid_t pid;
	pid=fork();
	if( pid == 0)
	{	
		sprintf(gCmdStr,"%s ifconfig %s %s",DUT_CMD_PREFIX,ifname,mrvl_dhcp_info->dhcp_srv_ip);
		system_with_log(gCmdStr);                      
		sprintf(gCmdStr,"dnsmasq --no-daemon --no-resolv --no-poll --dhcp-range=%s,1h",mrvl_dhcp_info->dhcp_range);
		system_with_log(gCmdStr);
		exit(0);
    }
	LEAVE( __func__ );
}  
