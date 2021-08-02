
SSID=$1
PASSPHRASE=$2
MODE=$3
CHANNEL=$4
SECURITY=$5
PMF=$6


INIT_HOSTAPD_CONF="hostapd_init_manual.conf"
DEST_HOSTAPD_CONF="hostapd_manual.conf"

if [ "$CHANNEL" = "" -o "$MODE" = "" -o "$PASSPHRASE" = "" -o "$SSID" = "" ]; then
        echo "USAGE: $0 SSID PASSPHRASE [a|b|g|n] CHANNEL [WPA2/WPA3] [PMF_SETTING]"
	echo
	echo -e "\t[SECURITY]: WPA2 WPA2/WPA3 (default: WPA2)"
	echo -e "\t[PMF_SETTING]: Disable: 0  MFPC: 1   MFPR: 2 (default 0)"
	echo
        echo "e.g: $0 wi-fiwi- 12345678 b 6"
        echo "e.g: $0 wi-fiwi- 12345678 ac 36 WPA2/WPA3 2"
        exit 1
fi

mount -o rw,remount /
# check passphrase len
if [ ${#PASSPHRASE} -gt 63 ]; then
    echo "Invalid passphrase/password length: ${#PASSPHRASE} > 63"
    exit 1
fi

# set init value for SECURITY
if [ "$SECURITY" = "" ]; then
	SECURITY="WPA2"
fi

# set init value for PMF
if [ "$PMF" = "" ]; then
	PMF=0
fi


cp $INIT_HOSTAPD_CONF $DEST_HOSTAPD_CONF


# Config MODE
case "$MODE" in
        a)
                if [ $CHANNEL -le 14 ]; then
                        echo "Invalid combination mode:[$MODE] channel:[$CHANNEL]"
                        exit 1
                fi
                sed -i -e "s/^ieee80211n=.*/ieee80211n=0/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^hw_mode=.*/hw_mode=a/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^channel=.*/channel=$CHANNEL/g" $DEST_HOSTAPD_CONF
                ;;
        b)
                if [ $CHANNEL -gt 14 ]; then
                        echo "Invalid combination mode:[$MODE] channel:[$CHANNEL]"
                        exit 1
                fi
                sed -i -e "s/^ieee80211n=.*/ieee80211n=0/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^hw_mode=.*/hw_mode=b/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^channel=.*/channel=$CHANNEL/g" $DEST_HOSTAPD_CONF
                ;;
        g)
                if [ $CHANNEL -gt 14 ]; then
                        echo "Invalid combination mode:[$MODE] channel:[$CHANNEL]"
                        exit 1
                fi
                sed -i -e "s/^ieee80211n=.*/ieee80211n=0/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^hw_mode=.*/hw_mode=g/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^channel=.*/channel=$CHANNEL/g" $DEST_HOSTAPD_CONF
                ;;
        n)
                if [ $CHANNEL -gt 14 ]; then
                        sed -i -e "s/^hw_mode=.*/hw_mode=a/g" $DEST_HOSTAPD_CONF
                else
                        sed -i -e "s/^hw_mode=.*/hw_mode=g/g" $DEST_HOSTAPD_CONF
                fi
                sed -i -e "s/^ieee80211n=.*/ieee80211n=1/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^channel=.*/channel=$CHANNEL/g" $DEST_HOSTAPD_CONF
                ;;
        ac)
                if [ $CHANNEL -gt 14 ]; then
                        sed -i -e "s/^hw_mode=.*/hw_mode=a/g" $DEST_HOSTAPD_CONF
                else
                        sed -i -e "s/^hw_mode=.*/hw_mode=g/g" $DEST_HOSTAPD_CONF
                fi
                sed -i -e "s/^ieee80211n=.*/ieee80211n=1/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^ieee80211ac=.*/ieee80211ac=1/g" $DEST_HOSTAPD_CONF
                sed -i -e "s/^channel=.*/channel=$CHANNEL/g" $DEST_HOSTAPD_CONF
                ;;
        *)
                echo "Unknown mode:$MODE"
                exit 1
                ;;
esac

# Config SECURITY
case "$SECURITY" in
        WPA2)
		sed -i -e "s/^wpa_key_mgmt=.*/wpa_key_mgmt=WPA-PSK/g" $DEST_HOSTAPD_CONF
                ;;
        WPA3)
		sed -i -e "s/^wpa_key_mgmt=.*/wpa_key_mgmt=SAE/g" $DEST_HOSTAPD_CONF
		PMF=2
		;;
        WPA2/WPA3)
		sed -i -e "s/^wpa_key_mgmt=.*/wpa_key_mgmt=WPA-PSK SAE/g" $DEST_HOSTAPD_CONF
		if [ "$PMF" = "0" ]; then
			PMF=1
		fi
		;;
        *)
		echo "Invalid security:[$SECURITY]"
		exit 1
                ;;
esac

# Config PMF
sed -i -e "s/^ieee80211w=.*/ieee80211w=$PMF/g" $DEST_HOSTAPD_CONF

# Config SSID
sed -i -e "s/^ssid=.*/ssid=$SSID/g" $DEST_HOSTAPD_CONF

# Config PASSPHRASE
sed -i -e "s/^wpa_passphrase=.*/wpa_passphrase=$PASSPHRASE/g" $DEST_HOSTAPD_CONF


echo "SSID      :[$SSID]"
echo "PASSPHRASE:[$PASSPHRASE]"
echo "MODE      :[$MODE]"
echo "CHANNEL   :[$CHANNEL]"
echo "SECURITY  :[$SECURITY]"
echo "PMF       :[$PMF]"
# restart hostapd
systemctl stop wpa_supplicant

killall hostapd
sleep 2
./hostapd $DEST_HOSTAPD_CONF &
