file=$1
pid_no=`busybox cat "$file" | busybox grep PID | busybox cut -f2 -d'='`
#echo "$file" | grep PID | cut -f2 -d'='
echo PID=$pid_no >> $2
