# $1 is tmp file name
# $2 is the interface name

echo -n "ssid=" > $1
iwconfig $2 | grep "ESSID:" | cut -f2 -d\" >> $1
echo -n "bssid=" >> $1
iwconfig $2 | grep "Access Point:" | cut -f2 -dt >> $1

