#!/bin/bash
DEST_MAC=$1

echo "enter startwfdconn.sh"
sleep 10

role=`wpa_cli -i wfd0 status | grep mode | cut -d= -f2`
if [ "$role" != "P2P GO" ]
then
    udhcpc -i wfd0 > /tmp/udhcpc.txt 2>&1 &
fi
sleep 10
echo "sleep done. start wfd connection"

port=$((16#`wpa_cli -i wfd0 p2p_peer $DEST_MAC | grep wfd_sub | cut -d= -f2 | cut -b 11-14`))
wpa_cli -i wfd0 p2p_peer $1 > /tmp/ken_p2p_peer.txt 2>&1
echo "port:$port"
interface_addr=`wpa_cli -i wfd0 p2p_peer $DEST_MAC | grep intended_addr | cut -d= -f2`
echo "interface_addr:$interface_addr"
ip=`./busybox.nosuid arp -a | grep $interface_addr | cut -d\( -f2 | cut -d\) -f1`
./busybox.nosuid arp -a > /tmp/arp_result.txt 2>&1

echo "ip:$ip"
dbus-send --system --print-reply --reply-timeout=2000 --type=method_call --dest=com.garmin.DBus.Miracast /com/garmin/DBus/Miracast/MiracastManager com.garmin.DBus.Miracast.MiracastManager.Setup string:{\"rtsp_connect\":\"$ip\;$port\"}


sleep 8
dbus-send --system --print-reply --reply-timeout=2000 --type=method_call --dest=com.garmin.DBus.Miracast /com/garmin/DBus/Miracast/MiracastManager com.garmin.DBus.Miracast.MiracastManager.Setup string:{\"dump\":0}

cat /wfd_session
# r_ssid=`wpa_cli -i wfd0 status | grep ^ssid | cut -d= -f2`
# echo $r_ssid
# 
# role=`wpa_cli -i wfd0 status | grep mode | cut -d= -f2`
# r_role="GC"
# if [ "$role" = "P2P GO" ]
# then
#     r_role="GO"
# fi
# 
# echo $r_role
