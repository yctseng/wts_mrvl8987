# 20/40MHz, SGI, Rx LDPC, Rx STBC Disabled, GF, Tx STBC Disabled, 2SS
./mlanutl mmlan0 deauth
./mlanutl mmlan0 antcfg 0x303 0x303
./mlanutl mmlan0 htstreamcfg 0x22
./mlanutl mmlan0 bandcfg 11
./mlanutl mmlan0 httxcfg 0x07F
./mlanutl mmlan0 htcapinfo 0x21C20000
./mlanutl mmlan0 httxbfcap 0x19E74618
./mlanutl mmlan0 vhtcfg 0 1 0 0x33D17030 0xffff 0xffff
./mlanutl mmlan0 vhtcfg 0 2 0 0x33D17030 0xffff 0xffff