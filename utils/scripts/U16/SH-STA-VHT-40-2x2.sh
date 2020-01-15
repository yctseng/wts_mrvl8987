# 20/40MHz, SGI, Rx LDPC, Rx STBC Disabled, GF, Tx STBC Disabled, MU Beamformee,2SS
./mlanutl mlan0 deauth
./mlanutl mlan0 antcfg 0x303 0x303
./mlanutl mlan0 htstreamcfg 0x22
./mlanutl mlan0 bandcfg 127
./mlanutl mlan0 httxcfg 0x07F
./mlanutl mlan0 htcapinfo 0x21C20000
./mlanutl mlan0 httxbfcap 0x19E74618
./mlanutl mlan0 vhtcfg 2 1 0 0x33D17030 0xfffa 0xfffa
./mlanutl mlan0 vhtcfg 2 2 0 0x33D17030 0xfffa 0xfffa