# 20/40MHz, SGI, Rx LDPC, Rx STBC Disabled, GF Disabled, Tx STBC Disabled,MU Beamformee,1SS
./mlanutl mlan0 deauth
./mlanutl mlan0 antcfg 0x101 0x101
./mlanutl mlan0 htstreamcfg 0x11
./mlanutl mlan0 bandcfg 0x37F
./mlanutl mlan0 httxcfg 0x06F  
./mlanutl mlan0 htcapinfo 0x01C20000
./mlanutl mlan0 httxbfcap 0x19E74608
./mlanutl mlan0 vhtcfg 2 1 0 0x33D07030 0xfffe 0xfffe
./mlanutl mlan0 vhtcfg 2 2 0 0x33D07030 0xfffe 0xfffe
./mlanutl mlan0 11axcfg config/11axcfg_40-1x1.conf