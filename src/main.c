#include <stdio.h>
#include <dlfcn.h> 
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>      /*  timeval */
#include <sys/ioctl.h>     /*  ioctl  */
#include <pthread.h>
#include <signal.h>
#include <alsa/asoundlib.h>

#ifndef MAX_PATH
#define MAX_PATH 256
#endif


char *opt = NULL;
uint32_t  opt_srv=0,opt_cfg=0;
uint32_t  opt_num=1,opt_len=0,opt_fds=0, opt_pos=0;
char      *opt_arg, *opt_sek;

int usage(){
    printf("run exec commanline [xdump -f test.c p 1] \n");
}

void fatal(char *str, char *file, char *func, uint32_t line)
{
    printf("Fatal: %s - %s, %s@%d \n", str, file, func, line);
}

void hexdump(const unsigned char *src, size_t len, unsigned long fp)
{
	static const unsigned char hex[]	= "0123456789abcdef";
	static const unsigned char tmpl[]	= "                                                    |                |\n";
	unsigned char ln[sizeof tmpl];
	const unsigned char *sp, *se;
	unsigned char *h, *g;
	unsigned i, n;
	unsigned int id=0;

do{
    if(len <= 0) break;
    if(src==NULL) break;
    
	sp	= src;
	se	= sp + len;

	while (sp < se) {
		printf("%08x", fp+id);
		id+=16;
		
		memcpy(ln, tmpl, sizeof ln);
		
		h	= &ln[2];
		g	= &ln[53];

		for (n = 0; n < 2; n++) {
			for (i = 0; i < 8 && se - sp > 0; i++, sp++) {
				h[0]	= hex[0x0f & (*sp >> 4)];
				h[1]	= hex[0x0f & (*sp >> 0)];
				h	+= 3;

				*g++	= (isgraph(*sp))? *sp : '.';
			}

			h++;
		}
        printf("%s", ln);
		//fputs((char *)ln, fp);
	}


}while(0);

	return /* void */;
} /* hexdump() */



int load_dir(uint32_t flags){
    char *ptr = (char *) malloc( MAX_PATH );
    if(ptr==NULL){
        fatal("allocate memory ", __FILE__, __FUNCTION__, __LINE__);
        return NULL;    
    }
    getcwd(ptr, MAX_PATH);

    return ptr;
}

int main(int argc, char *argv[])
{
    char *ptr=NULL;
    char szdir[MAX_PATH];
    int  fd=-1;
    char *buffer=NULL;
    int  length=0,size =0;

    while(argc > 1){
        if(opt_num == argc) break;        
        
        opt= argv[opt_num];
        opt_len = strlen(opt);
        do{
            switch(*opt){
               case '-':

               break;
               case 'f':
                  opt_fds=1;
                  opt_num++;
                  opt_arg = argv[opt_num]; 
               break;
               case 'p':
                  opt_pos=1;
                  opt_num++;
                  opt_sek = argv[opt_num];
               break;
            }    
            opt+=1;
            opt_len--;
        }while(opt_len>0);

        
        opt_num++;
    }
    
    if((ptr=load_dir(0))==NULL){
        usage();
        goto END;    
    }
    
    if(!opt_fds || opt_arg ==NULL){
        usage();
        goto END;        
    }

    strcpy(szdir, opt_arg);

    fd = open(szdir, O_RDWR,0);
    if(fd == -1){
        strcpy(szdir, ptr);
        strcat(szdir,"/");
        strcat(szdir, opt_arg);
        fd = open(szdir, O_RDWR,0);
        if(fd == -1){
            fatal("open file ", __FILE__, __FUNCTION__, __LINE__);
            goto END;
        }
    }
   
    lseek(fd, 0, SEEK_END);
    length =  4096;
    if(opt_pos){
        size = length*atoi(opt_sek);
    }else{
        size = 0;    
    }
    lseek(fd, size, SEEK_SET); 
    buffer = (char *) malloc( length );
    if(buffer == NULL){
       fatal("allocate memory failed.,", __FILE__, __FUNCTION__, __LINE__);
       goto END;
    }
    
    if((size = read(fd, buffer, length))<=0){
       fatal("readfile.. ", __FILE__, __FUNCTION__, __LINE__);
       goto END;    
    }
    hexdump(buffer, length, size);
    

    END:
    if(buffer!=NULL) free(buffer), buffer=NULL;

    if(fd!=-1) close(fd);
    return 0;
}
