# 20/40MHz, SGI, Rx LDPC, Rx STBC Disabled, GF, Tx STBC Disabled, MU Beamformee,2SS
./mlanutl $1 deauth
./mlanutl $1 antcfg 0x303 0x303
./mlanutl $1 htstreamcfg 0x22
./mlanutl $1 bandcfg 0x37F
./mlanutl $1 httxcfg 0x07F
./mlanutl $1 htcapinfo 0x21C20000
./mlanutl $1 httxbfcap 0x19E74618
./mlanutl $1 vhtcfg 2 1 0 0x33D17030 0xfffa 0xfffa
./mlanutl $1 vhtcfg 2 2 0 0x33D17030 0xfffa 0xfffa
./mlanutl $1 11axcfg config/11axcfg.conf
