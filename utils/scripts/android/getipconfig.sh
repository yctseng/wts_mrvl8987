echo -n "mac=" > $1
busybox ifconfig $2 | busybox grep HWaddr | busybox cut -f3 -dr >> $1
echo -n "ipaddr=" >> $1
busybox ifconfig $2 | busybox grep "inet addr" | busybox cut -f2 -d: >> $1
echo -n "bcast=" >> $1
busybox ifconfig $2 | busybox grep "inet addr" | busybox cut -f3 -d: >> $1
echo -n "mask=" >> $1
busybox ifconfig $2 | busybox grep "inet addr" | busybox cut -f4 -d: >> $1
