#include "main.h"
void wc(char *args[]);

void wc(char *args[]){
    int nlines=0;
    int nwords=0;
    int nbytes=0;
    int nread=0;
    bool wordflag=false;
    int argc=0;
    FILE *fp;
    for(int i=0;args[i]!=NULL;i++){
        argc++;
    }
    if(argc==1){
        fp=stdin;
    }else if(argc==2){
        fp=fopen(args[1],"r");
        if(!fp){
            perror("fopen");
            exit(0);
        }
    }
    char c;
    while((c=fgetc(fp))!=EOF){
        nbytes++;
        if(c=='\n')
            nlines++;
        if(wordflag && isspace(c)){
            wordflag=false;
        }else if(!wordflag　&& !isspace(c)){
            wordflag=true;
            nwords++;
        }
    }
    printf("% 8d % 8d % 8d \n",nlines,nwords,nbytes);
    exit(0);
}
