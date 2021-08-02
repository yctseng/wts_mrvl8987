
START=$1
END=$2
GATEWAY=$3
NETMASK=$4

if [ "$NETMASK" = "" -o "$GATEWAY" = "" -o "$END" = "" -o "$START" = "" ]; then
        echo "USAGE: $0 START END GATEWAY NETMASK"
        echo "e.g: $0 192.168.1.2 192.168.1.254 192.168.1.1 255.255.255.0"
	START="192.168.1.2"
	END="192.168.1.254"
	GATEWAY="192.168.1.1"
	NETMASK="255.255.255.0"
fi

mount -o rw,remount /
mkdir -p /var/lib/misc/
touch /var/lib/misc/udhcpd.leases

cat << EOF > /etc/udhcpd.conf
start           $START
end             $END
interface       uap0
opt     dns     $GATEWAY
option  subnet  $NETMASK
opt     router  $GATEWAY
option  domain  local
option  lease   864000
EOF

ifconfig uap0 $GATEWAY
killall -9 udhcpd
udhcpd -f -I 192.168.1.1 /etc/udhcpd.conf &
