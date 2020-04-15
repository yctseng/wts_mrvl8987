include Makefile.inc
DIRS= ${LIB} ${DUT} ${CA} ${TC} platform_tools
all:
	for i in ${DIRS}; do \
		$(MAKE) -C $$i || exit 1; \
	done
	rm -rf MrvlWTS_$(TARGET)_bin
	mkdir MrvlWTS_$(TARGET)_bin
	cp dut/wfa_dut MrvlWTS_$(TARGET)_bin/wfa_dut
	cp ca/wfa_ca MrvlWTS_$(TARGET)_bin/wfa_ca
	cp -rf utils/scripts/$(TARGET)/* MrvlWTS_$(TARGET)_bin/
	cp -rf utils/scripts/common/* MrvlWTS_$(TARGET)_bin/
	cp -rf utils/config/* MrvlWTS_$(TARGET)_bin/
	cp platform_tools/hostap/hostapd/hostapd_cli MrvlWTS_$(TARGET)_bin/
	cp platform_tools/hostap/hostapd/hostapd MrvlWTS_$(TARGET)_bin/
	cp platform_tools/ping MrvlWTS_$(TARGET)_bin/
	cp platform_tools/dnsmasq MrvlWTS_$(TARGET)_bin/
clean:
	for i in ${DIRS}; do \
		$(MAKE) -C $$i clean || exit 1; \
	done
	rm -rf MrvlWTS_$(TARGET)_bin
	
