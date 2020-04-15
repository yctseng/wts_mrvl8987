include Makefile.inc
DIRS= ${LIB} ${DUT} ${CA} ${TC} platform_tools
all:
	for i in ${DIRS}; do \
		$(MAKE) -C $$i || exit 1; \
	done
	rm -rf MrvlSigma_$(TARGET)_bin
	mkdir MrvlSigma_$(TARGET)_bin
	cp dut/wfa_dut MrvlSigma_$(TARGET)_bin/wfa_dut
	cp ca/wfa_ca MrvlSigma_$(TARGET)_bin/wfa_ca
	cp -rf utils/scripts/$(TARGET)/* MrvlSigma_$(TARGET)_bin/
	cp -rf utils/scripts/common/* MrvlSigma_$(TARGET)_bin/
	cp -rf utils/config/* MrvlSigma_$(TARGET)_bin/
	cp platform_tools/hostap/hostapd/hostapd_cli MrvlSigma_$(TARGET)_bin/
	cp platform_tools/hostap/hostapd/hostapd MrvlSigma_$(TARGET)_bin/
clean:
	for i in ${DIRS}; do \
		$(MAKE) -C $$i clean || exit 1; \
	done
	rm -rf MrvlSigma_$(TARGET)_bin
	
