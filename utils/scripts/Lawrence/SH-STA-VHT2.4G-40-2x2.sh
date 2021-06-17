# 20/40MHz, SGI, Rx LDPC, Rx STBC Disabled, GF, Tx STBC Disabled, MU Beamformee,2SS
./mlanutl mmlan0 deauth
./mlanutl mmlan0 antcfg 0x303 0x303
./mlanutl mmlan0 htstreamcfg 0x22
./mlanutl mmlan0 bandcfg 127
./mlanutl mmlan0 httxcfg 0x06F
./mlanutl mmlan0 htcapinfo 0x01C20000
./mlanutl mmlan0 httxbfcap 0x19E74618
./mlanutl mmlan0 vhtcfg 1 1 0 0x33D07030 0xfffa 0xfffa
./mlanutl mmlan0 vhtcfg 1 2 0 0x33D07030 0xfffa 0xfffa