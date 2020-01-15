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
#ifdef WFA_WMM_PS_EXT
#include "wfa_wmmps.h"
#endif


int clear_mwud_instance (char *ifname) {
	ENTER( __func__ );
	
	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=enrollee_deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	
	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);

	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=sta_deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);

	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=ap_deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);

	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);

	sprintf(gCmdStr, "%s %s passphrase 2",IWPRIV,ifname);
	system_with_log(gCmdStr);

	sprintf(gCmdStr, "%s killall mwud",DUT_CMD_PREFIX);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int launch_mwud_service (char *ifname) {
	ENTER( __func__ );
	sprintf(gCmdStr, "%s &",mrvl_mwud_info->mwud_service);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int init_mwud_grp_pd_request(char *ifname, char *devId) {
	ENTER( __func__ );

	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=pd_req device_address=%s methods=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,devId,runtime_test_data->wps_method);
	system_with_log(gCmdStr);
	
	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=negotiate_group device_address=%s methods=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,devId,runtime_test_data->wps_method);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int allow_peers_connections (char *ifname, char *devId) {
	ENTER( __func__ );

	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=allow device_address=%s methods=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,devId,runtime_test_data->wps_method);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int init_mwud_wps_enrollee (char *ifname, char *devId) {
	ENTER( __func__ );
	int cnt;
	char result[32],status[32],psk_key[128],ssid[32];
	char interface_addr[32];
	
	/** Get the interface address of Peer device */
	sprintf(gCmdStr, "%s/%s module=wifidirect iface=%s cmd=dump_peers device_address=%s > /tmp/iface_addr.txt",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,devId);
	system_with_log(gCmdStr);
	read_config("/tmp/iface_addr.txt","interface_address",interface_addr);
          	
	sprintf(gCmdStr,"%s/%s module=mwu cmd=clear_events",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin);
	system_with_log(gCmdStr);

	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=enrollee_init device_name=Marvell-Embedded model_name=Marvell_SoC model_number=0001 methods=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,mrvl_dut_info->config_methods);
	system_with_log(gCmdStr);
	
	if(!strcmp(runtime_test_data->wps_method,WPS_PBC)) {
		sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=enrollee_start mac=%s pin=00000000",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,interface_addr);
		system_with_log(gCmdStr);
	} else {
		sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=enrollee_start mac=%s pin=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,interface_addr,runtime_test_data->wps_pin);
		system_with_log(gCmdStr);
	}
	
	cnt=0;
	while( cnt < 120 && cnt !=-1 ) {
		sleep(1);
		sprintf(gCmdStr,"%s/%s module=mwu cmd=get_next_event > /tmp/enrollee.txt",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin);
		system_with_log(gCmdStr);
			
		read_config("/tmp/enrollee.txt","event",result);
		read_config("/tmp/enrollee.txt","status",status);
		
		result[strlen("enrollee_done")] = '\0';
		status[strlen("0")] = '\0' ;	

		if( !strcmp(result,"enrollee_done") && !strcmp(status,"0") ) {	
			printf("\nGOT Enrollee Event ( %d ) secs\n",cnt+1);
			read_config("/tmp/enrollee.txt","ssid",ssid);
			read_config("/tmp/enrollee.txt","key",psk_key);
			cnt=-2;
		} 
		cnt++;	
	}
	
	if(  cnt != -1  ) {
	    printf("\n ENROLLEE FAILURE\n");
        return FALSE;			
	}

	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=enrollee_deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=sta_init",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=sta_connect ssid=%s key=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,ssid,psk_key);
	system_with_log(gCmdStr);
	strcpy(runtime_test_data->ssid,ssid);
	LEAVE( __func__ );
	return TRUE;
}

int mwu_init_ftm(char *ifname, int BurstsExponent,int asap,int FormatBwFTM,int askForLocCivic,int askForLCI) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s module=ftm "
             "iface=%s "
             "cmd=init "
             "burst_exp=%d "
             "burst_duration=10 "
             "min_delta=10 "
             "asap=%d "
             "mlocation_per_burst=10 "
             "bw=%d "
             "burst_period=5 "
             "civic_location=%d "
             "civic_location_type=0 "
             "country_code=0 "
             "ca_type=0 "
             "ca_length=0 "
             "ca_value=0 "
             "lci_request=%d "
             "latitude=0 "
             "longitude=0 "
             "lat_unc=0 "
             "long_unc=0 "
             "altitude=0 "
             "alt_unc=0 "
             "range_drive_var=1.0 "
             "range_measurement_var=0.0001 "
             "range_rate_init=0.2 ",
			 APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,BurstsExponent,asap,FormatBwFTM,askForLocCivic,askForLCI);
	system_with_log(gCmdStr);
	sleep(2);		
	LEAVE( __func__ );
	return TRUE;
}

int mwu_start_ftm(char *ifname, char *mac){
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s module=ftm "
					"iface=%s "	
					"cmd=mlocation_session "	
					"mac=%s "
					"channel=0 ",
					APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,mac);
	system_with_log(gCmdStr);
	sleep(2);
	LEAVE( __func__ );
	return TRUE;
}

int mwu_send_anqp(char *ifname, char *mac, int civic_req, int lci_req) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s module=ftm "
				"iface=%s "	
				"cmd=mlocation_send_anqp_request "	
				"mac=%s "
				"civic_req=%d "
				"lci_req=%d ",
				APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,mac,civic_req,lci_req);
				
	system_with_log(gCmdStr);
	sleep(2);
	LEAVE( __func__ );
	return TRUE;	
}

int init_mwud_wifidirect ( char *ifname, int oper_chan, int listen_chan, int intent, char *wps_methods) {
	ENTER( __func__ );

	/** Deinit previous instance */ 
	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	sleep(2);
	
	sprintf(gCmdStr, "%s/%s module=wifidirect iface=%s cmd=init name=Marvell-Embedded intent=%d op_chan=%d listen_chan=%d methods=%s go_ssid=DIRECT- manuf=Marvell model_name=Marvell_SoC model_num=0001 serial_num=0001 key=1234567890",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,intent,oper_chan,listen_chan,wps_methods);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int fetch_mwud_wifidirect_capabilities (char *ifname) {
	ENTER( __func__ );
	char value[32];
	
	/** Get P2P group and device capabilities */
	sprintf(gCmdStr, "%s/%s module=wifidirect iface=%s cmd=p2p_get_cfg > /tmp/p2p_cap",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);

	read_config("/tmp/p2p_cap", "group_cap", value);
	runtime_test_data->grpCapability = strtol(value, NULL, 16);

	read_config("/tmp/p2p_cap", "device_cap", value);
	runtime_test_data->devCapability = strtol(value, NULL, 16);
	
	LEAVE( __func__ );
	return TRUE;
}

int set_mwud_wifidirect_capabilities (char *ifname) {
	ENTER( __func__ );
	
	sprintf(gCmdStr, "%s/%s module=wifidirect iface=%s cmd=p2p_set_cfg device_capability=0x%x group_capability=0x%x", APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname, runtime_test_data->devCapability,runtime_test_data->grpCapability);
	system_with_log(gCmdStr);
	
	LEAVE( __func__ );
	return TRUE;
}

int start_mwud_discover(char *ifname) {
	ENTER( __func__ );
	
	sprintf(gCmdStr, "%s/%s module=wifidirect iface=%s cmd=start_find",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);

	/** Scanning the devices please wait for some time... */
	sleep(3);
	sprintf(gCmdStr, "%s/%s module=wifidirect iface=%s cmd=dump_peers device_address=00:00:00:00:00:00",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	   
	LEAVE( __func__ );
	return TRUE;
}

int start_mwud_autogo (char *ifname, char *ssid) {
	ENTER( __func__ );
	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=start_group ssid=DIRECT-xy%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,ssid);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=ap_init ssid=DIRECT-xy%s key=1234567890",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,ssid);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=ap_start",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_set_pin pin=",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	LEAVE( __func__ );
	return TRUE;
}

int set_mwud_wps_registrar( char *ifname, char *ssid ) 
{
	ENTER( __func__ );
		
	int cnt;
	char status[5],result[25],lmethod[10];

	/** Clear the Event buffer */	
	sprintf(gCmdStr,"%s/%s module=mwu cmd=clear_events",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin);
	system_with_log(gCmdStr);

	/** Deinit the registrar */ 
	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	
	/** Need to check for this implementation */
	if (!strcmp(runtime_test_data->wps_method,WPS_PBC)) {
		strcpy(lmethod,mrvl_dut_info->config_methods);
	} else {
		strcpy(lmethod,"0108");
	}
	
	/** Initialize and Set the registrar */
	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_init device_name=Marvell-Embedded model_name=Marvell_SoC model_number=0001 methods=%s ssid=DIRECT-xy%s auth=0020 encrypt=0008 key=1234567890",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,lmethod,ssid);  
	system_with_log(gCmdStr);

	if(!strcmp(runtime_test_data->wps_method,WPS_PBC)) {
		sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_set_pin pin=00000000",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
		system_with_log(gCmdStr);                    
	}

	if(!strcmp(runtime_test_data->wps_method,READ_WPS_PIN)) {
		sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_set_pin pin=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,runtime_test_data->wps_pin);
		system_with_log(gCmdStr);
	}
		
	if( !strcmp(runtime_test_data->wps_method,ENTER_WPS_PIN)) {
		sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_set_pin pin=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,runtime_test_data->wps_pin);
		system_with_log(gCmdStr);
	}
	
	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_start",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr); 
	
	if(!runtime_test_data->autogo) {
		cnt=0;
		while( cnt < 120 && cnt !=-1 ) {
			sleep(1);
			sprintf(gCmdStr,"%s/%s module=mwu cmd=get_next_event > /tmp/registrar.txt",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin);
			system_with_log(gCmdStr);
	
			read_config("/tmp/registrar.txt","event",result);
			read_config("/tmp/registrar.txt","status",status);
	
			result[strlen("registrar_done")] = '\0';
			status[strlen("0")] = '\0' ;	

			if( !strcmp(result,"registrar_done") && !strcmp(status,"0") ) {	
				printf("\n GOT REGISTRAR EVENT ( %d ) secs ",cnt+1);
				cnt=-2;
			} 
			cnt++;	
		}	

	    if ( cnt != -1 ) 
	    printf("\n REGISTRAR FAILURE");
					
	    if(!strcmp(runtime_test_data->wps_method,WPS_PBC)) {
			sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_set_pin pin=00000000",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
			system_with_log(gCmdStr);                    
		}
		
		if(!strcmp(runtime_test_data->wps_method,READ_WPS_PIN)) {
			sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_set_pin pin=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,	runtime_test_data->wps_pin);
			system_with_log(gCmdStr);
		}
	
		if( !strcmp(runtime_test_data->wps_method,ENTER_WPS_PIN)) {
			sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_set_pin pin=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,runtime_test_data->wps_pin);
			system_with_log(gCmdStr);
		}
	   
	    /** Deinit the registrar */ 
		sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_deinit",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
		system_with_log(gCmdStr);
  
        /** Initialize and Set the registrar */
		sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=registrar_init device_name=Marvell-Embedded model_name=Marvell_SoC model_number=0001 methods=%s ssid=DIRECT-xy%s auth=0020 encrypt=0008 key=1234567890",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,lmethod,ssid);  
		system_with_log(gCmdStr);
	}	
	
	LEAVE( __func__ );
	return TRUE;	
}

int init_mwud_grp_negotiation ( char *ifname, char *devId, char *role) {
	ENTER( __func__ );
	int cnt;	
	char result[16],status[16],ssid[32];
	
	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=pd_req device_address=%s methods=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,devId,runtime_test_data->wps_method);
	system_with_log(gCmdStr);

	sprintf(gCmdStr,"%s/%s module=mwu cmd=clear_events",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin);
	system_with_log(gCmdStr);	
	
	sprintf(gCmdStr,"%s/%s module=wifidirect iface=%s cmd=negotiate_group device_address=%s methods=%s",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname,devId,runtime_test_data->wps_method);
	system_with_log(gCmdStr);
	 
	cnt=0;
	while( cnt < 60 && cnt !=-1 ) {
		sprintf(gCmdStr,"%s/%s module=mwu cmd=get_next_event > /tmp/read_role.txt",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin);
		system_with_log(gCmdStr);
		sleep(1);  
		
		read_config("/tmp/read_role.txt","event",result);
		read_config("/tmp/read_role.txt","status",status);
		status[1]='\0';
		result[10]='\0';
		if( !strcmp(result,"neg_result") && !strcmp(status,"0") )
		{	
			printf("\nGot neg_result Event ( %d ) sec\n",cnt+1);
			read_config("/tmp/read_role.txt","is_go",result);
			read_config("/tmp/read_role.txt","go_ssid",ssid);
			result[4]='\0';
			cnt=-2;
		}
		cnt++;
	}
	
	if(cnt == -1) {
		if(!strcmp(result,"true")) {
			strcpy(role,"GO");
			strcpy(runtime_test_data->ssid,ssid);
		} else { 
			strcpy(role,"CLIENT");
		}
	} else {
		return FALSE;
	}
	
	LEAVE( __func__ );
	return TRUE;
}

int start_mwud_go ( char *ifname, char *ssid ) {
	ENTER( __func__ );

	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=ap_init ssid=%s key=1234567890",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	
	sprintf(gCmdStr,"%s/%s module=mwpamod iface=%s cmd=ap_start",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);

	set_mwud_wps_registrar(ifname,ssid);
	
	LEAVE( __func__ );
	return TRUE;
}

int get_mwud_read_pin (char *ifname, char *wps_pin) {
	ENTER( __func__ );

	int i=0;
	FILE *fp;
	sprintf(gCmdStr,"%s/%s module=mwpsmod iface=%s cmd=generate_pin  > /tmp/pin.txt",APP_BIN_LOC,mrvl_mwud_info->mwu_cli_bin,ifname);
	system_with_log(gCmdStr);
	
	fp=fopen("/tmp/pin.txt","r");
	while(i<8) 
	{
		wps_pin[i]=fgetc(fp);
		i++;
	}
	wps_pin[i]='\0';
	fclose(fp);
	
	LEAVE( __func__ );
	return TRUE;
}
