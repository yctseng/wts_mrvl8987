# ====================================================
#  File: dut.sh	 
#  Copyright (C) 2014, Marvell International Ltd.
#  Support @ Abhishek Prasad ( aprasad@marvell.com )
# =========================================================
pid="$(ps -ae | grep wfa_dut | awk '{ print $1 }')"
if [ -n "$pid" ]; then kill $pid; fi

export LD_LIBRARY_PATH=`pwd`
export PATH=${PATH}:`pwd`
./wfa_dut lo 8000  











	












