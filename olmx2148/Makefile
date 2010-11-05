#
# $Id$
#

SRC_TOP=..
TARGET_NAME = olmx2148
ARMCPU = lpc2148

OBJECT_FILES = startup.o main.o hwinit.o

TARGETS = $(TARGET_NAME) $(TARGET_NAME).bin

INCLUDE_DIRS+=$(SRC_TOP)
LDLIBS+=\
    $(SRC_TOP)/usbmass/libusbmass_$(ARMCPU).a \
    $(SRC_TOP)/usb/libusb_$(ARMCPU).a \
    $(SRC_TOP)/shared/libshared_$(ARMCPU).a \
    $(SRC_TOP)/virtdisk/libvirtdisk_$(ARMCPU).a \
    $(SRC_TOP)/dcc/libdcc_$(ARMCPU).a \
    $(SRC_TOP)/FreeRTOS/libfreertos_$(ARMCPU).a

include $(SRC_TOP)/make_rules/MakeRules

all: $(TARGETS)
