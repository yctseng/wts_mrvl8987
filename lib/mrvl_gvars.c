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

/* New Variables */ 

char SIGMA_PROG_NAME[32];
char SIGMA_PROG_UTILITY[8];
int MWIFIEX =0;
int HOSTAPD =0;
struct mrvl_sigma_dut_info *mrvl_dut_info;
struct mrvl_sigma_dhcp_info *mrvl_dhcp_info;
struct mrvl_sigma_WS_info *mrvl_WS_info;
struct mrvl_sigma_mwud_info *mrvl_mwud_info;
struct mrvl_sigma_hostapd_info *mrvl_hostapd_info;
struct mrvl_sigma_debug_info *mrvl_debug_info;
struct sigma_runtime_test_data *runtime_test_data;

/* Old Variables */

char g_cmd_str_ret[128];
char grpCipher[3][32] = { "CCMP" , "TKIP" , "" };

char sta_saved_ssid[33];
char sta_saved_rxsp_stream[10] ={0};
char sta_saved_txsp_stream[10] ={0};

char ap_saved_rxsp_stream[10] = {0};
char ap_saved_txsp_stream[10] = {0};
unsigned int ap_saved_txmcsmap = 0x0;
int ap_vht_bw = 1;
int ap_ch_width = 80;
int sta_ch_width = 80;

int running_vht = 0;
int invalid_for_11ac = 0;

// Bandwidth Global
int width = 0;
int bw_signal = 0;
int bw_signal_flag = 0;
int dyn_bw_signal = 0;
int dyn_bw_signal_flag = 0;
int is_vht_tkip = 0;

// STA/AP type: Testbed/DUT
char sta_type[16];
char ap_type[16];
char prog_name[16];

int ap_sgi80_hack = 0; /* we do not want to use auto rate when sgi80 enable comes on AP */
unsigned char ap_mcs_fixedrate = 0xff;

/* uap default wmm params */ 
wmm_ac_t   vi = {2, 0, 3, 0, 2, 0, 47, 0};
wmm_ac_t   vo = {3, 0, 4, 0, 2, 0, 94, 0};
wmm_ac_t   be = {4, 0, 10, 0, 3, 0, 0, 0};
wmm_ac_t   bk = {4, 0, 10, 0, 7, 0, 0, 0};
ap_vi_flag = 0;
ap_vo_flag = 0;
ap_be_flag = 0;
ap_bk_flag = 0;

extern unsigned short wfa_defined_debug;
int wfaExecuteCLI(char *CLI);
// flag to indicate whether or not to use open-source supplicant
int useSupplicant = 0;
// flag to indicate whether or not to use supplicant for enterprise mode
int useSupplicantEnt = 0;
/* This is the flag to indicate whether wpswfd command has been executed */
int runWpswfd = 0;
int runCmdsAll = 0;
int autoGO = 0;

// declared my my //

char methods[] = "0188";
char wps_pin[]="1234567890";
char wps_read_pin[] = "1234567890";
int i_am_autogo = 0;
char autossid[] = "DIRECT-xy00.00.00";
char my_device_mac[] = "00:00:00:00:00:00";

int  intent = 7;
int  op_chan = 11;
int  listen_chan = 11;

char default_method[10];
char name[50];
char model_name[20];
char model_number[25];
char wpa2_key[64];
char p2p_interface[25];
char dhcp_srv_ip[25];
char dhcp_range[50];
char auth[16];
char encrypt[16];
char ssid_prefix[25];

char sta_interface[25];
char driver1_load[100];
char driver2_load[100];
char driver1_unload[100];
char driver2_unload[100];

// declared by my //

extern char e2eResults[];
//extern char *e2eResults;
FILE *e2efp = NULL;

/* Since the two definitions are used all over the CA function */
char gCmdStr[WFA_CMD_STR_SZ];
dutCmdResponse_t gGenericResp;
int wfaTGSetPrio(int sockfd, int tgClass);
void create_apts_msg(int msg, unsigned int txbuf[],int id);

char saved_mcs_fixed_rate[10] = {0};
char saved_txsp_stream[10] = {0};
int ap_use_hostapd = 0;