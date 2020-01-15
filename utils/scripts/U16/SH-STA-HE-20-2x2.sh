# 20MHz, SGI, Rx LDPC, Rx STBC, GF, Tx STBC,MU Beamformee, 2SS
./mlanutl mlan0 deauth
./mlanutl mlan0 antcfg 0x303 0x303
./mlanutl mlan0 htstreamcfg 0x22
./mlanutl mlan0 bandcfg 0x37F
./mlanutl mlan0 httxcfg 0x02D 
./mlanutl mlan0 htcapinfo 0x20C00000
./mlanutl mlan0 httxbfcap 0x19E74618
./mlanutl mlan0 vhtcfg 2 1 0 0x33D07030 0xfff5 0xfff5
./mlanutl mlan0 vhtcfg 2 2 0 0x33D07030 0xfff5 0xfff5
./mlanutl mlan0 11axcfg config/11axcfg_20-2x2.conf