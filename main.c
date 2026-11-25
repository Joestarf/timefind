#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<errno.h>
#include<stdio.h>
#include<stddef.h>
#include<string.h>
int main(int argc,char *argv[]){
    int status=1;
    if(argc!=2){
        fprintf(stderr,"Invalid argument number\n");
        goto pro_end;
    }
    DIR *dirp=opendir(argv[1]);
    if(dirp==NULL){
        perror("opendir");
        goto pro_end;
    }
    while(1){
        errno=0;
        struct dirent *entry = readdir(dirp);
        if(entry == NULL){
            if(errno!=0){
                perror("readdir");
                goto clean_up;
            }
            break;
        }
//      if(entry->d_type==DT_REG) printf("%s\n",entry->d_name);
        size_t len=strlen(argv[1])+strlen(entry->d_name)+1;
        char path[len+1];
        int ret=snprintf(path,sizeof(path),"%s/%s",argv[1],entry->d_name);
        if(ret<0){
            perror("snprintf");
            goto clean_up;
        }
        if((size_t)ret>=sizeof(path)){
            fprintf(stderr,"Path buffer is too small.\n");
            goto clean_up;
        }
        struct stat info;
        if(lstat(path,&info)==-1){
            perror(path);
            goto clean_up;
        }

        if(S_ISREG(info.st_mode)) printf("%s\n",entry->d_name);
    }
    status=0;
clean_up:
    if(closedir(dirp)==-1){
        perror("closedir");
        status=1;
    }
pro_end:
    return status;
}
