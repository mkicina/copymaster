#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "options.h"
#include <dirent.h>

void FatalError(char c, const char* msg, int exit_status);
void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);
int toString(char a[]); 

int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);

    //-------------------------------------------------------------------
    // Kontrola hodnot prepinacov
    //-------------------------------------------------------------------

    // Vypis hodnot prepinacov odstrante z finalnej verzie
    
    //  PrintCopymasterOptions(&cpm_options);
    
    //-------------------------------------------------------------------
    // Osetrenie prepinacov pred kopirovanim
    //-------------------------------------------------------------------
    
    if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }
    
    if (argc == 3){
     const char* const filename=argv[1];
     const char* const filename2=argv[2];
     char buf[100];
     int des;
     if ((des = open(filename,O_RDONLY)) == -1){
	     FatalError(0,"SUBOR NEEXISTUJE", 21);
     }	     
     long l = lseek(des, 0L, SEEK_END);
     lseek(des, 0L, SEEK_SET);
     read(des,buf,l);
     int des2 = open(filename2,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR); 
     if (write(des2,buf,l) != 1) {
     	FatalError(0,"INA CHYBA", 21);
     }	     
    // write(des2,buf,l);
     close(des);
     close (des2);
  // bez prepinaca //
  }



    //prepinac -fast
if (cpm_options.fast){
     char buf[100000];
     int des;
     if ((des = open(argv[2],O_RDONLY)) == -1){
	     FatalError(cpm_options.fast,"SUBOR NEEXISTUJE", 21);
     }
     long l = lseek(des, 0L, SEEK_END);
     lseek(des, 0L, SEEK_SET);
     read(des,buf,l);
     int des2 = open(argv[3],O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR); 
     write(des2,buf,l);
     close(des);
     close (des2);
}
    // prepinac -fast

    //prepinac -slow
if (cpm_options.slow){
     char buf[1];
     int des;
     if ((des = open(argv[2],O_RDONLY)) == -1){
	     FatalError(cpm_options.slow,"SUBOR NEEXISTUJE", 21);
     }	     
    int des2 = open(argv[3],O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR); 
    long l = lseek(des, 0L, SEEK_END);
    lseek(des, 0L, SEEK_SET);
    for(int i=0;i<l;i++){
	    read(des,buf,1);
	    write(des2,buf,1);
    }
    close(des);
    close (des2);
}
    // prepinac -slow

// prepinac overwrite

if (cpm_options.overwrite){
     char buf[100];
     int des;
     if ((des = open(argv[2],O_RDONLY)) == -1){
	     FatalError(cpm_options.overwrite,"SUBOR NEEXISTUJE", 24);
     }
     long l = lseek(des, 0L, SEEK_END);
     lseek(des, 0L, SEEK_SET);
     read(des,buf,l);
     int des2; 
     if ((des2 = open(argv[3],O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR)) == -1){
	      close(des);
	      FatalError(cpm_options.overwrite,"SUBOR NEEXISTUJE", 24);
     }  
     write(des2,buf,l);
     close(des);
     close (des2);

}
    // prepinac -overwrite


// prepinac append

if (cpm_options.append){
     char buf[100];
     int des;
     if ((des = open(argv[2],O_RDONLY)) == -1){
	     FatalError(cpm_options.overwrite,"SUBOR NEEXISTUJE", 22);
     }
     long l = lseek(des, 0L, SEEK_END);
     lseek(des, 0L, SEEK_SET);
     read(des,buf,l);
     int des2; 
     if ((des2 = open(argv[3],O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR)) == -1){
	      close(des);
	      FatalError(cpm_options.overwrite,"SUBOR NEEXISTUJE", 22); 
     }  
     write(des2,buf,l);
     close(des);
     close (des2);
}
    // prepinac -append



    //prepinac -delete
if (cpm_options.delete_opt){
     char buf[100];
     int des,des2;

     struct stat buf2;
     lstat(argv[2], &buf2);

     if (!S_ISREG(buf2.st_mode)){
 	FatalError(cpm_options.delete_opt,"INA CHYBA", 26);
     }  

     if ((des = open(argv[2],O_RDONLY)) == -1){
	     FatalError(cpm_options.delete_opt,"SUBOR NEEXISTUJE", 26);
     }
     long l = lseek(des, 0L, SEEK_END);
     lseek(des, 0L, SEEK_SET);
     read(des,buf,l);
     if ((des2 = open(argv[3],O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR))==-1){
	     close(des);
	     FatalError(cpm_options.delete_opt,"INA CHYBA", 26);
     }
     
     write(des2,buf,l);
     if (remove(argv[2]) == -1){
	close(des);
	close(des2);
	FatalError(cpm_options.delete_opt,"SUBOR NEBOL ZMAZANY", 26);
     }
     close(des);
     close (des2);
}
    // prepinac -delete

if(cpm_options.chmod){
	int digit=0;
        int i=0;
        int j=0;
        int sum=0;
        int dec_sum=0;
        int base=1;
	for(i = 0, j =1000; i < 4; i++, j /= 10){
	  digit = argv[2][i] - '0';
	  sum = sum + (digit * j);
	}		
		
	int last_digit;
	while(sum != 0){
	  last_digit = sum % 10;
	  sum = sum / 10;
	  dec_sum = dec_sum + last_digit * base;
	  base = base * 8;
	}
			
	if(sum > 511 || sum < 0){
	  FatalError('m', "ZLE PRAVA", 23);
	}
	if(chmod(argv[4], dec_sum) < 0){
	  FatalError('m', "INA CHYBA", 23);
	}
   }


    //prepinac -inode
if (cpm_options.inode){

struct stat buf2;
lstat(argv[3], &buf2);

if (!S_ISREG(buf2.st_mode)){
	FatalError(cpm_options.inode,"ZLY TYP VSTUPNEHO SUBORU", 27);
} 

int des, inode; 
if ((des = open(argv[3],O_RDONLY)) == -1){
	   	FatalError(cpm_options.inode,"SUBOR NEEXISTUJE", 21);
     }

if (des < 0) {  
    FatalError(cpm_options.inode,"INA CHYBA", 27);
}  

struct stat file_stat;  
int ret;  
ret = fstat (des, &file_stat);  
if (ret < 0) {  
   FatalError(cpm_options.inode,"INA CHYBA", 27); 
} 

inode = file_stat.st_ino; 
	int num = toString(argv[2]);
	if (num != inode){
		FatalError(cpm_options.inode,"ZLY INODE", 27);
	}	
     char buf[100];
     long l = lseek(des, 0L, SEEK_END);
     lseek(des, 0L, SEEK_SET);
     read(des,buf,l);
     int des2 = open(argv[4],O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR); 
     write(des2,buf,l);
     close(des);
     close (des2);
     
}
    // prepinac -inode

if(cpm_options.link){
     int des;
     if((des = open(argv[2], O_RDONLY)) == -1){
	close(des);
       FatalError('K', "VSTUPNY SUBOR NEEXISTUJE", 30);
     }

     if(link(argv[2], argv[3]) < 0){
	close(des);
       FatalError('K', "VYSTUPNY SUBOR NEEXISTUJE", 30);
     }
}


    //prepinac -truncate
if (cpm_options.truncate){
     char buf[100];
     int des;
     if ((des = open(argv[3],O_RDONLY)) == -1){
	      FatalError(cpm_options.truncate,"SUBOR NEEXISTUJE",31);
     }
     long l = lseek(des, 0L, SEEK_END);
     lseek(des, 0L, SEEK_SET);
     read(des,buf,l);
     int des2 = open(argv[4],O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR); 
     write(des2,buf,l);
     int new_size=toString(argv[2]);;
     if (truncate(argv[3],new_size) == -1){
	  close(des);
	  close(des2);
	  FatalError(cpm_options.truncate,"VSTUPNY SUBOR NEZMENENY", 31);
     }
     close(des);
     close (des2);
}
    // prepinac -truncate


    // TODO Nezabudnut dalsie kontroly kombinacii prepinacov ...
    
    //-------------------------------------------------------------------
    // Kopirovanie suborov
    //-------------------------------------------------------------------
    
    // TODO Implementovat kopirovanie suborov
    
    // cpm_options.infile
    // cpm_options.outfile
    
    //-------------------------------------------------------------------
    // Vypis adresara
    //-------------------------------------------------------------------
    
    if (cpm_options.directory) {
        // TODO Implementovat vypis adresara
    }
        
    //-------------------------------------------------------------------
    // Osetrenie prepinacov po kopirovani
    //-------------------------------------------------------------------
    
    // TODO Implementovat osetrenie prepinacov po kopirovani

    return 0;
}

int toString(char a[]) {
  int c, sign, offset, n;
 
  if (a[0] == '-') {  
    sign = -1;
  }
 
  if (sign == -1) { 
    offset = 1;
  }
  else {
    offset = 0;
  }
 
  n = 0;
 
  for (c = offset; a[c] != '\0'; c++) {
    n = n * 10 + a[c] - '0';
  }
 
  if (sign == -1) {
    n = -n;
  }
 
  return n;
} 

void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno); 
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}

void PrintCopymasterOptions(struct CopymasterOptions* cpm_options)
{
    if (cpm_options == 0)
        return;
    
    printf("infile:        %s\n", cpm_options->infile);
    printf("outfile:       %s\n", cpm_options->outfile);
    
    printf("fast:          %d\n", cpm_options->fast);
    printf("slow:          %d\n", cpm_options->slow);
    printf("create:        %d\n", cpm_options->create);
    printf("create_mode:   %o\n", (unsigned int)cpm_options->create_mode);
    printf("overwrite:     %d\n", cpm_options->overwrite);
    printf("append:        %d\n", cpm_options->append);
    printf("lseek:         %d\n", cpm_options->lseek);
    
    printf("lseek_options.x:    %d\n", cpm_options->lseek_options.x);
    printf("lseek_options.pos1: %ld\n", cpm_options->lseek_options.pos1);
    printf("lseek_options.pos2: %ld\n", cpm_options->lseek_options.pos2);
    printf("lseek_options.num:  %lu\n", cpm_options->lseek_options.num);
    
    printf("directory:     %d\n", cpm_options->directory);
    printf("delete_opt:    %d\n", cpm_options->delete_opt);
    printf("chmod:         %d\n", cpm_options->chmod);
    printf("chmod_mode:    %o\n", (unsigned int)cpm_options->chmod_mode);
    printf("inode:         %d\n", cpm_options->inode);
    printf("inode_number:  %lu\n", cpm_options->inode_number);
    
  
    printf("umask:\t%d\n", cpm_options->umask);
    for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options->umask_options[i][0] == 0) {
            // dosli sme na koniec zoznamu nastaveni umask
            break;
        }
        printf("umask_options[%u]: %s\n", i, cpm_options->umask_options[i]);
    }
    
    printf("link:          %d\n", cpm_options->link);
    printf("truncate:      %d\n", cpm_options->truncate);
    printf("truncate_size: %ld\n", cpm_options->truncate_size);
    printf("sparse:        %d\n", cpm_options->sparse);
}
