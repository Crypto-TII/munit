# Using µnit is very simple; just include the header and add the C
# file to your sources.  That said, here is a simple Makefile to build
# the example.

CSTD:=99
OPENMP:=n
ASAN:=n
UBSAN:=n
EXTENSION:=
TEST_ENV:=
CFLAGS:=
AGGRESSIVE_WARNINGS=n
OUT_DIR:= build/

ifeq ($(CC),pgcc)
        CFLAGS+=-c$(CSTD)
else
        CFLAGS+=-std=c$(CSTD)
endif

ifeq ($(OPENMP),y)
        ifeq ($(CC),pgcc)
                CFLAGS+=-mp
        else
                CFLAGS+=-fopenmp
        endif
endif

ifneq ($(SANITIZER),)
        CFLAGS+=-fsanitize=$(SANITIZER)
endif

ifneq ($(CC),pgcc)
        ifeq ($(EXTRA_WARNINGS),y)
                CFLAGS+=-Wall -Wextra -Werror
        endif

        ifeq ($(ASAN),y)
                CFLAGS+=-fsanitize=address
        endif

        ifeq ($(UBSAN),y)
                CFLAGS+=-fsanitize=undefined
        endif
endif

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

example$(EXTENSION): munit.h munit.c tests/example.c
	$(CC) $(CFLAGS) -I./ -o ${OUT_DIR}$@ munit.c tests/example.c


test_nested_suites$(EXTENSION): munit.h munit.c tests/test_nested_suites.c
	$(CC) $(CFLAGS) -I./ -DMUNIT_FAIL_NO_TEST_RUN -o ${OUT_DIR}$@ munit.c tests/test_nested_suites.c

test: ${OUT_DIR} example$(EXTENSION) test_nested_suites$(EXTENSION)
	$(TEST_ENV) ./${OUT_DIR}example$(EXTENSION)
	$(TEST_ENV) ./${OUT_DIR}test_nested_suites$(EXTENSION)

clean:
	rm -f ${OUT_DIR}example$(EXTENSION)
	rm -f ${OUT_DIR}test_*$(EXTENSION)
	rm -rf ${OUT_DIR}

all: ${OUT_DIR} example$(EXTENSION) test_nested_suites$(EXTENSION)
