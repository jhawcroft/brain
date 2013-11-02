# process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = brsh
brsh_SOURCES = 	src/client/client.c \
				src/brsh/error.c \
				src/brsh/event.c \
				src/brsh/ui.c \
				src/brsh/brsh.c \
				src/util/confscan.c \
				src/util/util.c \
				src/mem/alloc.c
brsh_LDADD	= @FORMLIB@ @CURSESLIB@
