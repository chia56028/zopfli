.PHONY:all run clean
CFLAGS:=-I../src/zopfli/ --coverage
LFLAGS:=-lm --coverage
ZOPFLILIB_SRC:=src/zopfli/blocksplitter.c src/zopfli/cache.c\
                src/zopfli/deflate.c src/zopfli/gzip_container.c\
                src/zopfli/hash.c src/zopfli/katajainen.c\
                src/zopfli/lz77.c src/zopfli/squeeze.c\
                src/zopfli/tree.c src/zopfli/util.c\
                src/zopfli/zlib_container.c src/zopfli/zopfli_lib.c
ZOPFLILIB_OBJ:=$(patsubst %.c,../obj/%.o,$(ZOPFLILIB_SRC))
APP:=script

all:
	cd .. && make
	make $(APP)

$(APP):$(APP).o $(ZOPFLILIB_OBJ)
	gcc $^ -o $@ $(LFLAGS)

$(APP).o:$(APP).c
	gcc $^ -o $@ $(CFLAGS) -c

run:
	make all
	./$(APP)

cov:
	gcov ../obj/src/zopfli/deflate.c
	grep '#####' ./deflate.c.gcov

clean:
	-rm $(APP)
	-rm *.o
	-rm *.gcno
	-rm *.gcda
	-rm *.gcov
	cd .. && make clean