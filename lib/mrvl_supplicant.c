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


int enable_supplicant_reassoc_ctrl ( char *ifname, int network_index, char *bssid, int channel ) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s -i%s disconnect",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d bssid %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,bssid);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s -i%s REASSOCIATE %s %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname, bssid, channel);
	system_with_log(gCmdStr);
   // sleep(10);

//	sprintf(gCmdStr,"%s/%s -i%s REASSOCIATE %s %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname, bssid, channel);
//    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int enable_supplicant_association ( char *ifname, char *ssid){
	ENTER( __func__ );
	/** TODO */
	
	LEAVE( __func__ );
	return TRUE;
}

int set_supplicant_p2pmanaged (char *ifname, int mode) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s -i%s p2p_set managed %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,mode);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int supplicant_disconnect ( char *ifname ) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s -i%s disconnect",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int configure_supplicant_security_mode (FILE * fd, char *encrptype, char* keyMgmtType, char *ssid) {
	ENTER( __func__ );	
	if (!strncmp(encrptype,"aes-ccmp-tkip",13)) {
		if (!strncmp(keyMgmtType,"wpa2-wpa",8)) {
			fprintf(fd,"\tproto=WPA RSN\n");
			fprintf(fd,"\tpairwise=TKIP CCMP\n");
		} else if (!strncmp(keyMgmtType,"wpa2",4)) {
			fprintf(fd,"\tproto=RSN\n");
			fprintf(fd,"\tpairwise=CCMP\n");
		} else if (!strncmp(keyMgmtType,"wpa",3)) {
			fprintf(fd,"\tproto=WPA\n");
			fprintf(fd,"\tpairwise=TKIP\n");
		}
		fprintf(fd,"\tgroup=CCMP TKIP\n");
    } else if (!strncmp(encrptype,"tkip",4)) {
		fprintf(fd,"\tproto=WPA\n");
		fprintf(fd,"\tpairwise=TKIP\n");
		//fprintf(fd,"\tgroup=TKIP\n");
    } else if (!strncmp(encrptype,"aes",3)) {
		fprintf(fd,"\tproto=RSN\n");
		fprintf(fd,"\tpairwise=CCMP\n");
		//fprintf(fd,"\tgroup=CCMP\n");
    } else {
		fprintf(fd,"\tproto=RSN\n");
		fprintf(fd,"\tpairwise=CCMP TKIP\n");
		//fprintf(fd,"\tgroup=CCMP TKIP\n");
    }
	LEAVE( __func__ );
	return TRUE;
}

int configure_supplicant_security_mode_cli (int network_index, char *encrptype, char* keyMgmtType, char *ssid) {
	ENTER( __func__ );	
  	char *ifname = mrvl_WS_info->sta_ctrl_interface;
	if (!strncmp(encrptype,"aes-ccmp-tkip",13)) {
		if (!strncmp(keyMgmtType,"wpa2-wpa",8)) {
  			sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d proto \\\"WPA RSN\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
			system_with_log(gCmdStr);
  			sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d pairwise \\\"TKIP CCMP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
			system_with_log(gCmdStr);
		} else if (!strncmp(keyMgmtType,"wpa2",4)) {
  			sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d proto \\\"RSN\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
			system_with_log(gCmdStr);
  			sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d pairwise \\\"CCMP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
			system_with_log(gCmdStr);
		} else if (!strncmp(keyMgmtType,"wpa",3)) {
  			sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d proto \\\"WPA\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
			system_with_log(gCmdStr);
  			sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d pairwise \\\"TKIP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
			system_with_log(gCmdStr);
		}
		sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d group \\\"TKIP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
		system_with_log(gCmdStr);
    } else if (!strncmp(encrptype,"tkip",4)) {
		sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d proto \\\"WPA\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
		system_with_log(gCmdStr);
		sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d pairwise \\\"TKIP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
		system_with_log(gCmdStr);
		sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d group \\\"TKIP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
		system_with_log(gCmdStr);
    } else if (!strncmp(encrptype,"aes",3) || !strncasecmp(encrptype,"aes-ccmp",8)) {
		sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d proto RSN",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
		system_with_log(gCmdStr);
		sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d pairwise CCMP",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
		system_with_log(gCmdStr);
		sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d group CCMP",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
		system_with_log(gCmdStr);
    } else {
		sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d proto \\\"RSN\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
		system_with_log(gCmdStr);
		sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d pairwise \\\"CCMP TKIP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
		system_with_log(gCmdStr);
		sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d group \\\"CCMP TKIP\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);    		
		system_with_log(gCmdStr);
    }
	LEAVE( __func__ );
	return TRUE;
}

int configure_eapttls_cli (caStaSetEapTTLS_t *setTTLS, int network_index) {
	ENTER( __func__ );	
  	char *ifname = mrvl_WS_info->sta_ctrl_interface;
    char *ssid = setTTLS->ssid;
    char *encrptype = setTTLS->encrptype;
    char *keyMgmtType = setTTLS->keyMgmtType;
    char *trustedRootCA = setTTLS->trustedRootCA;
    char *clientCertificate = setTTLS->clientCertificate;
    char *username = setTTLS->username;
    char *passwd = setTTLS->passwd;

    sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d ssid \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,ssid);
    system_with_log(gCmdStr);		
   	sprintf(gCmdStr,"%s/%s -i%s ENABLE_NETWORK %d no-connect",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
   	sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d key_mgmt WPA-EAP",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
	configure_supplicant_security_mode_cli (network_index,encrptype,keyMgmtType,ssid); 

   	sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d ca_cert \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,trustedRootCA);
    system_with_log(gCmdStr);
   	sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d identity \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,username);
    system_with_log(gCmdStr);
   	sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d password \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,passwd);
    system_with_log(gCmdStr);
   	sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d eap TTLS",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
  	sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d phase2 \\\"auth=MSCHAPV2\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
} 

int supplicant_network_scan ( char *ifname ) {
	ENTER( __func__ );
    sprintf(gCmdStr,"%s/%s -i%s scan",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
}

int launch_supplicant_file_instance ( char *ifname ) {
	int cnt=0;

	ENTER( __func__ );	
	sprintf(gCmdStr,"%s killall %s",DUT_CMD_PREFIX,mrvl_WS_info->supplicant_bin);		
    system_with_log(gCmdStr);
	/** Wait for clearance of previous supplicant Instance */
	sleep(2);

    sprintf(gCmdStr, "iwpriv %s deauth", ifname);
    system_with_log(gCmdStr);
    
	sprintf(gCmdStr, "ifconfig %s up",ifname);
	system_with_log(gCmdStr);
	
	if (!strcasecmp(SIGMA_PROG_NAME,MRVL_P2P_PROG) || !strcasecmp(SIGMA_PROG_NAME,MRVL_WFD_PROG)) {
		sprintf(gCmdStr, "%s/%s -D%s -i %s -c %s -ddd -N -D%s -i %s -c %s -O /var/run/wpa_supplicant > %s &", APP_BIN_LOC,mrvl_WS_info->supplicant_bin, mrvl_WS_info->supplicant_ext, ifname, P2P_SUPPLICANT_CONF,mrvl_WS_info->supplicant_ext, mrvl_dut_info->sta_interface, mrvl_WS_info->supplicant_conf, SUPP_LOG_FILE);
		system_with_log(gCmdStr);

	} 
	if (MWIFIEX) {
			sleep(2);		
			sprintf(gCmdStr,"%s/%s -i %s SCAN",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
			system_with_log(gCmdStr);
			sleep(3);
		}		
	if ( !strcasecmp(SIGMA_PROG_NAME,MRVL_TDLS_PROG) ) {
		/** Scan network for association. TDLS association issue with wpa supplicant */ 
		mlan_network_scan(mrvl_dut_info->sta_interface);
	}
		
	if ( mrvl_WS_info->supplicant_config_approach != WPA_CLI_APPROACH ) {
		while (cnt<10) {
			sleep(2);
			if ( check_wlan_connectivity(ifname)  == TRUE )
				break;
			cnt++;
		}
		} 
	
	{ 
		/** Wait approx 5sec. for successfull supplicant invocation */
		sprintf(gCmdStr, "%s/%s -D%s -i %s -c %s -ddd -O /var/run/wpa_supplicant > %s &", APP_BIN_LOC,mrvl_WS_info->supplicant_bin, mrvl_WS_info->supplicant_ext, ifname, SUPPLICANT_CONF,SUPP_LOG_FILE);
		system_with_log(gCmdStr);
		sleep(5);	
	}
	LEAVE( __func__ );
	return TRUE;
}

int select_active_network ( char *ifname,int network_index ) {
	ENTER( __func__ );
	int cnt=0;

	sprintf(gCmdStr, "%s/%s -i %s select_network %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);	
	while (cnt<10) {
		sleep(2);
		if ( check_wlan_connectivity(ifname)  == TRUE )
			break;
		cnt++;
	}
	LEAVE( __func__ );
	return TRUE;
}

int set_wpacli_wep_configuration ( char *key1, char*key2, char *key3, char *key4, int activeKeyIdx) {
	ENTER( __func__ );
	/** TODO */
	
	LEAVE( __func__ );
	return TRUE;
}
int set_wpacli_open_configuration ( char *ifname, int network_index, char *ssid) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d ssid \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,ssid);
    system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt NONE",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int enable_WS_tdls_capabilities ( char *ifname ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s SET tdls_disabled 0",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_WS_rrm (char *ifname, int mode) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s SET rrm_used %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,mode);
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int configure_hs2_homerealmlist( char *ifname, char* dest) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s NAI_HOME_REALM_LIST %s realm=mail.example.com",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,dest);			  
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int configure_hs2_iconrequest( char *ifname, char* dest, char* icon) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s HS20_ICON_REQUEST %s %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,dest,icon);			  
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int get_supplicant_cred_index ( char *ifname ) {
	ENTER( __func__ );
	FILE *pipe=NULL;
    char cred_index[4];
	sprintf(gCmdStr, "%s/%s -i%s ADD_CRED",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);			  
    pipe = popen(gCmdStr, "r");
    fgets(cred_index, 4, pipe);
	if (cred_index[ strlen(cred_index) -1 ] == '\n')
		cred_index[ strlen(cred_index) -1 ] = '\0';
    pclose(pipe);
	LEAVE( __func__ );
	return atoi(cred_index);
}

int get_supplicant_network_index ( char *ifname ) {
	ENTER( __func__ );
	FILE *pipe=NULL;
    char network_index[4];
	sprintf(gCmdStr, "%s/%s -i%s ADD_NETWORK",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);			  
    pipe = popen(gCmdStr, "r");
    fgets(network_index, 4, pipe);
	if (network_index[ strlen(network_index) -1 ] == '\n')
		network_index[ strlen(network_index) -1 ] = '\0';
    pclose(pipe);
	LEAVE( __func__ );
	return atoi(network_index);
}

int assign_network_priority ( char *ifname, int network_index ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d priority 1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}
int set_bssid_network(char *ifname,int network_index, char *bssid) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d bssid %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,bssid);
    system_with_log(gCmdStr);
}
int configure_supplicant_cli_eap (int network_index, caStaSetSecurity_t *sec) {
	ENTER( __func__ );
	char *ifname = mrvl_WS_info->sta_ctrl_interface;
	sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d ssid \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,sec->ssid);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s/%s -i %s ENABLE_NETWORK %d no-connect",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
	printf("\n\r The value of parameter  client_cert is----------------\n\r %s----------------\n\n\r",sec->secu.ttls.client_cert);
    if (sec->secu.ttls.akmsuitetype == 14 || sec->secu.ttls.akmsuitetype == 15) {
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt FILS-SHA256 FILS-SHA384",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d erp 1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
        system_with_log(gCmdStr);
    } else {
		sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt WPA-EAP",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
		system_with_log(gCmdStr);
    }
    sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d identity \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index, 
           sec->secu.ttls.username);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d password \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index, 
           sec->secu.ttls.passwd);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d ca_cert \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index, 
           sec->secu.ttls.trustedRootCA);
    system_with_log(gCmdStr);
    if (sec->type == SEC_TYPE_EAPTLS) {
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d eap TLS",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d private_key \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index, 
                sec->secu.ttls.client_cert);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d client_cert \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index, 
                sec->secu.ttls.client_cert);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d private_key_passwd \\\"wifi\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
        system_with_log(gCmdStr);
        if (strcasecmp(sec->keyMgmtType, "SuiteB") == 0) {
            char tls_cipher[50] = {0};
            char pairwise[50]  ={0};
            char group[50]  ={0};
            char group_mgmt_cipher[50] = {0};
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt WPA-EAP-SUITE-B-192",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
            if (strcasecmp(sec->secu.ttls.pairwise_cipher, "AES-GCMP") == 0) {
                strcpy(pairwise, "GCMP");
            } else {
                strcpy(pairwise, "GCMP-256");
            }
            if (strcasecmp(sec->secu.ttls.group_cipher, "AES-GCMP-256") == 0) {
                strcpy(group, "GCMP-256");
            } else {
                strcpy(group, "GCMP");
            }
            sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d pairwise %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,
                    pairwise);
            system_with_log(gCmdStr);
            sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d group %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,
                group);
            system_with_log(gCmdStr);
            if (strcasecmp(sec->secu.ttls.group_mgmt_cipher, "BIP-GMAC-256") == 0) {
                strcpy(group_mgmt_cipher, "BIP-GMAC-256");
            } else if (strcasecmp(sec->secu.ttls.group_mgmt_cipher, "BIP-CMAC-256") == 0) {
                strcpy(group_mgmt_cipher, "BIP-CMAC-256");
            } else if (strcasecmp(sec->secu.ttls.group_mgmt_cipher, "BIP-GMAC-128") == 0) {
                strcpy(group_mgmt_cipher, "BIP-GMAC-128");
            } else if (strcasecmp(sec->secu.ttls.group_mgmt_cipher, "BIP-CMAC-128") == 0) {
                strcpy(group_mgmt_cipher,"AES-128-CMAC");
            } else {
                strcpy(group_mgmt_cipher,"BIP-GMAC-256");//default group mgmt
            }
            sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d group_mgmt %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,
                    group_mgmt_cipher);
            system_with_log(gCmdStr);
            sprintf(gCmdStr,"%s/%s -i%s SET_NETWORK %d ieee80211w 2",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
            if (strcasecmp(sec->secu.ttls.cert_type,  "RSA") == 0) {
                sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d phase1 \\\"tls_suiteb=1\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
                system_with_log(gCmdStr);
            } else {
                sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d openssl_ciphers \\\"SUITEB192\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
                system_with_log(gCmdStr);
            }
            if (sec->secu.ttls.tls_cipher_flag == 1) {
                if (strcasecmp(sec->secu.ttls.tls_cipher, "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384") == 0)
                    strcpy(tls_cipher, "ECDHE-ECDSA-AES256-GCM-SHA384");
                else if (strcasecmp(sec->secu.ttls.tls_cipher,
                            "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384") == 0)
                    strcpy(tls_cipher, "ECDHE-RSA-AES256-GCM-SHA384");
                else if (strcasecmp(sec->secu.ttls.tls_cipher, "TLS_DHE_RSA_WITH_AES_256_GCM_SHA384") == 0)
                    strcpy(tls_cipher, "DHE-RSA-AES256-GCM-SHA384");
                else if (strcasecmp(sec->secu.ttls.tls_cipher,
                            "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256") == 0)
                    strcpy(tls_cipher, "ECDHE-ECDSA-AES128-GCM-SHA256");
                sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d phase1 \\\"\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
                system_with_log(gCmdStr);
                sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d openssl_ciphers %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index, 
                        tls_cipher);
                system_with_log(gCmdStr);
            }
        }
    }
    if (sec->type == SEC_TYPE_EAPTTLS) {
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d eap TTLS",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d phase2 \\\"auth=MSCHAPV2\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
        system_with_log(gCmdStr);
    }
	LEAVE( __func__ );
	return TRUE;
}

int configure_supplicant_cli (int network_index, int network_type, char* encrypType, char *keyMgmtType, char *ssid, char *passphrase) {
	ENTER( __func__ );
	char *ifname = mrvl_WS_info->sta_ctrl_interface;

	sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d ssid \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,ssid);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s/%s -i %s ENABLE_NETWORK %d no-connect",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
    if (network_type == SEC_TYPE_SAE) {
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK 0 ieee80211w 2",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,
                mrvl_WS_info->sta_ctrl_interface);
        system_with_log(gCmdStr);
        if (!strncmp(keyMgmtType,"wpa2-ft",7)) {
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt FT-SAE",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
        } else {
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt SAE",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
        }
    } else if (network_type == SEC_TYPE_SAE_PSK) {
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK 0 ieee80211w 2",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,
                mrvl_WS_info->sta_ctrl_interface);
        system_with_log(gCmdStr);
        if (!strncmp(keyMgmtType,"wpa2-ft",7)) {
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt FT-SAE FT-PSK",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
        } else {
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt SAE WPA-PSK",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
    system_with_log(gCmdStr);
        }

    } else if (network_type == SEC_TYPE_OWE) {
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK 0 ieee80211w 2",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,
                mrvl_WS_info->sta_ctrl_interface);
        system_with_log(gCmdStr);
        sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt OWE",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
        system_with_log(gCmdStr);
    } else if (network_type == SEC_TYPE_OPEN) {
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt NONE",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
    }
    else {
        if (!strncmp(keyMgmtType,"wpa2-ft",7)) {
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt FT-PSK",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d proactive_key_caching=1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
        } else {
            sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d key_mgmt WPA-PSK WPA-PSK-SHA256",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
            system_with_log(gCmdStr);
        }
    }
    if (network_type != SEC_TYPE_OPEN) {
	configure_supplicant_security_mode_cli (network_index,encrypType,keyMgmtType,ssid); 

    sprintf(gCmdStr, "%s/%s -i %s SET_NETWORK %d psk \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index,passphrase);
    system_with_log(gCmdStr);	
    }
	LEAVE( __func__ );
	return TRUE;
}

int assign_cred_priority ( char *ifname, int cred_index, int prefer) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d priority %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,prefer);			  
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int configure_hs2_cred_sim ( char *ifname, int cred_index, char* realm, char* imsi, char* password) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d realm \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,realm);			
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d imsi \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,imsi);			  
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d milenage \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,password);			  
	system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int download_certificate( char *certName, char *certPath, char *rename ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "wget %s/%s",certPath,certName);
	system_with_log(gCmdStr);  
	sprintf(gCmdStr, "\\cp -f %s %s",certName,rename);
	system_with_log(gCmdStr);  
	LEAVE( __func__ );
	return TRUE;
}
int install_certificate ( char *certName , char* destName) {
	ENTER( __func__ );
	sprintf(gCmdStr, "\\cp -f %s %s/%s",certName,HS2_R2_CERTPATH,destName);
	system_with_log(gCmdStr);  
	LEAVE( __func__ );
	return TRUE;
}

int generate_ppsmo_client_certificate (char* iface, char *certName ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -w %s/ -r hs20-osu-client.res -s summary -dddKt -f hs20-osu-client.txt from_tnds %s %s/pps.xml",APP_BIN_LOC,HS20_OSU_CLIENT,SUPPLICANT_CTRL_PATH,certName,HS2_R2_CERTPATH);
    system_with_log(gCmdStr); 
	sprintf(gCmdStr, "%s/%s -w %s/ -r hs20-osu-client.res -s summary -dddKt -f hs20-osu-client.txt dl_osu_ca %s/pps.xml %s/ca.pem",APP_BIN_LOC,HS20_OSU_CLIENT,SUPPLICANT_CTRL_PATH,HS2_R2_CERTPATH,HS2_R2_CERTPATH);
    system_with_log(gCmdStr); 
	sprintf(gCmdStr, "%s/%s -w %s/ -r hs20-osu-client.res -s summary -dddKt -f hs20-osu-client.txt dl_aaa_ca %s/pps.xml %s/aaa-ca.pem",APP_BIN_LOC,HS20_OSU_CLIENT,SUPPLICANT_CTRL_PATH,HS2_R2_CERTPATH,HS2_R2_CERTPATH);
    system_with_log(gCmdStr); 
	sprintf(gCmdStr, "%s/%s -w %s/ -S %s -r hs20-osu-client.res -s summary -dddKt -f hs20-osu-client.txt set_pps %s/pps.xml ",APP_BIN_LOC,HS20_OSU_CLIENT,SUPPLICANT_CTRL_PATH,iface,HS2_R2_CERTPATH);
    system_with_log(gCmdStr); 
	LEAVE( __func__ );
	return TRUE;
}

int configure_hs2_cred_uname ( char *ifname, int cred_index, char *realm, int home_fdqn, char *fqdn, char *username, char* password, char* ca) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d realm \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,realm);			  
	system_with_log(gCmdStr);
	if (home_fdqn) {
		sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d domain \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,fqdn);			  
		system_with_log(gCmdStr);
	}	
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d username \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,username);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d password \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,password);			  
	system_with_log(gCmdStr);	
	sprintf(gCmdStr, "%s/%s -i%s SET_CRED %d ca_cert \\\"%s\\\"",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,cred_index,ca);			  
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int configure_supplicant_hs2_anqp( char *ifname, char *ANQP_LIST, char *HS20_ANQP_LIST, char* dest) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s BSS %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,dest);			  
	system_with_log(gCmdStr);
	if (ANQP_LIST[0] != '\0') {
		sprintf(gCmdStr, "%s/%s -i%s ANQP_GET %s %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,dest,ANQP_LIST);			  
		system_with_log(gCmdStr);
	}	
	if (HS20_ANQP_LIST[0] != '\0') {
		sprintf(gCmdStr, "%s/%s -i%s HS20_ANQP_GET %s %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,dest,HS20_ANQP_LIST);			  
		system_with_log(gCmdStr);
	}
	LEAVE( __func__ );
	return TRUE;
}

int send_WS_tdls_frame (char *ifname, int frame, char *peer) {
	ENTER( __func__ );
	switch (frame) {
		case TDLS_TYPE_DISCOVERY:
		sprintf(gCmdStr, "%s/%s -i %s TDLS_DISCOVER %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,peer);
		system_with_log(gCmdStr);
		break;
		case TDLS_TYPE_SETUP:
		sprintf(gCmdStr, "%s/%s -i %s TDLS_SETUP %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,peer);
		system_with_log(gCmdStr);
		break;
		case TDLS_TYPE_TEARDOWN:
		sprintf(gCmdStr, "%s/%s -i %s TDLS_TEARDOWN %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,peer);
		system_with_log(gCmdStr);
		break;
		case TDLS_TYPE_CHANNELSWITCH:
		break;
		case TDLS_TYPE_NULLFRAME:
		break; 	
	}
	LEAVE( __func__ );
	return TRUE;	
}

int clear_supplicant_instance() {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s killall dnsmasq",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);	
    sprintf(gCmdStr, "%s killall wpa_supplicant",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s killall wpa_cli",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s killall p2p_supplicant",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s ifconfig %s down", DUT_CMD_PREFIX,mrvl_dut_info->p2p_interface);
    system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s ifconfig %s down", DUT_CMD_PREFIX,mrvl_dut_info->sta_interface);
    system_with_log(gCmdStr);		
#ifdef MV_PLATFORM_ANDROID	
	sprintf(gCmdStr,"%s rm /data/misc/wifi/sockets/wpa_ctrl_*",DUT_CMD_PREFIX);	
    system_with_log(gCmdStr);
#else 
	sprintf(gCmdStr, "%s mkdir -p /var/run/wpa_supplicant",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);
#endif    
	sprintf(gCmdStr, "%s dmesg -c > /dev/null",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s killall logcat",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "%s rm -rf /tmp/supp.logcat",DUT_CMD_PREFIX);
    system_with_log(gCmdStr);
    sprintf(gCmdStr, "logcat -c");
    system_with_log(gCmdStr);	

	/* Virtual Interface flush required here */	
	strcpy(mrvl_WS_info->p2p_interface,mrvl_dut_info->p2p_interface);
	read_ini_config(SIGMA_USER_CONFIG,"Wpa_Supplicant","p2p_ctrl_interface",mrvl_WS_info->p2p_ctrl_interface);
	
#ifdef MV_PLATFORM_ANDROID	
    sprintf(gCmdStr, "logcat -s wpa_supplicant > /tmp/supp.logcat &");
    system_with_log(gCmdStr);
#endif 	
	LEAVE( __func__ );
	return TRUE;	
}

int fetch_supplicant_passphrase (char *ifname, char *passphrase) {
	char buff[50],ch;
    char *str;
    int i;
    FILE *tmpfd;
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s p2p_get_passphrase > /tmp/pass.txt",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	tmpfd = fopen("/tmp/pass.txt", "r");
	fgets(buff, sizeof(buff), tmpfd);			
	strcpy(passphrase,buff);
	fclose(tmpfd);
	LEAVE( __func__ );
	return TRUE;	
}

int fetch_active_network_info (char *ifname, int network_index, char *ssid, char *bssid) {
	FILE *tmpfd;
	int i,j;
	
	ENTER( __func__ );
	
	sprintf(gCmdStr, "%s/%s -i %s get_network %d ssid > /tmp/ssid.txt", APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,ifname, network_index);
	system_with_log(gCmdStr);

	tmpfd = fopen("/tmp/ssid.txt", "r");
	fgets(ssid, 128, tmpfd);
  
	if( ssid[0]== '"' ) {
	i=1;
	j=0;
	while( ssid[i] != '"' ) {
		ssid[j]=ssid[i];
		i++;
		j++;
	}
	ssid[j]='\0';
	}
	fclose(tmpfd);

	sprintf(gCmdStr, "%s/%s -i %s get_network %d bssid > /tmp/bssid.txt", APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,ifname, network_index);
	system_with_log(gCmdStr);

	tmpfd = fopen("/tmp/bssid.txt", "r");
	fgets(bssid, 128, tmpfd);
	fclose(tmpfd);
	
	LEAVE( __func__ );
	return TRUE;
}

int fetch_supplicant_ssid (char *ifname, char *ssid, int network_index) {
	char buff[50];
    char *p;
  
    FILE *tmpfd;
	ENTER( __func__ );

	sprintf(gCmdStr, "%s/%s -i %s get_network %d ssid > /tmp/ssid.txt",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,network_index);
	system_with_log(gCmdStr);
	tmpfd = fopen("/tmp/ssid.txt", "r");
	fgets(buff, sizeof(buff), tmpfd);	
	
    p = strtok(buff, "\"");
    if (p)
	strcpy(buff, p);
		
	strcpy(ssid,buff);
	fclose(tmpfd);
	LEAVE( __func__ );
	return TRUE;	
}

int set_supplicant_wps_pin ( char *ifname, char *pin) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s wps_pin any %s", APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,ifname, pin);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int init_hs2_capabilities ( char *ifname ) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s WPS_ER_STOP",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s FLUSH",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s SET radio_disabled 0",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s SET interworking 1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s SET hs20 1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s SET pmf 0",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s REMOVE_CRED all",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s SET hessid 00:00:00:00:00:00",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s SET access_network_type 15",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr, "mkdir -p %s",HS2_R2_CERTPATH);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_hs2_network_type ( char *ifname, char *hessid, int accNetType) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s -i%s SET hessid %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,hessid);
    system_with_log(gCmdStr);
    sprintf(gCmdStr,"%s/%s -i%s SET access_network_type %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,accNetType);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int hs2_supplicant_associate ( char *ifname ) {
	int cnt=0;
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s INTERWORKING_SELECT auto",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);			  
    system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i%s REASSOCIATE",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);			  
    system_with_log(gCmdStr);
    while (cnt<10) {
		sleep(2);
		if ( check_wlan_connectivity(ifname) == TRUE )
			break;
		cnt++;
	}	
	LEAVE( __func__ );
	return TRUE;	
}

int set_supplicant_listen_channel ( char *ifname, int channel) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s p2p_set listen_channel %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,channel);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int enable_supplicant_persistent_mode (char *ifname) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s persistent_reconnect 1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int enable_supplicant_cross_connect (char *ifname) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s p2p_set cross_connect 1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

void supplicant_remove_network(char* ifname)
{
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s remove_network 0", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
}

int set_supplicant_ext_listen (char *ifname,int ext_listen_time_period, int ext_listen_time_int) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i%s p2p_ext_listen %d %d", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,ext_listen_time_period,ext_listen_time_int);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int supplicant_initiate_p2p_negotiation( char *ifname, char * devId, int intent_val, char * freq, char *wps_method, char *wps_pin) {
	ENTER( __func__ );
	int network_index; 

	int go_intent = intent_val;
	if(go_intent > 15)
		go_intent = 15;
	if(go_intent < 0)
		go_intent = 0;

	sprintf(gCmdStr,"%s/%s -i %s p2p_find",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sleep(10);	

	if (!strcmp(wps_method,WPS_PBC)) { 
		sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s pbc persistent go_intent=%d freq=%d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId,go_intent,freq);
	} 
	if (!strcmp(wps_method,ENTER_WPS_PIN)) { 
		sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s keypad persistent go_intent=%d freq=%d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId,wps_pin,go_intent,freq);
	}
	if (!strcmp(wps_method,READ_WPS_PIN)) { 
		sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s display persistent go_intent=%d freq=%d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId,wps_pin,go_intent,freq);
	}
	system_with_log(gCmdStr);	
	
	/** TODO: Need to check Negotiation Event in WPA Supplicant log*/
	sleep(10);
	
	network_index = get_active_network_index();
	printf("%s network_index:[%d]\n",__func__, network_index);
	if (network_index == -1) {
		return FALSE;
	}
	LEAVE( __func__ );
	return TRUE;
}

int init_p2p_connect_thread ( char *ifname, char * devId, int intent_val, char * freq, char *wps_method, char *wps_pin ) {
	ENTER( __func__ );	

	int i=0,c=0;
	char tmpStr[128];
	FILE *pipe;

	pid_t pid;
	pid=fork();

	if (pid == 0) {	 			
		while (i <= 30) {
			c = 0;
			sprintf(gCmdStr, "%s cat %s | %s grep P2P-GO-NEG-REQUEST", DUT_CMD_PREFIX,SUPP_LOG_FILE,DUT_CMD_PREFIX);
			pipe = popen(gCmdStr, "r");
			c = fgets(tmpStr, 150, pipe);
			if(c != 0) {
				pclose(pipe);
				printf("Negotiation request from peer received... Proceed with connection\n");
				c = 1;
				break;
			} else {
				i = i + 5;
				sleep(2);
				printf("Waiting for the negotiation req from peer (%d sec)", i);
			}
			pclose(pipe);
		}
		if (c == 1) {
			if (!strcmp(wps_method,WPS_PBC)) { 
				sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s pbc persistent go_intent=%d freq=%d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId,intent_val,freq);
			} 
			if (!strcmp(wps_method,ENTER_WPS_PIN)) { 
				sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s keypad persistent go_intent=%d freq=%d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname, devId,wps_pin,intent_val,freq);
			}
			if (!strcmp(wps_method,READ_WPS_PIN)) { 
				sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s display persistent go_intent=%d freq=%d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname, devId,wps_pin,intent_val,freq);
			}
			system_with_log(gCmdStr);
			exit(0);	
		}
	}
	LEAVE( __func__ );
	return TRUE;
}

int set_supplicant_p2p_wps (char *ifname, char *peerId, char *wps_method, char *wps_pin) {
	ENTER( __func__ );
	if (!strcmp(wps_method,WPS_PBC) ) {   
        sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s pbc join",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,peerId);
    }
    if (!strcmp(wps_method,ENTER_WPS_PIN)) {   
        sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s keypad join",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,peerId,wps_pin);
    }
    if (!strcmp(wps_method,READ_WPS_PIN)) {  
        sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s display join", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,peerId,wps_pin);
    }
    system_with_log(gCmdStr);	
	LEAVE( __func__ );
	return TRUE;
}

int set_supplicant_p2p_mode (char *ifname,char *mode) {
	ENTER( __func__ );
	if (!strcasecmp(mode,"Listen")) {
		sprintf(gCmdStr, "%s/%s -i%s p2p_listen",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
		system_with_log(gCmdStr);	
	} else if (!strcasecmp(mode,"discover")) {
		sprintf(gCmdStr, "%s/%s -i %s p2p_find",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
		system_with_log(gCmdStr);
	} else if (!strcasecmp(mode,"Idle")) {
		launch_supplicant_file_instance(mrvl_dut_info->p2p_interface);
	} else if (!strcmp(mode,"disable")) {
		sprintf(gCmdStr, "%s killall %s",DUT_CMD_PREFIX,mrvl_WS_info->supplicant_bin);
		system_with_log(gCmdStr);
	}	
	LEAVE( __func__ );
	return TRUE;
}

int set_supplicant_wps_pbc(char *ifname) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s wps_pbc",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int get_supplicant_read_pin (char *wps_pin) {
	char wps_read_pin[16];
	int i=0;
	FILE *fp;
    FILE *pipe = NULL;
    char pin_from_pd_req[10] = {0};
    char pd_log_line[256] = {0};
    char *pos,*cur_pos;
    int c = 0;
	
	ENTER( __func__ );
	sprintf(gCmdStr, "%s cat %s | %s grep P2P-PROV-DISC-SHOW-PIN", DUT_CMD_PREFIX,SUPP_LOG_FILE,DUT_CMD_PREFIX);
    pipe = popen(gCmdStr, "r");
    c = fgets(pd_log_line, 256, pipe);
    pclose(pipe);	
    printf("\n==> pd_log_line: %s\n", pd_log_line);
	if (c == 0) {
        printf("\n==> Provision discovery request with DISPLAY method not received, generating pin...\n");
        sprintf(gCmdStr, "echo 12345670 > /tmp/pin.txt");
        system_with_log(gCmdStr);
    }  else {
        printf("\n==> Provision discovery request with DISPLAY method received, extracting pin from event\n");
        pos = strstr(pd_log_line, "P2P-PROV-DISC-SHOW-PIN");
        cur_pos = strtok_r(NULL, " ", &pos);
        cur_pos = strtok_r(NULL, " ", &pos);
        cur_pos = strtok_r(NULL, " ", &pos);
        sprintf(gCmdStr, "echo %s > /tmp/pin.txt", cur_pos);
        system_with_log(gCmdStr);
    }
	fp=fopen("/tmp/pin.txt","r");
	while(i<8)
	  {
		 wps_read_pin[i]=fgetc(fp);
		 i++;
	  }
	wps_read_pin[i]='\0';
	fclose(fp);
	strcpy(wps_pin,wps_read_pin);
	LEAVE( __func__ );
	return TRUE;
}

int set_supplicant_readpin (char *ifname, char *wps_pin) {
	ENTER( __func__ );
    sprintf(gCmdStr, "%s/%s -i %s wps_pin any %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,wps_pin);
    system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_p2p_oper_channel( char *ifname, int channel) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s -i %s set p2p_oper_channel %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,channel);
    system_with_log(gCmdStr);
	return TRUE;
}

int enable_supplicant_p2p_go ( char *ifname, char * ssid, int channel) {
	ENTER( __func__ );
	int network_index;
	sprintf(gCmdStr,"%s/%s -i %s p2p_flush",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
    system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s -i %s set p2p_oper_channel %d",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,channel);
    system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s -i %s p2p_find",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
    system_with_log(gCmdStr);
	sleep(2);
    sprintf(gCmdStr,"%s/%s -i %s p2p_peers discovered",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
    system_with_log(gCmdStr);
	sprintf(gCmdStr, "%s/%s -i %s p2p_set ssid_postfix %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,ssid);
	system_with_log(gCmdStr);
    sprintf(gCmdStr,"%s/%s -i %s p2p_group_add",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
    system_with_log(gCmdStr);
    network_index = get_active_network_index();	    
    if (network_index == -1 ) {
	    return FALSE;
    }
	LEAVE( __func__ );
	return TRUE;
}

void update_virtual_interface(void) {
	ENTER( __func__ );
	FILE *pipe = NULL;
	int len=0, cnt=0 ,c = 0;
    char path[128] = {0}, *pos ;
	
	pos = strstr(mrvl_WS_info->p2p_ctrl_interface,"/");
	if ( pos != NULL ) {
		len = strlen(mrvl_WS_info->p2p_ctrl_interface); 	
	  	while( mrvl_WS_info->p2p_ctrl_interface[len] != '/' ) { len-- ; }  
		while( cnt <= len ) { path[cnt] = mrvl_WS_info->p2p_ctrl_interface[cnt] ; cnt++; }		
		sprintf(gCmdStr,"%s%s",path,mrvl_WS_info->p2p_virtual_interface);
		strcpy(mrvl_WS_info->p2p_ctrl_interface,gCmdStr);
	} else {
		strcpy(mrvl_WS_info->p2p_ctrl_interface,mrvl_WS_info->p2p_virtual_interface);			
	}
	strcpy(mrvl_WS_info->p2p_interface,mrvl_WS_info->p2p_virtual_interface);		
	LEAVE( __func__ );
}
int get_active_network_index(void) {
	ENTER( __func__ );
	FILE *pipe = NULL;
	int c, network_index = -1;
	char tmpStr[150] = {0};
	char *str;
	update_virtual_interface();	
	sprintf(gCmdStr, "%s/%s -i %s list_all_networks | %s grep CURRENT | %s cut -dD -f1",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin, mrvl_WS_info->p2p_ctrl_interface,DUT_CMD_PREFIX,DUT_CMD_PREFIX);
	pipe = popen(gCmdStr, "r");
	c = fgets(tmpStr, 150, pipe);
	pclose(pipe);
	if (!c) {
		sprintf(gCmdStr, "%s/%s -i %s list_networks | %s grep CURRENT | %s cut -dD -f1", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin, mrvl_WS_info->p2p_ctrl_interface, DUT_CMD_PREFIX, DUT_CMD_PREFIX);
		pipe = popen(gCmdStr, "r");
		c = fgets(tmpStr, 150, pipe);
		pclose(pipe);
		if (!c) {
			LEAVE( __func__ );
			return network_index;
		}
	} 
	str = strtok(tmpStr, " ");
	network_index = atoi(str); /* Uglyyy. If successful, network index is alway at first byte */
	mrvl_WS_info->network_index = network_index;
	LEAVE( __func__ );
	return network_index;
}

int get_supplicant_p2p_groupid (char *ifname, char *groupId ) {
	char ssid[128] = "";
    FILE *tmpfd;
    char *p;

	ENTER( __func__ );
	sprintf(gCmdStr, "%s/%s -i %s get_network %d ssid > /tmp/ssid.txt", APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin, ifname, mrvl_WS_info->network_index);
    system_with_log(gCmdStr);
    tmpfd = fopen("/tmp/ssid.txt", "r");
    if (!tmpfd) {
        printf("Could not open /tmp/ssid.txt");
        return FALSE;
    }
    fgets(ssid, 128, tmpfd);
    p = strtok(ssid, "\"");
    if (p)
	strcpy(ssid, p);
    fclose(tmpfd);
		
	sprintf(gCmdStr,"%s %s",mrvl_dut_info->p2p_dev_address,ssid);
	strcpy(groupId,gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int invoke_sendInvReq(char *ifname, char *devId) {
	ENTER( __func__ );
	
	sprintf(gCmdStr,"%s/%s -i %s p2p_find",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);		
	sleep(5);
			
	sprintf(gCmdStr,"%s/%s -i %s p2p_peer %s ",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,ifname,devId);
	system_with_log(gCmdStr);
	
	sprintf(gCmdStr,"%s/%s -i %s p2p_invite group=%s peer=%s",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,ifname,mrvl_WS_info->p2p_interface,devId);
	system_with_log(gCmdStr);
		
	LEAVE( __func__ );
	return TRUE;	
}

int reinvoke_sendInvReq(char *ifname, char *devId) {
	ENTER( __func__ );
	int i,persistent_index;
	char string[128],*str;
	FILE *pipe; 
	
	sprintf(gCmdStr, "%s/%s -i %s p2p_group_remove %s",APP_BIN_LOC, mrvl_WS_info->supplicant_cli_bin,ifname,mrvl_WS_info->p2p_interface);   
	system_with_log(gCmdStr);
	update_virtual_interface();
	
	sprintf(gCmdStr,"%s/%s -i %s list_networks",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,mrvl_WS_info->p2p_ctrl_interface);
	system_with_log(gCmdStr);

	sprintf(gCmdStr,"%s/%s -i %s p2p_find",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,mrvl_WS_info->p2p_ctrl_interface);
	system_with_log(gCmdStr);		
	sleep(5);

	sprintf(gCmdStr,"%s/%s -i %s p2p_peer %s | %s grep persistent | %s cut -d= -f2",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,mrvl_WS_info->p2p_ctrl_interface,devId,DUT_CMD_PREFIX,DUT_CMD_PREFIX);
	
	pipe = popen(gCmdStr, "r");
	i = fgets(string, 150, pipe);
	pclose(pipe);
	if (!i) {
		persistent_index = 0;		
	} else {
		str = strtok(string, " ");
		persistent_index = atoi(str);
	}
	sprintf(gCmdStr,"%s/%s -i %s p2p_invite persistent=%d peer=%s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin, mrvl_WS_info->p2p_ctrl_interface,persistent_index,devId);
	system_with_log(gCmdStr);
	sleep(10);
	
	LEAVE( __func__ );
	return TRUE;	
}

int invoke_acceptInvReq (char *ifname, char *devId) {
	ENTER( __func__ );

	sprintf(gCmdStr,"%s/%s -i %s p2p_find",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sleep(5);
	sprintf(gCmdStr,"%s/%s -i %s p2p_peer %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s -i %s p2p_listen",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	
	if (!strcmp(runtime_test_data->wps_method,WPS_PBC)) {   
		sprintf(gCmdStr, "%s/%s -i %s wps_pbc", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s join auth", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId);
		system_with_log(gCmdStr);    
	}
	if (!strcmp(runtime_test_data->wps_method,ENTER_WPS_PIN)) {   
		sprintf(gCmdStr, "%s/%s -i %s wps_pin any %s", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,runtime_test_data->wps_pin);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s join auth", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId,runtime_test_data->wps_pin);
		system_with_log(gCmdStr);    
	}
	if (!strcmp(runtime_test_data->wps_method,READ_WPS_PIN)) {   
		sprintf(gCmdStr, "%s/%s -i %s wps_pin any %s",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,runtime_test_data->wps_pin);
		system_with_log(gCmdStr);
		sprintf(gCmdStr, "%s/%s -i %s p2p_connect %s %s join auth", APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname,devId,runtime_test_data->wps_pin);
		system_with_log(gCmdStr);    
	}
	LEAVE( __func__ );
	return TRUE;
}

void preset_wfd( char *ifname ) {
	ENTER( __func__ );	
	sprintf(gCmdStr,"%s/%s -i %s SET wifi_display 1",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s -i %s WFD_SUBELEM_SET 0 000600111c4400c8",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
}

void get_discovered_list( char *ifname,  char *devlist ) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s -i %s p2p_peers | sed ':a;N;$!ba;s/\\n/ /g' > /tmp/p2p_devlist.txt",APP_BIN_LOC,mrvl_WS_info->supplicant_cli_bin,ifname);
	system_with_log(gCmdStr);
	int ret;
	if( devlist ) {
		ret = read_line_from_file( "/tmp/p2p_devlist.txt", devlist, 256 );
	}
	DPRINT_INFO(WFA_OUT, "%s devlist:[%s]", __func__, devlist );
	LEAVE( __func__ );
}
