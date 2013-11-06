# process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = subdir-objects


thoughtdir = $(pkgdatadir)/thoughts/


AM_CPPFLAGS = -DBRAIN_CONFIG='"$(sysconfdir)/brain.conf"' \
	-DBRAIN_BIN_DIR='"$(bindir)/"' -DBRAIN_THOUGHTS_DIR='"$(thoughtdir)"' \
	-DBRAIN_SOCKET='"$(localstatedir)/run/brain.sock"' \
	-DBRAIN_LOG='"$(prefix)/var/brain.log"'
	

bin_PROGRAMS = 	brsh thought braind

brsh_SOURCES = 	src/brsh/brsh.c \
				src/brsh/error.c \
				src/brsh/event.c \
				src/brsh/ui.c \
				src/client/client.c \
				src/client/setup.c \
				src/util/conf.c \
				src/util/confscan.c \
				src/util/util.c \
				src/mem/alloc.c \
				\
				src/config.h \
				src/fatal.h \
				includes/protocol.h \
				includes/client.h \
				src/protocol-int.h \
				src/brsh/event.h \
				src/brsh/ui.h \
				src/client/client-int.h \
				src/util/conf.h \
				src/util/confscan.h \
				src/util/util.h \
				src/mem/alloc.h

thought_SOURCES = src/thought/thought.c \
				src/util/conf.c \
				src/util/confscan.c \
				src/util/util.c \
				src/mem/alloc.c \
				\
				src/config.h \
				src/fatal.h \
				src/util/conf.h \
				src/util/confscan.h \
				src/util/util.h \
				src/mem/alloc.h
				
braind_SOURCES = src/braind/braind.c \
				src/braind/event.c \
				src/braind/log.c \
				src/braind/server.c \
				src/cont/array.c \
				src/cont/glue.c \
				src/cont/hashmap.c \
				src/cont/ptrset.c \
				src/intl/utf8.c \
				src/kn/bootstrap.c \
				src/kn/concept.c \
				src/kn/kn.c \
				src/kn/nl.c \
				src/kn/pattern.c \
				src/mem/alloc.c \
				src/nl/debug.c \
				src/nl/detmean.c \
				src/nl/genutt.c \
				src/nl/init.c \
				src/nl/iparse.c \
				src/nl/iplug.c \
				src/nl/isplit.c \
				src/nl/itok.c \
				src/nl/iwizer.c \
				src/nl/parts.c \
				src/util/conf.c \
				src/util/confscan.c \
				src/util/file-read.c \
				src/util/ifsai.c \
				src/util/sbuff.c \
				src/util/util.c \
				\
				src/config.h \
				src/fatal.h \
				src/protocol-int.h \
				includes/nlip.h \
				includes/protocol.h \
				\
				src/braind/log.h \
				src/braind/server.h \
				src/cont/array.h \
				src/cont/glue.h \
				src/cont/hashmap.h \
				src/cont/ptrset.h \
				src/intl/utf8.h \
				src/kn/common.h \
				src/kn/kn.h \
				src/kn/structs.h \
				src/mem/alloc.h \
				src/nl/common.h \
				src/nl/context.h \
				src/nl/input.h \
				src/nl/nl.h \
				src/nl/parts.h \
				src/util/conf.h \
				src/util/confscan.h \
				src/util/file-read.h \
				src/util/ifsai.h \
				src/util/sbuff.h \
				src/util/util.h \
				\
				src/nl/plug/builtins.c \
				src/nl/plug/acronyms.c \
				src/nl/plug/datetimes.c \
				src/nl/plug/emoticons.c \
				src/nl/plug/numbers.c \
				src/nl/plug/sentences.c \
				src/nl/plug/words.c \
				src/nl/plug/wordizer.c
braind_LDADD =	$(BRAIND_INTL_LIB)

sysconf_DATA = data/brain.conf

pkginclude_HEADERS = includes/client.h includes/nlip.h includes/protocol.h


#thought_DATA = data/thoughts/hello
thought_SCRIPTS = data/thoughts/hello


install-exec-hook:
	ln -s $(DESTDIR)$(bindir)/brsh \
	$(DESTDIR)$(bindir)/respond

uninstall-hook:
	rm $(DESTDIR)$(bindir)/respond

