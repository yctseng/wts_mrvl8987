#CC = gcc
###FOR PXA, We use a perl wrapper over gcc (agcc). THis is necessary since Android uses bionic not libc
#CC = agcc 

TARGET=Lawrence
LIB=lib
DUT=dut
CA=ca
UCC=ucc
MAKE=make

CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_VOICE_EXT -DWFA_STA_TB -DEXPERIMENTAL_AP_SUPPORT -Wall -I../inc
#CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_WMM_AC -DWFA_VOICE_EXT -DWFA_STA_TB -DEXPERIMENTAL_AP_SUPPORT -Wall -I../inc
#CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_WMM_AC -DWFA_VOICE_EXT -DWFA_STA_TB -Wall -I../inc

## uncoment for _PXA9xx_with MWU  ( non-Test-Bed WMM-AC, WMM-PS and Voice )
#CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_WMM_AC -DWFA_VOICE_EXT -DWFA_STA_TB -DMV_PLATFORM_ANDROID -DMV_PLATFORM_PXA_MWU -Wall -I../inc

## for PXA9xx_ ( non-Test-Bed WMM-AC, WMM-PS and Voice )
#CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_VOICE_EXT -DWFA_STA_TB -DMV_PLATFORM_ANDROID -DEXPERIMENTAL_AP_SUPPORT -Wall -I../inc
#CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_WMM_AC -DWFA_VOICE_EXT -DWFA_STA_TB -DMV_PLATFORM_ANDROID -DEXPERIMENTAL_AP_SUPPORT -Wall -I../inc
#CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_WMM_AC -DWFA_VOICE_EXT -DWFA_STA_TB -DMV_PLATFORM_ANDROID -Wall -I../inc

## uncoment for _PLUG_ ( non-Test-Bed WMM-AC, WMM-PS and Voice )
#CFLAGS = -g -O2 -D_REENTRANT  -DWFA_WMM_PS_EXT -DWFA_WMM_AC -DWFA_VOICE_EXT -DMV_PLATFORM_PLUG -Wall -I../inc

## for SYSKT and PLUG
DUTLIBS = ../lib/libwfa_dut.a -lpthread
CALIBS = ../lib/libwfa_ca.a -lpthread

## uncoment for PXA, we need not to use -lpthread since android bionic supports custom pthreads natively
#DUTLIBS = ../lib/libwfa_dut.a
#CALIBS = ../lib/libwfa_ca.a

RANLIB = ranlib

LIBWFA_NAME_DUT = libwfa_dut.a
LIBWFA_NAME_CA = libwfa_ca.a
LIBWFA_NAME = libwfa.a

LIB_OBJS = wfa_sock.o wfa_tg.o wfa_cs.o wfa_ca_resp.o wfa_tlv.o wfa_typestr.o wfa_cmdtbl.o wfa_cmdproc.o wfa_miscs.o wfa_thr.o wfa_wmmps.o mrvl_common.o mrvl_supplicant.o mrvl_hostapd.o mrvl_mwud.o mrvl_miscs.o mrvl_debug.o mrvl_gvars.o 

LIB_OBJS_DUT = wfa_sock.o wfa_tlv.o wfa_cs.o wfa_cmdtbl.o wfa_tg.o wfa_miscs.o wfa_thr.o wfa_wmmps.o mrvl_common.o mrvl_supplicant.o mrvl_hostapd.o mrvl_mwud.o mrvl_miscs.o mrvl_debug.o mrvl_gvars.o

LIB_OBJS_CA = wfa_sock.o wfa_tlv.o wfa_ca_resp.o wfa_cmdproc.o wfa_miscs.o wfa_typestr.o

CLEANFILES = core core.* *.core.* *.o *.a
