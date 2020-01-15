/* New Variables */ 
extern char SIGMA_PROG_NAME[];
extern char SIGMA_PROG_UTILITY[];
extern int  HOSTAPD;
extern int  MWIFIEX;

struct mrvl_sigma_dut_info {
	char sta_interface[32];
	char p2p_interface[32];
	char mmh_interface[32];
	char p2p_dev_address[32];
	char sta_dev_address[32];
	char mwlan_info_path[256];
	int  use_supplicant;
	char build_load[256];
	char build_unload[256];
	char config_methods[8];
	char model[30];
};

struct mrvl_sigma_dhcp_info {
	char dhcp_srv_ip[32];
	char dhcp_range[64];
	char dhcpd_lease_file[256];
	char dhclient_lease_file[256];
};

struct mrvl_sigma_WS_info {
	char supplicant_bin[32];
	char supplicant_cli_bin[32];
	char supplicant_ext[32];
	char supplicant_conf[32];
	char p2p_ctrl_interface[64];
	char p2p_interface[64];
	char sta_ctrl_interface[64];
	char p2p_virtual_interface[64];
	int network_index;
	int credential_index;
	int supplicant_config_approach;
};

struct mrvl_sigma_mwud_info {
	char mwu_bin[32];
	char mwu_cli_bin[32];
	char mwu_conf_wifidir[256];
	char mwu_conf_wifidirect_defaults[256];
	char mwud_service[256];
};

struct mrvl_sigma_hostapd_info {
	char hostapd_bin[32];
	char hostapd_cli_bin[32];
	char hostapd_ext[32];
	char hostapd_conf[32];
	char hostapd_conf_ent[32];
	char hostapd_as_ip_conf[32];
	char hostapd_as_port_conf[32];
	char hostapd_as_password_conf[32];
};

struct mrvl_sigma_debug_info {
	int console_logging;
	int function_level_debug;
	int runtime_config;
};

struct sigma_runtime_test_data {
	char wps_method[32];
	char wps_pin[32];
	char neg_res[8];
	int autogo;
	int dhcp;
	int oper_chn;
	int listen_chn;
	int reinvoke;
	int grpCapability;
	int devCapability;
	int eap_test;
	int roam;
	char ssid[32];
    char preset_non_pref_chan_list[50];
    char rfeat_non_pref_chan_list[50];
};

extern struct mrvl_sigma_dut_info* mrvl_dut_info;
extern struct mrvl_sigma_dhcp_info* mrvl_dhcp_info;
extern struct mrvl_sigma_WS_info* mrvl_WS_info;
extern struct mrvl_sigma_mwud_info* mrvl_mwud_info;
extern struct mrvl_sigma_hostapd_info* mrvl_hostapd_info;
extern struct mrvl_sigma_debug_info* mrvl_debug_info;
extern struct sigma_runtime_test_data* runtime_test_data;

/* Old Variables */
extern char g_cmd_str_ret[];
extern char * grpCipher[];
extern char sta_saved_ssid[];
extern char sta_saved_rxsp_stream[];
extern char sta_saved_txsp_stream[];
extern char ap_saved_rxsp_stream[];
extern char ap_saved_txsp_stream[];
extern unsigned int ap_saved_txmcsmap;


extern int ap_vht_bw;
extern int ap_ch_width;
extern int sta_ch_width;

extern int running_vht;
extern int invalid_for_11ac;

// Bandwidth Global
extern int width;
extern int bw_signal;
extern int bw_signal_flag;
extern int dyn_bw_signal;
extern int dyn_bw_signal_flag;
extern int is_vht_tkip;

// STA/AP type: Testbed/DUT
extern char sta_type[];
extern char ap_type[];
extern char prog_name[];

extern int ap_sgi80_hack; /* we do not want to use auto rate when sgi80 enable comes on AP */
extern unsigned char ap_mcs_fixedrate;

/* uap default wmm params */ 
extern wmm_ac_t   vi;
extern wmm_ac_t   vo;
extern wmm_ac_t   be;
extern wmm_ac_t   bk;

extern ap_vi_flag;
extern ap_vo_flag;
extern ap_be_flag;
extern ap_bk_flag;

extern unsigned short wfa_defined_debug;
// flag to indicate whether or not to use open-source supplicant
extern int useSupplicant;
// flag to indicate whether or not to use supplicant for enterprise mode
extern int useSupplicantEnt;
/* This is the flag to indicate whether wpswfd command has been executed */
extern int runWpswfd;
extern int runCmdsAll;
extern int autoGO;

// declared my my //

extern char methods[];
extern char wps_pin[];
extern char wps_read_pin[];
extern int i_am_autogo;
extern char autossid[];
extern char my_device_mac[];

extern int  intent;
extern int  op_chan;
extern int  listen_chan;

extern char default_method[];
extern char name[];
extern char model_name[];
extern char model_number[];
extern char wpa2_key[];
extern char p2p_interface[];
extern char dhcp_srv_ip[];
extern char dhcp_range[];
extern char auth[];
extern char encrypt[];
extern char ssid_prefix[];

extern char sta_interface[];
extern char driver1_load[];
extern char driver2_load[];
extern char driver1_unload[];
extern char driver2_unload[];

// declared by my //

extern char e2eResults[];
//extern char *e2eResults;
extern FILE *e2efp;

/* Since the two definitions are used all over the CA function */
extern char gCmdStr[];
extern dutCmdResponse_t gGenericResp;

extern char saved_mcs_fixed_rate[];
extern char saved_txsp_stream[];
extern int ap_use_hostapd;

