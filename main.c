#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<errno.h>
#include<stdio.h>
#include<stddef.h>
#include<string.h>
void normalize_path(char *path){
    if(strlen(path)<=1) return;
    int i=1,j=1;
    for(;path[j]!='\0';j++){
       if(path[j]=='/'&&path[j-1]=='/') continue;
       path[i++]=path[j];
    }
    path[i]='\0';
}
int traverse(const char *path){
    int status=0;
    DIR *dirp=opendir(path);
    if(dirp==NULL){
        perror(path);
        status=1;
        goto func_end;
    }
    while(1){
        errno=0;
        struct dirent *entry = readdir(dirp);
        if(entry == NULL){
            if(errno!=0){
                perror("readdir");
                status=1;
            }
            break;
        }
        if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0) continue;
        size_t len=strlen(path)+strlen(entry->d_name)+1;
        char sub_path[len+1];
        int ret;
        if(path[strlen(path)-1]!='/')
           ret=snprintf(sub_path,sizeof(sub_path),"%s/%s",path,entry->d_name);
        else 
           ret=snprintf(sub_path,sizeof(sub_path),"%s%s",path,entry->d_name);
        if(ret<0){
            perror("snprintf");
            status=1;
            continue;
        }
        if((size_t)ret>=sizeof(sub_path)){
            fprintf(stderr,"Path buffer is too small.\n");
            status=1;
            continue;
        }
        struct stat info;
        if(lstat(sub_path,&info)==-1){
            perror(sub_path);
            status=1;
            continue;
        }
        if(S_ISREG(info.st_mode)) printf("%s\n",sub_path);
        else if(S_ISDIR(info.st_mode)) status|=traverse(sub_path);
    }
if(closedir(dirp)==-1){
    perror("closedir");
    status=1;
}
func_end:
    return status;
}
int main(int argc,char *argv[]){
    int status=1;
    if(argc!=2){
        fprintf(stderr,"Invalid argument number\n");
        goto pro_end;
    }
    normalize_path(argv[1]);
//    printf("%s\n",argv[1]);
    if(traverse(argv[1])) goto pro_end;
    status=0;
pro_end:
    return status;
}
