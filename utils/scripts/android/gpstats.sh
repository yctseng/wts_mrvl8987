file=$1
tx=`busybox cat "$file" | busybox grep transmitted | busybox cut -f1 -d' '`
rx=`busybox cat "$file" | busybox grep transmitted | busybox cut -f2 -d, | busybox cut -f2 -d' '`

#echo "$file" | grep transmitted | cut -f1 -d' '
#echo "$file" | grep transmitted | cut -f2 -d, | cut -f2 -d' '
echo $tx > /tmp/stpsta.txt
echo $rx >> /tmp/stpsta.txt

