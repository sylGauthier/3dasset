DEPS := 3dmr

PREFIX ?= /usr/local
INCLUDEDIR ?= include
LIBDIR ?= lib

CFLAGS ?= -std=c99 -pedantic -march=native -Wall -O3 -I.
CFLAGS += $(shell pkg-config --cflags $(DEPS))
LDLIBS += -L. -l3dasset -lm $(shell pkg-config --libs-only-l $(DEPS))
LDFLAGS += $(shell pkg-config --libs-only-L --libs-only-other $(DEPS))

OBJECTS := $(patsubst %.c,%.o,$(wildcard src/*.c))
TESTS := $(patsubst %.c,%,$(wildcard test/*.c))

LIB := lib3dasset.a

.PHONY: all
all: $(LIB)

$(LIB): $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	rm -rf $(LIB) $(OBJECTS) $(TESTS)

install: $(LIB) 3dasset.pc
	mkdir -p $(PREFIX)/$(INCLUDEDIR) $(PREFIX)/$(LIBDIR)/pkgconfig $(PREFIX)/bin
	cp 3dasset.h $(PREFIX)/$(INCLUDEDIR)
	cp $(LIB) $(PREFIX)/$(LIBDIR)
	cp 3dasset.pc $(PREFIX)/$(LIBDIR)/pkgconfig

.PHONY: 3dasset.pc
3dasset.pc:
	printf 'prefix=%s\nincludedir=%s\nlibdir=%s\n\nName: %s\nDescription: %s\nVersion: %s\nCflags: %s\nLibs: %s\nRequires: %s' \
		'$(PREFIX)' \
		'$${prefix}/$(INCLUDEDIR)' \
		'$${prefix}/$(LIBDIR)' \
		'lib3dasset' \
		'An asset library for 3dmr' \
		'0.1' \
		'-I$${includedir}' \
		'-L$${libdir} -l3dasset' \
		'$(DEPS)' \
		> $@

$(TESTS): $(LIB)

test: $(TESTS)

