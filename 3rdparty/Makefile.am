# This Makefile is for building third party packages from
# tarballs. For autotools-based packages, we configure each of the
# packages to build static PIC binaries. Non-autotools based packages
# may be special cases; this Makefile is responsible for passing any
# special make or configure flags that might be required.

SUBDIRS = stout

# We need to add '--srcdir=.' needed because 'make distcheck' adds
#  '--srcdir=...' when configuring.
CONFIGURE_ARGS = @CONFIGURE_ARGS@ --enable-shared=no --with-pic --srcdir=.

include versions.am

STOUT = stout
BOOST = boost-$(BOOST_VERSION)
GLOG = glog-$(GLOG_VERSION)

EXTRA_DIST =			\
  $(BOOST).tar.gz		\
  $(GLOG).tar.gz

CLEAN_EXTRACTED =	\
  $(BOOST)		\
  $(GLOG)

# This is where the magic happens: we use stamp files as dependencies
# which cause the packages to get extracted as necessary. We also
# apply any patches as appropriate.
%-stamp: %.tar.gz
	gzip -d -c $^ | tar xf -
	test ! -e $(srcdir)/$*.patch || patch -d $* -p1 <$(srcdir)/$*.patch
	touch $@

$(BOOST)/boost: $(BOOST)-stamp

$(GLOG)/libglog.la: $(GLOG)-stamp
	cd $(GLOG) && ./configure $(CONFIGURE_ARGS) && \
          $(MAKE) $(AM_MAKEFLAGS)

# Dependencies for all-local.
ALL_LOCAL =				\
  $(STOUT)/Makefile			\
  $(BOOST)-stamp			\
  $(GLOG)/libglog.la

all-local: $(ALL_LOCAL)

clean-local:
	rm -r -f $(CLEAN_EXTRACTED)
	rm -f *-stamp
