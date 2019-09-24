# We define the global AM_CPPFLAGS as everything we compile includes from these
# directories.
AM_CPPFLAGS = -isystem $(srcdir)/include -I$(srcdir)  -I$(srcdir)/../../include
#AM_CXXFLAGS =
#AM_LDFLAGS = 
#LDADD =

# Build rules for libraries.
noinst_LIBRARIES = libprops.a
libprops_a_SOURCES = src/props_reader.cc src/props_file_tracker.cc
