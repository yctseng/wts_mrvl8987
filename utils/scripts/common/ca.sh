# ====================================================
#  File: ca.sh
#  Copyright (C) 2014, Marvell International Ltd.
#  Support @ Abhishek Prasad ( aprasad@marvell.com )
# =========================================================   
pid="$(ps -ae | grep wfa_ca  | awk '{ print $1 }')"
if [ -n "$pid" ]; then kill $pid; fi

export WFA_ENV_AGENT_PORT=8000
export WFA_ENV_AGENT_IPADDR=127.0.0.1
./wfa_ca $1 9000 











	












