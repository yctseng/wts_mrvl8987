# 20MHz, SGI, Rx LDPC, Rx STBC Disabled, GF Disabled, Tx STBC Disabled,MU Beamformee, 1x2
./mlanutl mmlan0 deauth
./mlanutl mmlan0 antcfg 0x101 0x303
./mlanutl mmlan0 htstreamcfg 0x11
./mlanutl mmlan0 bandcfg 127
./mlanutl mmlan0 httxcfg 0x02D
./mlanutl mmlan0 htcapinfo 0x00C00000
./mlanutl mmlan0 httxbfcap 0x19E74608
./mlanutl mmlan0 vhtcfg 1 1 0 0x33D07030 0xfffd 0xfff5
./mlanutl mmlan0 vhtcfg 1 2 0 0x33D07030 0xfffd 0xfff5