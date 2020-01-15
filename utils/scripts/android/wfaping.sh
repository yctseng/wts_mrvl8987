#echo $@
/system/bin/ping $@ &
echo PID=$! > /tmp/pingpid.txt 
