extern int enable_mlan_reassoc_ctrl(char *);
extern int check_wlan_connectivity (char *);
extern int enable_mlan_association(char *, char *);
extern int fetch_mlan_ip_connection_info (caStaGetIpConfigResp_t *, char *,int);
extern int set_static_ip_config ( char *, caStaSetIpConfig_t * );
extern int check_ping_connectivity ( char *, unsigned int );
extern int fetch_mac_address ( char *, char*);
extern int fetch_sta_rssi( char *, char*);
extern int get_mlan_driver_version ( char *, char * );
extern int set_mlan_uapsd(char *, int);
extern int get_mlan_bssid_info ( char *, char *);
extern int configure_powersave_mode ( char *, int);
extern int configure_wmm_group (char *, int, int, int);
extern int configure_11n_settings(int , BYTE *, int *, BYTE *);
extern int configure_vht_settings(int , BYTE *, int *, BYTE *);
extern int configure_he_settings(int , BYTE *, int *, BYTE *);
extern int preset_vht_parameters(int , BYTE *, int *, BYTE *);
extern int set_vht_nss_config (char *, int , int ) ;
extern int set_mlan_deauth ( char * );
//extern int fetch_mbss_scan_res( char *, char*);


