bin_PROGRAMS = tv_ctrlpt tv_combo tv_device
AUTOMAKE_OPTIONS = foreign subdir-objects

tv_device_SOURCES = \
	common/sample_util.c \
	common/sample_util.h \
	common/tv_device.c \
	common/tv_device.h \
	linux/tv_device_main.c

tv_ctrlpt_SOURCES = \
	common/sample_util.c \
	common/sample_util.h \
	common/tv_ctrlpt.c \
	common/tv_ctrlpt.h \
	linux/tv_ctrlpt_main.c

tv_combo_SOURCES = \
	common/sample_util.c \
	common/sample_util.h \
	common/tv_ctrlpt.c \
	common/tv_ctrlpt.h \
	common/tv_device.c \
	common/tv_device.h \
	linux/tv_combo_main.c

AM_CFLAGS = $(GLIB_CFLAGS) $(LIBUPNP_CFLAGS) -Icommon/
LDADD = $(GLIB_LIBS) $(LIBUPNP_LIBS) 
