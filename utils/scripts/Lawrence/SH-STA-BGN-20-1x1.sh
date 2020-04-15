# 20MHz, SGI, Rx LDPC, Rx STBC Disabled, GF Disabled, Tx STBC Disabled, 1SS
./mlanutl mmlan0 deauth
./mlanutl mmlan0 antcfg 0x101 0x101
./mlanutl mmlan0 htstreamcfg 0x11
./mlanutl mmlan0 bandcfg 11
./mlanutl mmlan0 httxcfg 0x02D  
./mlanutl mmlan0 htcapinfo 0x00C00000
./mlanutl mmlan0 httxbfcap 0x19E74608
./mlanutl mmlan0 vhtcfg 0 1 0 0x33D07030 0xffff 0xffff
./mlanutl mmlan0 vhtcfg 0 2 0 0x33D07030 0xffff 0xffff