# 20MHz, SGI, Rx LDPC, Rx STBC, GF, Tx STBC, 2SS
./mlanutl mmlan0 deauth
./mlanutl mmlan0 antcfg 0x303 0x303
./mlanutl mmlan0 htstreamcfg 0x22
./mlanutl mmlan0 bandcfg 11
./mlanutl mmlan0 httxcfg 0x02D
./mlanutl mmlan0 htcapinfo 0x20C00000
./mlanutl mmlan0 httxbfcap 0x19E74618
./mlanutl mmlan0 vhtcfg 0 1 0 0x33D17030 0xffff 0xffff
./mlanutl mmlan0 vhtcfg 0 2 0 0x33D17030 0xffff 0xffff