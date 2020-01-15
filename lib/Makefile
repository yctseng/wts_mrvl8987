include ../Makefile.inc

all:    ${LIB_OBJS}
		ar crv ${LIBWFA_NAME} $?
		ar crv ${LIBWFA_NAME_DUT} ${LIB_OBJS_DUT}
		ar crv ${LIBWFA_NAME_CA} ${LIB_OBJS_CA} 
		${RANLIB} ${LIBWFA_NAME} ${LIBWFA_NAME_DUT} ${LIBWFA_NAME_CA}

wfa_tg.o: wfa_tg.c ../inc/wfa_agt.h ../inc/wfa_types.h ../inc/wfa_rsp.h ../inc/wfa_tlv.h  ../inc/wfa_tg.h

wfa_cs.o: wfa_cs.c ../inc/wfa_agt.h ../inc/wfa_types.h ../inc/wfa_rsp.h ../inc/wfa_tlv.h ../inc/mrvl_common.h ../inc/mrvl_gvars.h ../inc/mrvl_macros.h ../inc/mrvl_supplicant.h ../inc/mrvl_mwud.h ../inc/mrvl_miscs.h ../inc/mrvl_debug.h 

wfa_ca_resp.o: wfa_ca_resp.c ../inc/wfa_agtctrl.h ../inc/wfa_types.h ../inc/wfa_rsp.h ../inc/wfa_tlv.h ../inc/wfa_types.h

wfa_cmdproc.o: wfa_cmdproc.c ../inc/wfa_agtctrl.h ../inc/wfa_types.h ../inc/wfa_tg.h ../inc/wfa_tlv.h ../inc/mrvl_macros.h

wfa_cmdtbl.o: wfa_cmdtbl.c ../inc/wfa_types.h ../inc/wfa_tlv.h

wfa_sock.o: wfa_sock.c ../inc/wfa_sock.h ../inc/wfa_types.h

wfa_thr.o: wfa_thr.c ../inc/wfa_tg.h 

wfa_wmmps.o: wfa_wmmps.c ../inc/wfa_wmmps.h

mrvl_common.o: mrvl_common.c ../inc/mrvl_common.h ../inc/mrvl_gvars.h ../inc/mrvl_macros.h 

mrvl_supplicant.o: mrvl_supplicant.c ../inc/mrvl_supplicant.h ../inc/mrvl_common.h ../inc/mrvl_gvars.h ../inc/mrvl_macros.h 

mrvl_hostapd.o: mrvl_hostapd.c ../inc/mrvl_hostapd.h ../inc/mrvl_common.h ../inc/mrvl_gvars.h ../inc/mrvl_macros.h 

mrvl_mwud.o: mrvl_mwud.c ../inc/mrvl_mwud.h ../inc/mrvl_common.h ../inc/mrvl_gvars.h ../inc/mrvl_macros.h 

mrvl_miscs.o: mrvl_miscs.c ../inc/mrvl_miscs.h ../inc/mrvl_common.h ../inc/mrvl_gvars.h ../inc/mrvl_macros.h 

mrvl_debug.o: mrvl_debug.c ../inc/mrvl_debug.h ../inc/wfa_debug.h

mrvl_gvars.o: mrvl_gvars.c ../inc/mrvl_gvars.h 

clean:
		rm -f ${PROGS} ${CLEANFILES}

