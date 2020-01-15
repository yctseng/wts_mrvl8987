#file=$1
#pid_no=`busybox cat "$file" | busybox grep PID | busybox cut -f2 -d'='`
busybox killall -2 ping 2>/dev/null
