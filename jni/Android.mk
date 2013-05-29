
# OPTIONS
ENABLE_BLUETOOTH 		:= 0
ENABLE_DEBUG 			:= 0
ENABLE_DEBUG_LOGGING 	:= 0


# STORE VARS FOR AFTER CLEANING
LOCAL_PATH := $(call my-dir)
ORG_PATH := $(LOCAL_PATH)


# BUILD ALL EXTERNAL LIBS
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))


# BUILD EMU LIB
include $(CLEAR_VARS)
LOCAL_PATH := $(ORG_PATH)/
LOCAL_MODULE_TAGS := user
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libgenesis

LIB_EMU_DIR := libemu

MY_FILES 	:=	\
				$(wildcard $(LOCAL_PATH)/../genplusgx/*.c) \
				$(wildcard $(LOCAL_PATH)/../genplusgx/cart_hw/*.c) \
				$(wildcard $(LOCAL_PATH)/../genplusgx/cart_hw/svp/*.c) \
				$(wildcard $(LOCAL_PATH)/../genplusgx/input_hw/*.c) \
				$(wildcard $(LOCAL_PATH)/../genplusgx/m68k/*.c) \
				$(wildcard $(LOCAL_PATH)/../genplusgx/ntsc/*.c) \
				$(wildcard $(LOCAL_PATH)/../genplusgx/sound/*.c) \
				$(wildcard $(LOCAL_PATH)/../genplusgx/z80/*.c)			
		
		
# Correct the file names		
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%) 

# Emulator Engine source files that we will compile.
LOCAL_SRC_FILES := \
				$(MY_FILES) \
				$(LIB_EMU_DIR)/GraphicsDriver.cpp \
				$(LIB_EMU_DIR)/InputHandler.cpp \
				$(LIB_EMU_DIR)/Quad.cpp \
				EmulatorBridge.cpp \
				Application.cpp		
				

# INCLUDE DIRS
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/libemu/ \
					$(LOCAL_PATH)/../genplusgx/ \
					$(LOCAL_PATH)/../genplusgx/m68k \
					$(LOCAL_PATH)/../genplusgx/z80 \
					$(LOCAL_PATH)/../genplusgx/input_hw \
					$(LOCAL_PATH)/../genplusgx/sound \
					$(LOCAL_PATH)/../genplusgx/cart_hw \
					$(LOCAL_PATH)/../genplusgx/cart_hw/svp \
					$(LOCAL_PATH)/../genplusgx/ntsc \
					$(LOCAL_PATH)/../genplusgx/android

	   
# Debug
LOCAL_CFLAGS := -DLOG_TAG=\"DroidGEN\"
ifeq ($(ENABLE_DEBUG), 1)
	# DEBUG FLAGS
	LOCAL_CFLAGS += -g
else
	# RELEASE/Optimization flags
	LOCAL_CFLAGS += -Os \
					-fomit-frame-pointer \
					-fvisibility=hidden
					#-ffast-math \
					#-funit-at-a-time \
					#-fms-extensions \
					#-finline-functions
					#-funroll-loops \
					#-finline-functions \
					#-mfloat-abi=softfp \
					#-mfpu=neon
					#--param inline-unit-growth=1000 \
					#--param large-function-growth=5000 \
					#--param max-inline-insns-single=2450
					# -fvisibility=hidden
endif

ifeq ($(ENABLE_DEBUG_LOGGING), 1)
	LOCAL_CFLAGS += -DDEBUG_LOGGING
endif
					                  
			
# Compiler Warning flags
#LOCAL_CFLAGS += -Winline
LOCAL_CFLAGS += -Wno-write-strings

# Custom Emulator Flags
LOCAL_CFLAGS += -DHALDROID

# FCEU Flags
LOCAL_CFLAGS += -DLSB_FIRST -DFRAMESKIP -DMAXPATHLEN=1024 $(MACHDEP)

     
# C99
LOCAL_CFLAGS += -std=c99     


# Copy C Flags to CXX flags
LOCAL_CPPFLAGS := $(LOCAL_CFLAGS)
LOCAL_CXXFLAGS := $(LOCAL_CFLAGS)
     

# Native libs to link to
LOCAL_LDLIBS := -lz -llog -lGLESv2 


# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := libzip libgnupng 

                   
# Static libraries.              
LOCAL_STATIC_LIBRARIES :=   


# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true. However,
# it's difficult to do this for applications that are not supplied as
# part of a system image.
LOCAL_PRELINK_MODULE := false


include $(BUILD_SHARED_LIBRARY)


