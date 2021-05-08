#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deflate.h"
#include "gzip_container.h"
#include "zlib_container.h"

void testZopfliDeflate(){
	ZopfliOptions options;
	ZopfliInitOptions(&options);//util.c
	//options.verbose=1;
	int btype=2;//the block type
	int final=1;//whether to set the "final" bit on this block, must be the last block
	unsigned char in[10000];
	size_t insize=10000;
	unsigned char bp=0;
	unsigned char* out=0;
	size_t outsize=0;

	ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);
	//printf("[TEST] insize: %lu, bp: %d, outsize: %lu\n",insize,bp,outsize);

	assert(outsize!=0);
	assert(out!=0);
	free(out);
}

int main(){
	//call your test function here
	testZopfliDeflate();

	printf("[TEST] All done.\n");
}