# 20/40MHz, SGI, Rx LDPC, Rx STBC Disabled, GF, Tx STBC Disabled, 2SS
./mlanutl mlan0 deauth
./mlanutl mlan0 antcfg 0x303 0x303
./mlanutl mlan0 htstreamcfg 0x22
./mlanutl mlan0 bandcfg 20
./mlanutl mlan0 httxcfg 0x07F 
./mlanutl mlan0 htcapinfo 0x21C20000
./mlanutl mlan0 httxbfcap 0x19E74618
./mlanutl mlan0 vhtcfg 0 1 0 0x33D17030 0xffff 0xffff
./mlanutl mlan0 vhtcfg 0 2 0 0x33D17030 0xffff 0xffff