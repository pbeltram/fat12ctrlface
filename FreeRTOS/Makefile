#
# $Id$
#

SRC_TOP=..
TARGET_NAME=libfreertos
#Define ARMCPU! ARMCPU = lpc2478

OBJECT_FILES += \
    $(SRC_TOP)/FreeRTOS/Source/tasks.o \
    $(SRC_TOP)/FreeRTOS/Source/queue.o \
    $(SRC_TOP)/FreeRTOS/Source/list.o \
    $(SRC_TOP)/FreeRTOS/Source/portable/MemMang/heap_2.o

ifeq ("$(ARMCPU)", "lpc2148")
    OBJECT_FILES += \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC2000/port.o \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC2000/portISR.o
endif

ifeq ("$(ARMCPU)", "lpc2106")
    OBJECT_FILES += \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC2000/port.o \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC2000/portISR.o
endif

ifeq ("$(ARMCPU)", "lpc2368")
    OBJECT_FILES += \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC23xx/port.o \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC23xx/portISR.o
endif

ifeq ("$(ARMCPU)", "lpc2478")
    OBJECT_FILES += \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC23xx/port.o \
        $(SRC_TOP)/FreeRTOS/Source/portable/GCC/ARM7_LPC23xx/portISR.o
endif

INCLUDE_DIRS+=$(SRC_TOP)
include $(SRC_TOP)/make_rules/MakeRules

TARGETS=$(ARCHIVE_LIB)

all: $(TARGETS)
