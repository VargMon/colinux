# Standard Makefile header
ifeq ($(BUILD_ROOT),)
BUILD_ROOT=../../..
include $(BUILD_ROOT)/Makefile
else
BUILD_PATH := $(BUILD_PATH)/console-base
#-----------------------------------------------------------------

LOCAL_OBJS = \
	widget.o \
	main.o \
	console.o \

LOCAL_OBJS := $(addprefix $(BUILD_PATH)/,$(LOCAL_OBJS))

$(LOCAL_OBJS) : %.o : %.cpp
	$(TOOL_USER_GPP)

$(BUILD_PATH)/user-console.o : $(LOCAL_OBJS)
	$(TOOL_LD_RELOC)

CLEAN_FILES := $(CLEAN_FILES) \
	$(LOCAL_OBJS) $(BUILD_PATH)/user-console.o 

#-----------------------------------------------------------------
# include $(call BUILD_SUBDIRS, )
BUILD_PATH := $(shell dirname $(BUILD_PATH))
endif
