########################################################################
# LIBRARY SOURCES - MUST BE IN THE SAME FOLDER as main.c (DO NOT CHANGE)
########################################################################

# Including memory heap model
LIBSRCS += $(FREERTOS_PATH)/portable/MemMang/heap_4.c

# Set mylib folder path.
# Do not change the MYLIB_PATH variable name.
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
# e.g. MYLIBPATH=$(SOURCELIB_ROOT)/../mylib
MYLIB_PATH=$(SOURCELIB_ROOT)/../repo/mylib

# Set folder path with header files to include.
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
CFLAGS += -I$(MYLIB_PATH)

# List all c file locations that must be included (use space as separator
# e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
LIBSRCS += $(MYLIB_PATH)/rcm_lta1000g.c \
           $(MYLIB_PATH)/rcm_mfs_led.c \
           $(MYLIB_PATH)/rcm_mfs_pb.c \
           $(MYLIB_PATH)/rcm_rcmsys.c \
           $(MYLIB_PATH)/rcm_tx.c \
           $(MYLIB_PATH)/rcm_packet.c \
           $(MYLIB_PATH)/rcm_cmdin.c \
           $(MYLIB_PATH)/rcm_rgb.c \
           $(MYLIB_PATH)/rcm_mfs_trimpot.c \
           $(MYLIB_PATH)/rcm_hamming.c \
           $(MYLIB_PATH)/rcm_switchbank.c \
           $(MYLIB_PATH)/rcm_ssd.c \
           $(MYLIB_PATH)/rcm_output.c \
           $(MYLIB_PATH)/rcm_cli_radio.c \
           $(MYLIB_PATH)/rcm_cli_task.c

###################################################
# LIBRARY SOURCES

NRF24L01PLUS_PATH=$(SOURCELIB_ROOT)/../sourcelib/components/peripherals/nrf24l01plus/

CFLAGS += -I$(NRF24L01PLUS_PATH)

LIBSRCS += $(wildcard $(NRF24L01PLUS_PATH)/*.c)
