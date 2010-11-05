#
# $Id$
#

SRC_TOP=.

.PHONY: all clean
.DEFAULT_GOAL := all

all:
	@echo "==> Building all"
	@$(MAKE) $@ -C FreeRTOS RELEASE_BUILD=1 ARMCPU=lpc2148
	@$(MAKE) $@ -C dcc RELEASE_BUILD=1 ARMCPU=lpc2148
	@$(MAKE) $@ -C shared RELEASE_BUILD=1 ARMCPU=lpc2148
	@$(MAKE) $@ -C usb RELEASE_BUILD=1 ARMCPU=lpc2148
	@$(MAKE) $@ -C usbmass RELEASE_BUILD=1 ARMCPU=lpc2148
	@$(MAKE) $@ -C virtdisk RELEASE_BUILD=1 ARMCPU=lpc2148
	@$(MAKE) $@ -C olmx2148 RELEASE_BUILD=1 ARMCPU=lpc2148

clean:
	@echo "==> Cleaning all"
	@$(MAKE) $@ -C FreeRTOS ARMCPU=lpc2148
	@$(MAKE) $@ -C dcc ARMCPU=lpc2148
	@$(MAKE) $@ -C shared ARMCPU=lpc2148
	@$(MAKE) $@ -C usb ARMCPU=lpc2148
	@$(MAKE) $@ -C usbmass ARMCPU=lpc2148
	@$(MAKE) $@ -C virtdisk ARMCPU=lpc2148
	@$(MAKE) $@ -C olmx2148 ARMCPU=lpc2148

