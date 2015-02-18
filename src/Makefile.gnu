# This is a plain Makefile for general Phorward Foundation Toolkit development.
# It compiles libphorward.a quickly and out of the box without autotools.

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I.

AWK=awk
LIBPHORWARD = libphorward.a
PROTOFILE =	proto.h
LIBHEADER = phorward.h
PATHEXT = PATH="../run:$(PATH)"

SRC = \
	base/array.c \
	base/ccl.c \
	base/dbg.c \
	base/list.c \
	base/memory.c \
	parse/ast.c \
	parse/bnf.c \
	parse/gram.c \
	parse/ll.c \
	parse/lr.c \
	parse/parse.c \
	parse/prod.c \
	parse/scan.c \
	parse/sym.c \
	regex/dfa.c \
	regex/direct.c \
	regex/lex.c \
	regex/misc.c \
	regex/nfa.c \
	regex/pattern.c \
	regex/regex.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	util/system.c \
	util/xml.c \
	value/value.c \
	value/value.conv.c \
	value/value.get.c \
	value/value.set.c

HSRC = \
	base/array.h \
	base/ccl.h \
	base/dbg.h \
	base/list.h \
	parse/parse.h \
	phorward.tpl.h \
	regex/regex.h \
	util/xml.h \
	value/value.h \
	$(PROTOFILE)

OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(LIBPHORWARD)

$(LIBPHORWARD): $(LIBHEADER) $(OBJ)
	$(AR) rv $@ $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(LIBPHORWARD)

clean_all: clean
	-rm value/value.get.c
	-rm value/value.set.c
	-rm value/value.conv.c

# Prototypes Files
$(PROTOFILE): $(SRC)
	$(PATHEXT) pproto $(SRC) >$@

# Library Header
$(LIBHEADER): $(HSRC)
	$(PATHEXT) filehead phorward.h "Phorward Foundation Toolkit Global Header" \
		>$@
	$(PATHEXT) pinclude phorward.tpl.h >>$@

# pvalue object functions (generated from definitions in pvalue.h comments)
value/value.get.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_get=1 value/value.h >$@

value/value.set.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_set=1 value/value.h >$@

value/value.conv.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_conv=1 value/value.h >$@

