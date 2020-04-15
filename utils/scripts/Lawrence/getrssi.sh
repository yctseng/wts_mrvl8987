echo "Return the RSSI received by STA"
./iwconfig mlan0 grep "Signal"| awk {'print $4'} | cut -d =-f2 > /tmp/rssi.txt
echo "iwconfig mlan0 grep "Signal"  awk {'print $4'} | cut -d =-f2"
