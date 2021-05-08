.PHONY:all run clean
CFLAGS:=-I../src/zopfli/
LFLAGS:=-lm
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

clean:
	cd .. && make clean
	-rm $(APP)
	-rm $(APP).o
