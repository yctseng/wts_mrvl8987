
CONF=$1

if [ "$CONF" = "" ]; then
    echo "Usage: $0 CONFIG_FILE"
        exit 1
fi

SSID=`cat $CONF | grep ssid | awk -F"=" '{print $2}'`

if [ "$SSID" = "Iq9865hfg|?" ]; then
    /sbin/uaputl.exe aggrpriotbl 2 2 0 0 1 1 3 3 4 4 5 5 6 6 255 7
fi

