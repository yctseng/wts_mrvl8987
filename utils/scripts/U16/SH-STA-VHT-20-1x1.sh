# 20MHz, SGI, Rx LDPC, Rx STBC Disabled, GF Disabled, Tx STBC Disabled,MU Beamformee, 1SS
./mlanutl mlan0 deauth
./mlanutl mlan0 antcfg 0x101 0x101
./mlanutl mlan0 htstreamcfg 0x11
./mlanutl mlan0 bandcfg 127
./mlanutl mlan0 httxcfg 0x02D  
./mlanutl mlan0 htcapinfo 0x00C00000
./mlanutl mlan0 httxbfcap 0x19E74608
./mlanutl mlan0 vhtcfg 2 1 0 0x33D07030 0xfffd 0xfffd
./mlanutl mlan0 vhtcfg 2 2 0 0x33D07030 0xfffd 0xfffd