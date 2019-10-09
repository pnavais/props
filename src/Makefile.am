AM_CXXFLAGS = $(WARN_CXXFLAGS)
AM_CPPFLAGS = $(MAIN_LIB_HEADER) $(PROPS_LIB_HEADER)

bin_PROGRAMS = props

props_SOURCES = props.cc  props_cli.cc  props_cmd.cc  props_cmd_factory.cc  props_help_cmd.cc  \
props_search_result.cc  props_tracker_cmd.cc props_unknown_cmd.cc props_search_cmd.cc \
arg_parser.cc string_utils.cc file_utils.cc
#props_LDFLAGS = -Wl,-Bdynamic
props_LDADD = $(PROPS_LIB_FUNC)

if HAVE_PTHREAD
    AM_CXXFLAGS += $(PCRE_CFLAGS) $(PTHREAD_CFLAGS) -DHAVE_PTHREADS
    props_LDADD += $(PTHREAD_LIBS)
endif
