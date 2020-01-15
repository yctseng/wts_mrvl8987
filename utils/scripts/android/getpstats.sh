ifile=$1
res=`busybox grep transmitted $ifile`
tx=`echo $res | busybox cut -f1 -d, | busybox cut -f1 -d' '`
echo $tx > /tmp/stpsta.txt
rx=`echo $res | busybox cut -f2 -d, | busybox cut -f2 -d' '`
echo $rx >> /tmp/stpsta.txt
