# $1 is tmp file name
# $2 is the interface name

echo -n "ssid=" > $1
iwconfig $2 | busybox grep "ESSID:" | busybox cut -f2 -d\" >> $1
echo -n "bssid=" >> $1
iwconfig $2 | busybox grep "Access Point:" | busybox cut -f2 -dt >> $1

