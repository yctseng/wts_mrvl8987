# 20MHz, SGI, Rx LDPC, Rx STBC Disabled, GF, Tx STBC Disabled, MU Beamformee,2SS
./mlanutl $1 deauth
./mlanutl $1 antcfg 0x303 0x303
./mlanutl $1 htstreamcfg 0x22
./mlanutl $1 bandcfg 0x37F
./mlanutl $1 httxcfg 0x02D 
./mlanutl $1 htcapinfo 0x20C00000
./mlanutl $1 httxbfcap 0x19E74618
./mlanutl $1 vhtcfg 0 1 0 0x33D07030 0xffff 0xffff
./mlanutl $1 vhtcfg 0 2 0 0x33D07030 0xffff 0xffff
./mlanutl $1 11axcfg config/11axcfg_2.4G-20-2x2.conf