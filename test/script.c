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
#include "squeeze.h"

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

//Base Choice Coverage
void ZopfliDeflate_BCCtttt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		options.verbose=1;
		options.numiterations=15;
		btype=2;
		in=malloc(100);
		insize=100;
		for(int i=0;i<insize;++i)in[i]=87;

		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);
		free(out);
		free(in);

		assert(insize>=outsize);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliDeflate_BCCfttt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		options.verbose=0;
		options.numiterations=15;
		btype=2;
		in=malloc(100);
		insize=100;
		for(int i=0;i<insize;++i)in[i]=87;

		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);
		free(out);
		free(in);

		assert(insize>=outsize);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==0);
		assert(status==0);
	}
}
void ZopfliDeflate_BCCtftt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		options.verbose=1;
		options.numiterations=1;
		btype=2;
		in=malloc(100);
		insize=100;
		for(int i=0;i<insize;++i)in[i]=87;

		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);
		free(out);

		options.numiterations=15;
		bp=0;
		out=0;
		size_t tmp=0;
		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&tmp);
		free(out);
		free(in);

		assert(outsize>=tmp);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliDeflate_BCCttft(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		options.verbose=1;
		options.numiterations=15;
		btype=0;
		in=malloc(100);
		insize=100;
		for(int i=0;i<insize;++i)in[i]=87;

		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);
		free(out);

		btype=2;
		bp=0;
		out=0;
		size_t tmp=0;
		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&tmp);
		free(out);
		free(in);

		assert(outsize>=tmp);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliDeflate_BCCtttf(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		options.verbose=1;
		options.numiterations=15;
		btype=0;
		in=malloc(10);
		insize=10;
		for(int i=0;i<insize;++i)in[i]=i%128;

		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&outsize);
		free(out);
		free(in);

		in=malloc(100);
		insize=100;
		for(int i=0;i<insize;++i)in[i]=i%128;
		bp=0;
		out=0;
		size_t tmp=0;
		ZopfliDeflate(&options,btype,final,in,insize,&bp,&out,&tmp);
		free(out);
		free(in);

		assert(outsize<=tmp);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliDeflatePart_BCCtttt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char* out=0;
		size_t outsize=0;

		options.verbose=1;
		options.numiterations=15;
		btype=2;
		in=malloc(100);
		insize=100;
		for(int i=0;i<insize;++i)in[i]=87;

		ZopfliDeflatePart(&options,btype,final,in,50,100,&bp,&out,&outsize);
		free(out);

		options.verbose=0;
		options.numiterations=1;
		btype=1;
		bp=0;
		out=0;
		size_t tmp=0;
		ZopfliDeflatePart(&options,btype,final,in,0,100,&bp,&out,&tmp);
		free(out);
		free(in);

		assert(outsize<=tmp);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliDeflatePart_BCCfttt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char *out=0,*out_=0;
		size_t outsize=0,outsize_=0;

		options.verbose=0;
		options.numiterations=15;
		btype=2;
		in=malloc(1000);
		insize=1000;
		for(int i=0;i<insize;++i)in[i]=i%128;

		ZopfliDeflatePart(&options,btype,final,in,0,100,&bp,&out,&outsize);
		bp=0;
		ZopfliDeflate(&options,btype,final,in,100,&bp,&out_,&outsize_);
		assert(outsize==outsize_);
		for(int i=0;i<outsize;++i){
			assert(out[i]==out_[i]);
		}
		free(out);
		free(out_);
		free(in);

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
		int status;
		waitpid(child,&status,0);

		assert(flag==0);
		assert(status==0);
	}
}
void ZopfliDeflatePart_BCCtftt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char *out=0,*out_=0;
		size_t outsize=0,outsize_=0;

		options.verbose=1;
		options.numiterations=1;
		btype=2;
		in=malloc(1000);
		insize=1000;
		for(int i=0;i<insize;++i)in[i]=i%128;

		ZopfliDeflatePart(&options,btype,final,in,0,100,&bp,&out,&outsize);
		bp=0;
		ZopfliDeflate(&options,btype,final,in,100,&bp,&out_,&outsize_);
		assert(outsize==outsize_);
		for(int i=0;i<outsize;++i){
			assert(out[i]==out_[i]);
		}
		free(out);
		free(out_);
		free(in);

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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliDeflatePart_BCCttft(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char *out=0,*out_=0;
		size_t outsize=0,outsize_=0;

		options.verbose=1;
		options.numiterations=15;
		btype=1;
		in=malloc(1000);
		insize=1000;
		for(int i=0;i<insize;++i)in[i]=i%128;

		ZopfliDeflatePart(&options,btype,final,in,0,100,&bp,&out,&outsize);
		bp=0;
		ZopfliDeflate(&options,btype,final,in,100,&bp,&out_,&outsize_);
		assert(outsize==outsize_);
		for(int i=0;i<outsize;++i){
			assert(out[i]==out_[i]);
		}
		free(out);
		free(out_);
		free(in);

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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliDeflatePart_BCCtttf(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		ZopfliOptions options;ZopfliInitOptions(&options);
		int btype=2;
		int final=1;
		unsigned char* in;
		size_t insize;
		unsigned char bp=0;
		unsigned char *out=0,*out_=0;
		size_t outsize=0,outsize_=0;

		options.verbose=1;
		options.numiterations=15;
		btype=2;
		in=malloc(1000);
		insize=1000;
		for(int i=0;i<insize;++i)in[i]=i%128;

		ZopfliDeflatePart(&options,btype,final,in,0,50,&bp,&out,&outsize);
		bp=0;
		ZopfliDeflate(&options,btype,final,in,50,&bp,&out_,&outsize_);
		assert(outsize==outsize_);
		for(int i=0;i<outsize;++i){
			assert(out[i]==out_[i]);
		}
		free(out);
		free(out_);
		free(in);

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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSize_BCCtttt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=1;
    	options.numiterations=15;
    	int btype=2;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,100,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,100,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSize(&store,0,store.size,btype);

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);

    	assert(ret>=.0f);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSize_BCCfttt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=0;
    	options.numiterations=15;
    	int btype=2;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,100,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,100,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSize(&store,0,store.size,btype);

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);

    	assert(ret>=.0f);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==0);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSize_BCCtftt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=1;
    	options.numiterations=1;
    	int btype=2;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,100,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,100,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSize(&store,0,store.size,btype);

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);

    	assert(ret>=.0f);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSize_BCCttft(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=1;
    	options.numiterations=15;
    	int btype=2;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,10,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,10,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSize(&store,0,store.size,btype);

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);

    	assert(ret>=.0f);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSize_BCCtttf(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=1;
    	options.numiterations=15;
    	int btype=1;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,256,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,256,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSize(&store,0,store.size,btype);

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);

    	assert(ret/8<=256.0f);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSizeAutoType_BCCttt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=1;
    	options.numiterations=15;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,256,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,256,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSizeAutoType(&store,0,store.size);

    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,2));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,1));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,0));

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSizeAutoType_BCCftt(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=0;
    	options.numiterations=15;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,256,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,256,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSizeAutoType(&store,0,store.size);

    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,2));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,1));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,0));

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==0);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSizeAutoType_BCCtft(){
	int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=1;
    	options.numiterations=1;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,256,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,256,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSizeAutoType(&store,0,store.size);

    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,2));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,1));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,0));

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
void ZopfliCalculateBlockSizeAutoType_BCCttf(){
		int fd[2];pipe(fd);
	int child=fork();
	assert(child>=0);
	if(child==0){
		close(fd[0]);
		dup2(fd[1],STDERR_FILENO);

		unsigned char in[1000];
		size_t insize=1000;
		for(int i=0;i<1000;++i)in[i]=i%128;
		ZopfliOptions options;ZopfliInitOptions(&options);
		options.verbose=1;
    	options.numiterations=15;

		ZopfliBlockState s;
    	ZopfliLZ77Store store;
    	ZopfliInitLZ77Store(in,&store);
    	ZopfliInitBlockState(&options,0,10,1,&s);
    	ZopfliLZ77Optimal(&s,in,0,10,options.numiterations,&store);
    	double ret=ZopfliCalculateBlockSizeAutoType(&store,0,store.size);

    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,2));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,1));
    	assert(ret<=ZopfliCalculateBlockSize(&store,0,store.size,0));

    	ZopfliCleanBlockState(&s);
    	ZopfliCleanLZ77Store(&store);
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
		int status;
		waitpid(child,&status,0);

		assert(flag==1);
		assert(status==0);
	}
}
//end of Base Choice Coverage

int main(){//call your test function here
	ZopfliDeflate_Example();

	//Logic Coverage
	ZopfliDeflate_PC0();
	ZopfliDeflate_PC1();
	ZopfliDeflatePart_PC0();
	ZopfliDeflatePart_PC1();
	ZopfliCalculateBlockSize_PC0();
	ZopfliCalculateBlockSize_PC1();
	ZopfliCalculateBlockSizeAutoType_PC0();
	printf("[TEST] Logic Coverage done.\n");

	//Base Case Coverage
	ZopfliDeflate_BCCtttt();
	ZopfliDeflate_BCCfttt();
	ZopfliDeflate_BCCtftt();
	ZopfliDeflate_BCCttft();
	ZopfliDeflate_BCCtttf();
	ZopfliDeflatePart_BCCtttt();
	ZopfliDeflatePart_BCCfttt();
	ZopfliDeflatePart_BCCtftt();
	ZopfliDeflatePart_BCCttft();
	ZopfliDeflatePart_BCCtttf();
	ZopfliCalculateBlockSize_BCCtttt();
	ZopfliCalculateBlockSize_BCCfttt();
	ZopfliCalculateBlockSize_BCCtftt();
	ZopfliCalculateBlockSize_BCCttft();
	ZopfliCalculateBlockSize_BCCtttf();
	ZopfliCalculateBlockSizeAutoType_BCCttt();
	ZopfliCalculateBlockSizeAutoType_BCCftt();
	ZopfliCalculateBlockSizeAutoType_BCCtft();
	ZopfliCalculateBlockSizeAutoType_BCCttf();
	printf("[TEST] Base Choice Coverage done.\n");

	printf("[TEST] All done.\n");
}