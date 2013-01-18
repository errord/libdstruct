# libdstruct
libdstruct_LIBRARIES = libdstruct.a
libdstructdir = @libdir@
libdstruct_a_SOURCES = baseio.cpp 	\
			dsioformat.cpp	\
			dstruct.cpp	\
			dsconfig.cpp	\
			easycheck.cpp

# xml2ds ds2xml test_dstruct
bin_PROGRAMS = xml2ds ds2xml test_dstruct
xml2ds_SOURCES = xml2ds.cpp
xml2ds_DEPENDENCIES = libdstruct.a
ds2xml_SOURCES = ds2xml.cpp
ds2xml_DEPENDENCIES = libdstruct.a
test_dstruct_SOURCES = test_dstruct.cpp
test_dstruct_DEPENDENCIES = libdstruct.a
LDADD = ./libdstruct.a

# install headers file
pkginclude_HEADERS = libdstruct.h	\
		dstruct.h		\
		baseio.h		\
		dsbase.h		\
		dsconfig.h		\
		dsioformat.h

