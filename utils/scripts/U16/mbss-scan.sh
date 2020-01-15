rm -rf scan.txt scan-list.txt MBSS_SCAN.txt MBSS_SCAN_RES.txt MBSS_SCAN_RES-1.txt
iwlist $1 scan | grep 'Address:\|ESSID:'  > scan.txt;
cp scan.txt scan-Org.txt;
find ./scan.txt -type f  -exec sed -i 's/Address:/BSSID,/g' {} \;
find ./scan.txt -type f  -exec sed -i 's/ESSID:/SSID,/g' {} \;
sleep 2
awk 'ORS=NR%2?FS:RS' scan.txt > scan-list.txt;
sleep 2
awk '{print$6,",",$4$5}' scan-list.txt | tr '\n' ',' > MBSS_SCAN.txt;
cut -c 1-470 <MBSS_SCAN.txt > MBSS_SCAN_RES.txt;
cat MBSS_SCAN_RES.txt | tr -d \" > MBSS_SCAN_RES-1.txt
cp MBSS_SCAN_RES-1.txt MBSS_SCAN_RES.txt
cat MBSS_SCAN_RES-1.txt |tr -d " " > MBSS_SCAN_RES.txt
cp MBSS_SCAN_RES.txt /tmp/. ;

