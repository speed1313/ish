#include "main.h"
//Macro definition
#define BUFLEN    1024     /* コマンド用のバッファの大きさ */
#define MAXARGNUM  256     /* 最大の引数の数 */
char precommand[]="!!";
char default_prompt[]="Command : ";
char *prompt_str;


char *BuiltinCommand[]={
    "cd",
    "pushd",
    "dirs",
    "popd",
    "prompt",
    "alias",
    "unalias"
};
char *ExternalCommand[]={
    "history",
    "echo",
    "wc"
};
void (*builtin_func[]) (char *args[]) = {
    &ish_cd,
    &pushd,
    &dirs,
    &popd,
    &prompt,
    &alias,
    &unalias,
};
void (*external_func[]) (char *args[]) = {
    &history,
    &echo,
    &wc


};
/*
 *  ローカルプロトタイプ宣言
 */
int parse(char [], char *[]);
void execute_command(char *[], int);
void str_replace(char *buf,char *str1,char *str2);
void complemental_replace(char *buf,linkedList *ptr);
void get_cwd_files(char *buf);
char **wildcard(char *argv[]);
/*----------------------------------------------------------------------------
 *
 *  関数名   : main
 *
 *  作業内容 : シェルのプロンプトを実現する
 *
 *  引数     :
 *
 *  返り値   :
 *
 *  注意     :
 *
 *--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char command_buffer[BUFLEN]; /* コマンド用のバッファ */
    char *args[MAXARGNUM];       /* 引数へのポインタの配列 */
    int command_status;          /* コマンドの状態を表す
                                    command_status = 0 : フォアグラウンドで実行
                                    command_status = 1 : バックグラウンドで実行
                                    command_status = 2 : シェルの終了
                                    command_status = 3 : 何もしない */
    prompt_str=default_prompt;
    //script機能
    if(!isatty(0)){
        while (gets(command_buffer)!=NULL) {
            char CurrentPath[512];
            CurrentPath[0]='\0';
            int pathlen=0;
            getcwd(CurrentPath, pathlen);
            fprintf(stdout,"%s %s\n",prompt_str,command_buffer);
            if(command_buffer==NULL) {
                printf("\n");
                continue;
            }
            command_status = parse(command_buffer, args);
            if(command_status == 2) {
                printf("done.\n");
                clear_list(histStackTop);
                clear_list(dirStackTop);
                exit(EXIT_SUCCESS);
            } else if(command_status == 3) {
                continue;
            }
            execute_command(args, command_status);

        }
        return 0;
    }

    /*
     *  無限にループする
     */


    for(;;) {
        // プロンプトを表示
        char CurrentPath[512];
        CurrentPath[0]='\0';
        int pathlen=0;
        getcwd(CurrentPath, pathlen);
        printf("%s",prompt_str);
        /*
         *  標準入力から１行を command_buffer へ読み込む
         *  入力が何もなければ改行を出力してプロンプト表示へ戻る
         */

        if(fgets(command_buffer, BUFLEN, stdin) == NULL) {
            printf("\n");
            continue;
        }
        /*
         *  入力されたバッファ内のコマンドを解析する
         *
         *  返り値はコマンドの状態
         */
        command_status = parse(command_buffer, args);
        /*
         *  終了コマンドならばプログラムを終了
         *  引数が何もなければプロンプト表示へ戻る
         */
        if(command_status == 2) {
            printf("done.\n");
            clear_list(histStackTop);
            clear_list(dirStackTop);
            exit(EXIT_SUCCESS);
        } else if(command_status == 3) {
            continue;
        }
        //コマンドを実行する
        execute_command(args, command_status);
    }

    return 0;
}

/*----------------------------------------------------------------------------
 *
 *  関数名   : parse
 *
 *  作業内容 : バッファ内のコマンドと引数を解析する
 *
 *  引数     :
 *
 *  返り値   : コマンドの状態を表す :
 *                0 : フォアグラウンドで実行
 *                1 : バックグラウンドで実行
 *                2 : シェルの終了
 *                3 : 何もしない
 *
 *  注意     : 引数が cat a.txtの時, まずargs[0]に"cat a.txt"の先頭アドレスが格納されるが,
 *            その後cat'\0'a.txt'\0'となるためargs[0]には"cat"のみ抽出される
 *
 *--------------------------------------------------------------------------*/

int parse(char buffer[],        /* バッファ */
          char *args[])         /* 引数へのポインタ配列 */
{
    int arg_index;   /* 引数用のインデックス */
    int status;   /* コマンドの状態を表す */
    arg_index = 0;
    status = 0;
    if(*(buffer + (strlen(buffer) - 1))=='\n'){
        *(buffer + (strlen(buffer) - 1)) = '\0';
    }
    if(histStackTop!=NULL){
        str_replace(buffer,precommand,histStackTop->name);
        complemental_replace(buffer,histStackTop);
    }
    /*
     *  バッファが終了を表すコマンド（"exit"）ならば
     *  コマンドの状態を表す返り値を 2 に設定してリターンする
     */

    if(strcmp(buffer, "exit") == 0) {

        status = 2;
        return status;
    }

    /*
     *  バッファ内の文字がなくなるまで繰り返す
     *  （ヌル文字が出てくるまで繰り返す）
     */

    while(*buffer != '\0') {

        /*
         *  空白類（空白とタブ）をヌル文字に置き換える
         *  これによってバッファ内の各引数が分割される
         */

        while(*buffer == ' ' || *buffer == '\t') {
            *(buffer++) = '\0';
        }

        /*
         * 空白の後が終端文字であればループを抜ける
         */

        if(*buffer == '\0') {
            break;
        }

        /*
         *  空白部分は読み飛ばされたはず
         *  buffer は現在は arg_index + 1 個めの引数の先頭を指している
         *
         *  引数の先頭へのポインタを引数へのポインタ配列に格納する
         */

        args[arg_index] = buffer;
        ++arg_index;

        /*
         *  引数部分を読み飛ばす
         *  （ヌル文字でも空白類でもない場合に読み進める）
         */

        while((*buffer != '\0') && (*buffer != ' ') && (*buffer != '\t')) {
            ++buffer;
        }
    }
    /*
     *  最後の引数の次にはヌルへのポインタを格納する
     */
    args[arg_index] = NULL;
    /*
     *  最後の引数をチェックして "&" ならば
     *  "&" を引数から削る
     *  コマンドの状態を表す status に 1 を設定する
     *  そうでなければ status に 0 を設定する
     */
    if(arg_index > 0 && strcmp(args[arg_index - 1], "&") == 0) {

        --arg_index;
        args[arg_index] = '\0';
        status = 1;

    } else {

        status = 0;

    }

    /*
     *  引数が何もなかった場合
     */

    if(arg_index == 0) {
        status = 3;
    }
    /*
     *  コマンドの状態を返す
     */
    return status;
}

/*----------------------------------------------------------------------------
 *
 *  関数名   : execute_command
 *
 *  作業内容 : 引数として与えられたコマンドを実行する
 *             コマンドの状態がフォアグラウンドならば、コマンドを
 *             実行している子プロセスの終了を待つ
 *             バックグラウンドならば子プロセスの終了を待たずに
 *             main 関数に返る（プロンプト表示に戻る）
 *
 *  引数     :
 *
 *  返り値   :
 *
 *  注意     :
 *
 *--------------------------------------------------------------------------*/

void execute_command(char *args[],    /* 引数の配列 */
                     int command_status)     /* コマンドの状態 */
{
    int pid;      /* プロセスＩＤ */
    int status=0;   /* 子プロセスの終了ステータス */
    int NumBuiltin=sizeof(BuiltinCommand)/sizeof(char *);
    int NumExternalCommand=sizeof(ExternalCommand)/sizeof(char *);
    int isBuiltin=0;
    char histstr[]="history";
    pushHistory(args);
    for(int i=0;args[i]!=NULL;i++){
        strcpy(alias_tmp,args[i]);
        args[i]=search_alias(args[i],aliasStackTop);
    }

    args=wildcard(args);
    if(command_status==0){
        for(int i=0;i<NumBuiltin;i++){
            if(strcmp(args[0],BuiltinCommand[i])==0){
                isBuiltin=1;
                printf("builtin %s will execute\n",BuiltinCommand[i]);
                (*builtin_func[i])(args);
                return;
            }
        }
    }
    if(isBuiltin==0){
        pid=fork();
        /*
        *  子プロセスの場合には引数として与えられたものを実行する
        *
        *  引数の配列は以下を仮定している
        *  ・第１引数には実行されるプログラムを示す文字列が格納されている
        *  ・引数の配列はヌルポインタで終了している
        */
        if(pid==-1){
            fprintf(stderr,"error :fork failed at main.c\n");
        }else if(pid==0){//child
            FILE *fd1;
            FILE *fd2;
            /*built in (pipe用)*/
            for(int i=0;args[i]!=NULL;i++){
                if(strcmp(args[i],"<")==0){
                    if(args[i+1]!=NULL && !(fd1=open(args[i+1],O_RDONLY))){
                        perror("fopen");
                        exit(1);
                    }
                    printf("stdin redirect\n");
                    args[i]=NULL;
                    i++;
                    dup2(fd1,0);
                    close(fd1);
                }
                if(strcmp(args[i],">")==0){
                    if(args[i+1]!=NULL && !(fd2=open(args[i+1],O_WRONLY))){
                        perror("fopen");
                        exit(1);
                    }
                    printf("stdout redirect\n");
                    dup2(fd2,1);
                    args[i]=NULL;
                    close(fd2);
                    i++;
                }
            }
            for(int i=0;i<NumBuiltin;i++){
                if(strcmp(args[0],BuiltinCommand[i])==0){
                    //fprintf(stderr,"builtin %s will execute\n",BuiltinCommand[i]);
                    (*builtin_func[i])(args);
                    exit(0);
                }
            }
            /*ish_func*/
            for(int i=0;i<NumExternalCommand;i++){
                if(strcmp(args[0],ExternalCommand[i])==0){
                    //fprintf(stderr,"external %s will execute\n",ExternalCommand[i]);
                    (*external_func[i])(args);
                    exit(0);
                }
            }
            //fprintf(stderr,"%s will execute\n",args[0]);
            execvp(args[0],args);
            fprintf(stderr, "error :execve failed at main.c\n");
            fprintf(stderr,"ish : perhaps command not found : %s\n",args[0]);
            exit(1);
        }else{//parent
            //printf("command_status=%d\n",command_status);
            if(command_status==0){//foreground
                for(;;){
                    if((waitpid(pid,&status,WUNTRACED))==-1){
                        fprintf(stderr,"error :waitpid failed at main.c\n");
                        break;
                    }
                    if(WIFEXITED(status)||WIFSIGNALED(status)){
                        break;
                    }
                }
                return;
            }else{//background
                fprintf(stderr,"background\n");
                return;
            }
        }
    }


    return;
}


//general functions
void str_replace(char *buf,char *str1,char *str2){
  char tmp[1024];
  char *p;
  while ((p = strstr(buf, str1)) != NULL) {
    *p = '\0';
    p += strlen(str1);
    strcpy(tmp, p);
    strcat(buf, str2);
    strcat(buf, tmp);
  }
}


void complemental_replace(char *buf,linkedList *ptr){
  char tmp[1024];
  char *p;
  char *p2;
  int  match=0;
  while ((p = strstr(buf,"!")) != NULL) {
    p2=p;
    int i=0;
    char strbuf[1024];
    char *str2;
    p++;
    while(isspace(*p)==0&&(p!=NULL)){
        strbuf[i++]=*p;
        p++;
    }
    strbuf[i]='\0';//strbufに!strのstrがはいる
    i=0;
    *p2='\0';
    p2 += (strlen(strbuf)+1);//"!str” 分進める
    strcpy(tmp, p2);//tmpに!strの後ろ部分を格納
    while(ptr!=NULL){
        int matchNum=0;
        for(int i=0;i<strlen(strbuf);i++){
            if(strbuf[i]==ptr->name[i]){
                matchNum++;
            }
        }
        if(matchNum==strlen(strbuf)){
            match=1;
            break;
        }
        ptr=ptr->next;
    }
    if(match==1){
        strcat(buf,ptr->name);
    }else{
        strcat(buf," ");
    }
    strcat(buf, tmp);
  }
}

void  get_cwd_files(char *buf){
    DIR *dp;
    struct dirent *dir;
    char *path = ".";
    if((dp = opendir(path))==NULL){
        perror("Can't open direcotry");
        exit(1);
    }
    while((dir = readdir(dp)) != NULL){
        if((dir->d_type==DT_REG)&&(dir->d_name[0]!='.')){
            strcat(buf,dir->d_name);
            strcat(buf," ");
        }
    }
    closedir(dp);
    return;
}
char **wildcard(char *args[]){
    char *newargs[MAXARGNUM];
    char newcommand_buffer[BUFLEN];
    newcommand_buffer[0]='\0';
    int argc=0;
    int isWild=0;
    for(int i=0;args[i]!=NULL;i++){
        argc++;
    }
    for(int i=0;i<argc;i++){
        if(strcmp(args[i],"*")==0){
            isWild=0;
            char fileList[256];
            fileList[0]='\0';
            get_cwd_files(fileList);
            strcpy(args[i],fileList);
        }
        strcat(newcommand_buffer,args[i]);
        strcat(newcommand_buffer," ");
    }
    if(isWild){
        parse(newcommand_buffer,newargs);
        return newargs;
    }else{
        return args;
    }
}

/*-- END OF FILE -----------------------------------------------------------*/
