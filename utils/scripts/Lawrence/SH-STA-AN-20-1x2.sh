# 20MHz, SGI, Rx LDPC, Rx STBC Disabled, GF Disabled, Tx STBC Disabled, 1x2
./mlanutl mlan0 deauth
./mlanutl mlan0 antcfg 0x101 0x303
./mlanutl mlan0 htstreamcfg 0x22
./mlanutl mlan0 bandcfg 20
./mlanutl mlan0 httxcfg 0x02D  
./mlanutl mlan0 htcapinfo 0x0C00000
./mlanutl mlan0 httxbfcap 0x19E74608
./mlanutl mlan0 vhtcfg 0 1 0 0x33D07030 0xffff 0xffff
./mlanutl mlan0 vhtcfg 0 2 0 0x33D07030 0xffff 0xffff