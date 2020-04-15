#!/bin/bash
DEST_MAC=$1
PIN=$2
echo "enter startwfdconn_accept.sh"

echo "sleep 20"
sleep 20
wpa_cli -i wfd0 p2p_connect $DEST_MAC $PIN auth

echo "sleep 40"
sleep 40
echo "sleep done. start wfd connection"

port=$((16#`wpa_cli -i wfd0 p2p_peer $DEST_MAC | grep wfd_sub | cut -d= -f2 | cut -b 11-14`))
wpa_cli -i wfd0 p2p_peer $1 > /tmp/ken_p2p_peer.txt 2>&1
echo "port:$port"
interface_addr=`wpa_cli -i wfd0 p2p_peer $DEST_MAC | grep intended_addr | cut -d= -f2`
echo "interface_addr:$interface_addr"
ip=`./busybox.nosuid arp -a | grep $interface_addr | cut -d\( -f2 | cut -d\) -f1`
./busybox.nosuid arp -a > /tmp/arp_result.txt 2>&1

if [ "x$ip" = "x" ]; then
    echo "Can't find IP in arp table"
    # workaround for 6.1.3
    ip="192.168.16.2"
fi

echo "ip:$ip"
dbus-send --system --print-reply --reply-timeout=2000 --type=method_call --dest=com.garmin.DBus.Miracast /com/garmin/DBus/Miracast/MiracastManager com.garmin.DBus.Miracast.MiracastManager.Setup string:{\"rtsp_connect\":\"$ip\;$port\"}

