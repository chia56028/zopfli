#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "deflate.h"
#include "gzip_container.h"
#include "zlib_container.h"

//Example
void ZopfliDeflate_Example(){
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
//end of Example

//Logic Coverage (when |C|=1, PC=CC=CACC)
void ZopfliDeflate_PC0(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;
		ZopfliInitOptions(&options);
		options.verbose=1;
		int btype=2;
		int final=1;
		unsigned char in[10000];
		size_t insize=10000;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);

		exit(0);
	}else{
		close(fd[1]);

		int flag=0;
		char buf[100];
		while(1){
			int n=read(fd[0],buf,100);
			if(n>0){
				flag=1;
				//write(1,buf,n);
			}else{
				break;
			}
		}

		close(fd[0]);
		waitpid(child,0,0);

		assert(flag==1);//verbose exist
	}
}
void ZopfliDeflate_PC1(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;
		ZopfliInitOptions(&options);
		options.verbose=0;
		int btype=2;
		int final=1;
		unsigned char in[10000];
		size_t insize=10000;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);

		exit(0);
	}else{
		close(fd[1]);

		int flag=0;
		char buf[100];
		while(1){
			int n=read(fd[0],buf,100);
			if(n>0){
				flag=1;
				//write(1,buf,n);
			}else{
				break;
			}
		}

		close(fd[0]);
		waitpid(child,0,0);

		assert(flag==0);//no verbose
	}
}
void ZopfliDeflatePart_PC0(){
	ZopfliOptions options;
	ZopfliInitOptions(&options);
	int btype=0;
	int final=1;
	unsigned char in[10000];
	size_t insize=10000;

	int res[2];
	{
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;
		ZopfliDeflatePart(&options,btype,final,in,0,insize,&bp,&out,&outsize);
		res[0]=outsize;
		free(out);
	}
	{
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;
		ZopfliDeflatePart(&options,2,final,in,0,insize,&bp,&out,&outsize);
		res[1]=outsize;
		free(out);
	}

	assert(res[0]>=res[1]);
}
void ZopfliDeflatePart_PC1(){
	ZopfliOptions options;
	ZopfliInitOptions(&options);
	int btype=2;
	int final=1;
	unsigned char in[10000];
	size_t insize=10000;

	int res[2];
	{
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;
		ZopfliDeflatePart(&options,btype,final,in,0,insize,&bp,&out,&outsize);
		res[0]=outsize;
		free(out);
	}
	{
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;
		ZopfliDeflatePart(&options,1,final,in,0,insize,&bp,&out,&outsize);
		res[1]=outsize;
		free(out);
	}

	assert(res[0]<=res[1]);
}
void ZopfliCalculateBlockSize_PC0() {
    const unsigned char data;
    ZopfliLZ77Store lz77;
    ZopfliInitLZ77Store(&data, &lz77);
    size_t lstart = 0;
    size_t lend = 0;
    int btype;

    double result[2];
    {
        btype = 0;
        result[0] = ZopfliCalculateBlockSize(&lz77, lstart, lend, btype);
    }
    {
        btype = 1;
        result[1] = ZopfliCalculateBlockSize(&lz77, lstart, lend, btype);
    }

    // printf("%lf %lf\n", result[0], result[1]);
    assert(result[0] <= result[1]);
}
void ZopfliCalculateBlockSize_PC1() {
    const unsigned char data;
    ZopfliLZ77Store lz77;
    ZopfliInitLZ77Store(&data, &lz77);
    size_t lstart = 0;
    size_t lend = 0;
    int btype;

    double result[2];
    {
        btype = 2;
        result[0] = ZopfliCalculateBlockSize(&lz77, lstart, lend, btype);
    }
    {
        btype = 1;
        result[1] = ZopfliCalculateBlockSize(&lz77, lstart, lend, btype);
    }

    assert(result[0] >= result[1]);
}
void ZopfliCalculateBlockSizeAutoType_PC0() {
    const unsigned char data;
    ZopfliLZ77Store lz77;
    ZopfliInitLZ77Store(&data, &lz77);

    size_t lstart = 0;
    size_t lend = 0;

    double result[2];
    {
        lz77.size = 0;
        result[0] = ZopfliCalculateBlockSizeAutoType(&lz77, lstart, lend);
    }
    {
        lz77.size = 1001;
        result[1] = ZopfliCalculateBlockSizeAutoType(&lz77, lstart, lend);
    }
    assert(result[0] == result[1]);
}
//end of Logic Coverage

int main(){
	//call your test function here
	ZopfliDeflate_Example();
	ZopfliDeflate_PC0();
	ZopfliDeflate_PC1();
	ZopfliDeflatePart_PC0();
	ZopfliDeflatePart_PC1();

	ZopfliCalculateBlockSize_PC0();
	ZopfliCalculateBlockSize_PC1();
	ZopfliCalculateBlockSizeAutoType_PC0();

	printf("[TEST] All done.\n");
}