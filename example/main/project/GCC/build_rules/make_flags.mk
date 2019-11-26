
CORE=4
CPUFLAGS :=  -mcpu=cortex-m4 -mthumb -g2
#CPUFLAGS += -w
CPUFLAGS += -Wall -Wno-unused-function
CPUFLAGS += -O2
CPUFLAGS += -Wno-pointer-sign
CPUFLAGS += -fno-common
CPUFLAGS += -fmessage-length=0
CPUFLAGS += -ffunction-sections
CPUFLAGS += -fdata-sections
CPUFLAGS += -fomit-frame-pointer
CPUFLAGS += -fno-short-enums
CPUFLAGS += -DF_CPU=166000000L
CPUFLAGS += -std=gnu99
CPUFLAGS += -fsigned-char
#-mthumb -mcpu=cortex-m$(CORE) -nostartfiles -Wl,--no-enum-size-warning -Wl,--no-wchar-size-warning -Wl,--gc-sections -Wl,--cref
#-mfloat-abi=soft  -mthumb-interwork -specs=nosys.specs -specs=nano.specs -specs=rdimon.specs -g -fno-common -fmessage-length=0

FLAGS    := 
#-g -Wall

#header files links
ifeq ($(CONFIG_CUSTOME_ZG), 1)
CUSTOM_ZG_INC = -I $(CODE_DIR)/customer/                            \
            -I $(CODE_DIR)/customer/light                           \
            -I $(CODE_DIR)/customer/net                             \
            -I $(CODE_DIR)/customer/port                            \
            -I $(CODE_DIR)/customer/protocol_handle                 \
            -I $(CODE_DIR)/customer/socket                          \
            -I $(CODE_DIR)/customer/sys
endif

CFLAGS    = -I $(CODE_DIR)/system                                                   \
           -I $(CODE_DIR)/utils                                                     \
           -I $(CODE_DIR)/cmsis                                                     \
           -I $(CODE_DIR)/cmsis/inc                                                 \
           -I $(CODE_DIR)/core/os                                                   \
           -I $(CODE_DIR)/core/os/freertos_v10.0.1/Source/include                   \
           -I $(CODE_DIR)/core/os/freertos_v10.0.1/Source/portable/GCC/ARM_CM4F     \
           -I $(CODE_DIR)/core/wlan                                                 \
           -I $(CODE_DIR)/core/hal                                                  \
           -I $(CODE_DIR)/atcmd/at_esp                                              \
           -I $(CODE_DIR)/atcmd/at_test                                             \
           -I $(CODE_DIR)/atcmd                                                     \
           -I $(CODE_DIR)/net                                                       \
           -I $(CODE_DIR)/net/ping                                                  \
           -I $(CODE_DIR)/net/dhcpc                                                 \
           -I $(CODE_DIR)/net/dhcps                                                 \
           -I $(CODE_DIR)/net/httpc                                                 \
           -I $(CODE_DIR)/net/mqtt/MQTTPacket/src/                                  \
           -I $(CODE_DIR)/net/mqtt/MQTTPacket/samples/                              \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include                          \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include/netif                    \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include/posix                    \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include/posix/sys/               \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include/ipv4/                    \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include/ipv6/                    \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include/lwip/                    \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/include/lwip/apps                \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/src/apps/sntp                        \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/port/sci                             \
           -I $(CODE_DIR)/net/lwip/lwip_v1.4.1/port/sci/freertos                    \
           -I $(CODE_DIR)/net/mbedtls/include                                       \
           -I $(CODE_DIR)/net/mbedtls/include/mbedtls                               \
           -I $(CODE_DIR)/fs/kv                                                     \
           -I $(CODE_DIR)/fs/kv/include

ifeq ($(CONFIG_CUSTOME_ZG), 1)
CFLAGS    += $(CUSTOM_ZG_INC)
endif

export CC COPY DUMP AR MK MV SED CP ROOT_DIR CFLAGS OBJS_DIR CPUFLAGS FLAGS MAKE LD
export RULES_DIR CUSTOM_ZG_INC


