.POSIX:

PREFIX = /usr/local

BUILD_FLAGS = \
	-std=c99 \
	-D_POSIX_C_SOURCE=200809L \
	-D_XOPEN_SOURCE=500 \
	-Wall \
	-Wextra \
	-pedantic \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wwrite-strings \
	-Wshadow \
	-Wundef \
	-Wsign-conversion \
	$(CFLAGS)

VALGRIND = \
	--leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	--error-exitcode=1 \
	--trace-children=yes

OBJ = \
	src/kiss.o \
	src/cache.o \
	src/str.o \
	src/repo.o \
	src/file.o \
	src/pkg.o \
	src/util.o

HDR = \
	src/vec.h \
	src/cache.h \
	src/str.h \
	src/repo.h \
	src/file.h \
	src/pkg.h \
	src/util.h

kiss: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) $(BUILD_FLAGS) -g -O0 -c -o $@ $<

$(OBJ): $(HDR)

check:
	valgrind $(VALGRIND) ./kiss
	valgrind $(VALGRIND) ./kiss d
	valgrind $(VALGRIND) ./kiss s zlib
	valgrind $(VALGRIND) ./kiss v
	valgrind $(VALGRIND) ./kiss b

compdb:
	ninja -t compdb cc > compile_commands.json

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
