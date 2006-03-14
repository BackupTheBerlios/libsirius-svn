lib_LTLIBRARIES = libsirius.la

cc_sources = \
	curl.cc \
	initializer.cc \
	urlencode.cc \
	sirius.cc

hh_sources = \
	container_base.hh \
	curl.hh \
	initializer.hh \
	urlencode.hh \
	sirius.hh

libsirius_la_SOURCES = $(hh_sources) $(cc_sources)
library_includedir=$(includedir)/sirius
library_include_HEADERS = $(hh_sources)
