# Makefile for bilingua.

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = foreign

SUBDIRS = 3rdparty .

include 3rdparty/versions.am

STOUT = 3rdparty/stout
BOOST = 3rdparty/boost-$(BOOST_VERSION)
GLOG = 3rdparty/glog-$(GLOG_VERSION)

pkginclude_HEADERS =				\
  include/jvm.hpp				\
  include/java/io.hpp				\
  include/java/lang.hpp				\
  include/java/net.hpp				\
  include/org/apache/log4j.hpp			\
  include/org/apache/zookeeper.hpp

# Library. It is not installable presently because most people link it
# statically into their own library or binary and don't need or want
# any part of our library installed (which happens even if you attempt
# to do conditional installation via configure arguments, see
# configure.ac).
noinst_LTLIBRARIES = libbilingua.la

libbilingua_la_SOURCES =	\
  src/jvm.cpp			\
  src/org/apache/log4j.cpp

libbilingua_la_CPPFLAGS =	\
  -I$(srcdir)/include		\
  -I$(srcdir)/$(STOUT)/include	\
  -I$(BOOST)			\
  -I$(GLOG)/src			\
  $(JNI_CPPFLAGS)		\
  $(AM_CPPFLAGS)

libbilingua_la_LIBADD =		\
  $(JNI_LDFLAGS)		\
  $(GLOG)/libglog.la

# Tests.
check_PROGRAMS = tests

tests_SOURCES =			\
  src/tests/main.cpp

tests_CPPFLAGS =		\
  $(libbilingua_la_CPPFLAGS)

tests_LDADD =			\
  $(libbilingua_la_LIBADD)	\
  libbilingua.la

TESTS = tests
