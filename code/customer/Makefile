
include $(RULES_DIR)/.config

ZG_SDK_SOURCES = ./light/auto_report.c      \
                ./light/clock.c             \
                ./light/color.c             \
                ./light/IR.c                \
				./light/Light_fun.c         \
                ./light/Remote.c            \
				./light/Sun_func.c          \
                ./net/WIFI_config.c         \
                ./protocol_handle/protocol_handle.c     \
                ./protocol_handle/proto_parser_pack.c   \
                ./socket/tcp_client.c       \
                ./socket/tcp_perf.c         \
                ./socket/udp_perf.c         \
                ./sys/debuginfo.c           \
                ./sys/encryption.c          \
                ./sys/fota.c                \
                ./sys/RTC_time.c            \
                ./sys/store.c               \
                ./sys/ZG_AT_CMD.c           \
                ./sys/ZG_system.c           \
                ./zg_main.c

ZG_SDK_PORT_SOURCES = ./port/s907x_fota.c   \
                ./port/s907x_pwm.c          \
                ./port/s907x_store.c        \
                ./port/s907x_wlan.c         \
                ./port/zg_entry.c

MODULE_SOURCES = $(ZG_SDK_SOURCES)
MODULE_SOURCES += $(ZG_SDK_PORT_SOURCES)

include $(RULES_DIR)/make_objects.mk

