#
# $Id$
#

SRC_TOP=..
TARGET_NAME=libusb
#Define ARMCPU! ARMCPU = lpc2478

OBJECT_FILES=usbISR.o usbcontrol.o usbinit.o usbstdreq.o

INCLUDE_DIRS+=$(SRC_TOP)
include $(SRC_TOP)/make_rules/MakeRules

TARGETS=$(ARCHIVE_LIB)

all: $(TARGETS)
