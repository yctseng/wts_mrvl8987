
CONF=$1

if [ "$CONF" = "" ]; then
    echo "Usage: $0 CONFIG_FILE"
    exit 1
fi

SECURITY=`cat $CONF | grep wpa_key_mgmt | awk -F"=" '{print $2}'`
TESTCASE=`cat $CONF | grep ssid | awk -F"=" '{print $2}' | awk -F"_" '{print $1}'`
# config PBAC according to chip support, NXP 88w8987 doesn't support
PBAC=0

if [ "$SECURITY" = "WPA-PSK" -a "$TESTCASE" = "Wi-Fi-4.2.3" ]; then
    if [ $PBAC -eq 0 ]; then
        sed -i -e 's/ieee80211w=[012]/ieee80211w=0/g' $CONF
    fi
fi

