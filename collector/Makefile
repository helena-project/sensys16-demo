# makefile for user application

CURRENT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

TOCK_USERLAND_BASE_DIR = $(abspath $(CURRENT_DIR)/../tock/userland)
TOCK_BASE_DIR = $(abspath $(CURRENT_DIR)/../tock)
BUILDDIR ?= $(abspath build/$(TOCK_ARCH))

DEVICE := NRF51
SOFTDEVICE_MODEL := s130

SERIALIZATION_MODE = application
NRF_BASE_PATH ?= $(abspath $(CURRENT_DIR)/../nrf5x-base)
NRF_MODEL ?= nrf51
BOARD ?= BOARD_NULL

USE_BLE := 1
SOFTDEVICE_VERSION ?= 8.0.0
NRF_MODEL := nrf51

BLEADDR_FLASH_LOCATION ?= 0x0007FFF8
CFLAGS += -DBLEADDR_FLASH_LOCATION=$(BLEADDR_FLASH_LOCATION)

# Add useful paths from nRF5x-base
LIBRARY_PATHS += $(NRF_BASE_PATH)/advertisement/
LIBRARY_PATHS += $(NRF_BASE_PATH)/devices/
LIBRARY_PATHS += $(NRF_BASE_PATH)/lib/
LIBRARY_PATHS += $(NRF_BASE_PATH)/peripherals/
LIBRARY_PATHS += $(NRF_BASE_PATH)/services/

SOURCE_PATHS += $(NRF_BASE_PATH)/advertisement/
SOURCE_PATHS += $(NRF_BASE_PATH)/devices/
SOURCE_PATHS += $(NRF_BASE_PATH)/lib/
SOURCE_PATHS += $(NRF_BASE_PATH)/peripherals/
SOURCE_PATHS += $(NRF_BASE_PATH)/services/
SOURCE_PATHS += $(NRF_BASE_PATH)/startup

SDK_PATH ?= $(NRF_BASE_PATH)/sdk/nrf51_sdk_11.0.0/

# Other knowns about the SDK paths
SDK_INCLUDE_PATH   = $(SDK_PATH)components/
SDK_SOURCE_PATH    = $(SDK_PATH)components/

# Need to add the paths for all the directories in the SDK.
# Note that we do not use * because some folders have conflicting files.
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)libraries/*/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/ble_flash/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/clock/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/common/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/delay/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/gpiote/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/hal/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/lpcomp/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/ppi/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/pstorage/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/pstorage/config/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/radio_config/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/rng/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/rtc/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/sdio/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/spi_master/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/spi_slave/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/swi/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/timer/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/twi_master/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/uart/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/wdt/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/validation/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_ext/*/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)device/)
LIBRARY_PATHS += $(SDK_INCLUDE_PATH)toolchain/gcc/
LIBRARY_PATHS += $(SDK_INCLUDE_PATH)toolchain/

SOURCE_PATHS += $(SDK_SOURCE_PATH)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)*/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)libraries/*/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)drivers_nrf/*/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)drivers_ext/*/)

LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/*/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/common/transport/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/$(SERIALIZATION_MODE)/codecs/common/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/$(SERIALIZATION_MODE)/hal/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/$(SERIALIZATION_MODE)/transport/)
# LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/common/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/common/transport/ser_phy/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/common/transport/ser_phy/config/)

SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/*/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/common/transport/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/application/codecs/common/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/application/hal/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/application/transport/)
# SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/common/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/common/transport/ser_phy/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/common/transport/ser_phy/config/)

CFLAGS += -DSVCALL_AS_NORMAL_FUNCTION -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DCENTRAL_LINK_COUNT=0 -DPERIPHERAL_LINK_COUNT=1

LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)ble/*/)
LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)ble/ble_services/*/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)ble/*/)
SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)ble/ble_services/*/)
CFLAGS += -DBLE_STACK_SUPPORT_REQD

LIBRARY_PATHS += $(SDK_INCLUDE_PATH)softdevice/common/softdevice_handler/
LIBRARY_PATHS += $(SDK_INCLUDE_PATH)softdevice/$(SOFTDEVICE_MODEL)/headers/

SOURCE_PATHS += $(SDK_SOURCE_PATH)softdevice/common/softdevice_handler/

LIBRARY_PATHS += $(SDK_INCLUDE_PATH)serialization/application/codecs/$(SOFTDEVICE_MODEL)/middleware/
LIBRARY_PATHS += $(SDK_INCLUDE_PATH)serialization/application/codecs/$(SOFTDEVICE_MODEL)/serializers/
LIBRARY_PATHS += $(SDK_INCLUDE_PATH)serialization/common/struct_ser/$(SOFTDEVICE_MODEL)/

SOURCE_PATHS += $(SDK_SOURCE_PATH)serialization/application/codecs/$(SOFTDEVICE_MODEL)/middleware/
SOURCE_PATHS += $(SDK_SOURCE_PATH)serialization/application/codecs/$(SOFTDEVICE_MODEL)/serializers/
SOURCE_PATHS += $(SDK_SOURCE_PATH)serialization/common/struct_ser/$(SOFTDEVICE_MODEL)/
SOURCE_PATHS += $(SDK_SOURCE_PATH)softdevice/$(SOFTDEVICE_MODEL)/headers/

LIBRARY_INCLUDES = $(addprefix -I,$(LIBRARY_PATHS))
CMSIS_INCLUDE_PATH = $(SDK_PATH)components/toolchain/CMSIS/Include/
LIBRARY_INCLUDES += $(addprefix -I,$(CMSIS_INCLUDE_PATH))

CFLAGS += -g -D$(DEVICE) -D$(BOARD) $(LIBRARY_INCLUDES) -std=c11 -Os -DSDK_VERSION_$(SDK_VERSION) -DSOFTDEVICE_$(SOFTDEVICE_MODEL)
CFLAGS += -D$(shell echo $(SOFTDEVICE_MODEL) | tr a-z A-Z)
CFLAGS += -D__TOCK__
COMPILE_ONLY += -c

VPATH = $(SOURCE_PATHS)

APPLICATION_SRCS += simple_ble.c
APPLICATION_SRCS += ser_hal_transport.c
APPLICATION_SRCS += ser_sd_transport.c
APPLICATION_SRCS += ser_softdevice_handler.c

APPLICATION_SRCS += app_error.c
APPLICATION_SRCS += app_mw_ble_gap.c
APPLICATION_SRCS += ble_enable.c
APPLICATION_SRCS += ble_gap_scan_start.c
APPLICATION_SRCS += ble_gap_scan_stop.c
APPLICATION_SRCS += ble_gap_struct_serialization.c
APPLICATION_SRCS += ble_serialization.c
APPLICATION_SRCS += cond_field_serialization.c

SRCS = $(notdir $(APPLICATION_SRCS))
OBJS = $(addprefix $(BUILDDIR)/, $(SRCS:.c=.o)) $(addprefix $(BUILDDIR)/,$(APPLICATION_LIBS))

C_SRCS := $(wildcard *.c)
OBJS += $(patsubst %.c,$(BUILDDIR)/%.o,$(C_SRCS))

CPPFLAGS += -DSTACK_SIZE=2048

# include userland master makefile. Contains rules and flags for actually
# 	building the application
include $(TOCK_USERLAND_BASE_DIR)/Makefile

.PHONY:
clean::
	@echo $(OBJS)
	rm -Rf $(BUILDDIR)

