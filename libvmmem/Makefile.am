AM_CPPFLAGS := \
    -W \
    -Wall \
    -Wextra \
    -Wunused
ACLOCAL_AMFLAGS = -I m4


libvmmem_sources = vmmem.cpp vmmem_wrapper.cpp

# Build libvmmem.so
lib_LTLIBRARIES = libvmmem.la

libvmmem_la_SOURCES := $(libvmmem_sources)
libvmmem_la_CPPFLAGS := -I$(top_srcdir)/include
libvmmem_la_CPPFLAGS += $(AM_CPPFLAGS) -Wno-error=pointer-to-int-cast

if MEMORY_HINT_DISABLED
libvmmem_la_CPPFLAGS += -DNO_MEMORY_HINT
endif

# Export headers
vmmem_includedir=$(includedir)
vmmem_include_HEADERS := include/vmmem.h
vmmem_include_HEADERS += include/vmmem_wrapper.h

pkgconfigdir := $(libdir)/pkgconfig
pkgconfig_DATA := libvmmem.pc
EXTRA_DIST := $(pkgconfig_DATA)
