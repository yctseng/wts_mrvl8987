#define CERTIFICATES_PATH  ""

// open-source supplicant details
#define SUPPLICANT_CONF    "wpa_supplicant.conf"
#define SUPPLICANT_BIN     "./wpa_supplicant"

#ifdef MV_PLATFORM_ANDROID        
    #define SUPPLICANT_EXT  "nl80211 -ddd"
#else
    #define SUPPLICANT_EXT  "wext"	
#endif 

#define HOSTAPD_PATH            ""
#define HOSTAPD_BIN             "./hostapd"
#define HOSTAPD_CONF_TEMPLATE   "hostapd.conf"
#define HOSTAPD_CONF_ENT        "hostapd-ent.conf"
#define HOSTAPD_AS_IP_CONF      "hostapd-as-ip.conf"
#define HOSTAPD_AS_PORT_CONF    "hostapd-as-port.conf"
#define HOSTAPD_AS_PASSWD_CONF  "hostapd-as-passwd.conf"
#define HOSTAPD_CONF            "hostapd.conf"

#define  MIN_A_CHANNEL 36
#define  MAX_A_CHANNEL 252

#define SIGMA_USER_CONFIG       "user_config.conf"

#define MRVL_WFD_PROG	"WFD"   
#define MRVL_P2P_PROG	"P2P"
#define MRVL_HE_PROG	"HE"
#define MRVL_VHT_PROG	"VHT"
#define MRVL_11N_PROG	"11N"
#define MRVL_PMF_PROG	"PMF"
#define MRVL_WMM_PS_PROG "WMM-PS"	
#define MRVL_TDLS_PROG	"TDLS"
#define MRVL_HS2_PROG   "HS2-R2"
#define MRVL_HS1_PROG   "HS2-R1"
#define MRVL_V_E_PROG   "V-E"
#define MRVL_WPA3_PROG  "WPA3"
#define MRVL_MBO_PROG   "MBO"

#define WPA_SUPPLICANT  "WS" 
#define MWUD  "MWU"	 	 

#define P2P_SUPPLICANT_CONF "p2p.conf"
#define SUPP_LOG_FILE  "supplicant.log"

#define APP_BIN_LOC "."
#define MLANUTL "mlanutl" 
#define IWPRIV "iwpriv"
#define UAPUTL "uaputl.exe"
  
#define AES    "aes" 
#define TKIP   "tkip"
#define WPA2   "wpa2"
#define WPA2_FT     "wpa2-ft"
#define WPA2_PSK    "wpa2-psk"
#define WPA2_SHA256 "wpa2-sha256"
#define WPA2_EAP    "wpa2-eap"

#define WPA2_PSK "1234567890"
#define ENTER_WPS_PIN "0008"
#define READ_WPS_PIN "0100"
#define WPS_PBC  "0080"
#define DEFAULT_CHAN 11
#define DEFAULT_INTENT 7
#define WPA_CLI_APPROACH    0
#define CONF_FILE_APPROACH   1
   
#ifdef MV_PLATFORM_ANDROID        
	#define DUT_CMD_PREFIX  "busybox"
	#define SUPPLICANT_CTRL_PATH "/data/misc/wifi/sockets"
	#define HOSTAPD_CTRL_PATH    "/data/misc/wifi/sockets"
#else
	#define DUT_CMD_PREFIX  ""
	#define SUPPLICANT_CTRL_PATH "/var/run/wpa_supplicant"
	#define HOSTAPD_CTRL_PATH    "/var/run/hostapd"
#endif 

  
/** HS2 */  
#define MASK_QUERY_LIST	""
#define MASK_CAPA_LIST	"257"
#define MASK_VENUE_NAME	"258"
#define MASK_EMERG_CALL	""
#define MASK_NETWORK_AUTH	""
#define MASK_ROAMING_CONS	""
#define MASK_IP_TYPE	""
#define MASK_NAI_REALM	"263"
#define MASK_3GPP_NETWORK	"264"
#define MASK_DOMAIN_INFO	"268"
#define MASK_VENDOR_SPEC	""
#define MASK_DLS	""
#define MASK_WNM	""

/** WFA ANQP Vendor specific Element Subtype Definition */
#define MASK_WIFI_HS_QUERY_LIST	""
#define MASK_WIFI_HS_CAPA_LIST	"2"
#define MASK_WIFI_HS_OP_FRIENDLY_NAME	"3"
#define MASK_WIFI_HS_WAN_METRICS	"4"
#define MASK_WIFI_HS_CONNECTION_CAPA	""
#define MASK_WIFI_HS_OP_CLASS_IND	"7"
#define MASK_WIFI_HS_OSU	"8"
#define MASK_WIFI_HS_ANONYMOUS_NAI	""
#define MASK_WIFI_HS_NAI_HOME_REALM_QUERY	""
#define MASK_WIFI_HS_ICON	""
#define MASK_WIFI_HS_ICON_DEFAULT	""
#define MASK_WIFI_HS_ICON_SPECIFIED	""
 
#define HS2_R2_CERTPATH  "SP/wi-fi.org"  
#define HS20_OSU_CLIENT  "hs20-osu-client" 
