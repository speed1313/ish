#include "main.h"
//Macro definition
#define BUFLEN    1024     /* コマンド用のバッファの大きさ */
#define MAXARGNUM  256     /* 最大の引数の数 */
char precommand[]="!!";
char default_prompt[]="Command : ";
char prompt_str[512];
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
int complemental_replace(char *buf,histlinkedList *node);
void get_cwd_files(char *buf);
void wildcard(char *argv[],char *newcommand_buffer);
void redirect(char *args[],int pipenum,int savefd[2]);
void child_exec_command(char *args[],int NumBuiltin,int NumExternalCommand,int saveinputfd,int savefd[2]);
void ish_pipe(char *args[],int commandPosition,int NumBuiltin,int NumExternalCommand);
void ish_close(int fd);

int main(int argc, char *argv[])
{
    char command_buffer[BUFLEN]; /* コマンド用のバッファ */
    char *args[MAXARGNUM];       /* 引数へのポインタの配列 */
    int command_status;          /* コマンドの状態を表す
                                    command_status = 0 : フォアグラウンドで実行
                                    command_status = 1 : バックグラウンドで実行
                                    command_status = 2 : シェルの終了
                                    command_status = 3 : 何もしない */
    strcpy(prompt_str,default_prompt);
    /*script機能
    *  scriptの最後は改行しておくこと
    *
    */

    if(!isatty(0)){
        fprintf(stderr,"script\n");
        int nlines=0;
        while(fgets(command_buffer,BUFLEN,stdin)!=NULL){
            nlines++;
        }
        rewind(stdin);
        int i=0;
        while ((i<nlines) && (fgets(command_buffer,BUFLEN,stdin)!=NULL)){
            i++;
            char CurrentPath[512];
            CurrentPath[0]='\0';
            if(getcwd(CurrentPath,sizeof(CurrentPath))==NULL){
                perror("getcwd");
                exit(1);
            }
            char p[512];
            p[0]='\0';
            strcpy(p,prompt_str);
            str_replace(p,"\\w",CurrentPath);
            fprintf(stderr,"%s %s",p,command_buffer);
            if(strcmp(command_buffer,"\n")==0) {
                fprintf(stderr,"\n");
                continue;
            }
            command_status = parse(command_buffer, args);
            if(command_status == 2) {
                fprintf(stderr,"done.\n");
                hist_clear_list(histStackTop);
                clear_list(dirStackTop);
                clear_list(aliasStackTop);
                exit(EXIT_SUCCESS);
            } else if(command_status == 3) {
                continue;
            }
            execute_command(args, command_status);
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"done.\n");
        hist_clear_list(histStackTop);
        clear_list(dirStackTop);
        clear_list(aliasStackTop);
        exit(0);
    }
    for(;;) {
        //プロンプトデザイン処理
        char CurrentPath[512];
        CurrentPath[0]='\0';
        if(getcwd(CurrentPath,sizeof(CurrentPath))==NULL){
            perror("getcwd");
            exit(1);
        }
        char p[512];
        p[0]='\0';
        strcpy(p,prompt_str);
        str_replace(p,"\\w",CurrentPath);
        // プロンプトを表示
        fprintf(stderr,"%s",p);
        /*
         *  標準入力から１行を command_buffer へ読み込む
         *  入力が何もなければ改行を出力してプロンプト表示へ戻る
         */
        if(fgets(command_buffer, BUFLEN, stdin) == NULL) {
            fprintf(stderr,"\n");
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
            fprintf(stderr,"done.\n");
            hist_clear_list(histStackTop);
            clear_list(dirStackTop);
            clear_list(aliasStackTop);
            exit(EXIT_SUCCESS);
        } else if(command_status == 3) {
            continue;
        }
        //コマンドを実行する
        execute_command(args, command_status);
        fprintf(stderr,"\n");
    }
    return 0;
}

/*----------------------------------------------------------------------------
 *  関数名   : parse
 *  作業内容 : バッファ内のコマンドと引数を解析する
 *  引数     :
 *  返り値   : コマンドの状態を表す :
 *                0 : フォアグラウンドで実行
 *                1 : バックグラウンドで実行
 *                2 : シェルの終了
 *                3 : 何もしない
 *  注意     : 引数が cat a.txtの時, まずargs[0]に"cat a.txt"の先頭アドレスが格納されるが,
 *            その後cat'\0'a.txt'\0'となるためargs[0]には"cat"のみ抽出される
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
    /* !!*/
    if(strstr(buffer,"!!")){
        if(histStackTop==NULL){
            fprintf(stderr,"ish: event not found: !!\n");
            return 3;
        }
        str_replace(buffer,precommand,histStackTop->name);
    }
    if(complemental_replace(buffer,histStackTop)==-1){/*!string機能*/
            return 3;
    }
    if(strcmp(buffer, "exit") == 0) {
        status = 2;
        return status;
    }
    /*文字列をparse*/
    while(*buffer != '\0') {
        while(*buffer == ' ' || *buffer == '\t') {
            *(buffer++) = '\0';
        }
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
 *  引数     :
 *  返り値   :
 *  注意     :
 *--------------------------------------------------------------------------*/

void execute_command(char *args[],    /* 引数の配列 */
                     int command_status)     /* コマンドの状態 */
{
    int pid=0;      /* プロセスＩＤ */
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
    char newcommand_buffer[BUFLEN];
    wildcard(args,newcommand_buffer);
    int argc=0;
    for(int i=0;args[i]!=NULL;i++){
        argc++;
    }
    if(command_status==0){
        ish_pipe(args,argc-1,NumBuiltin,NumExternalCommand);
        return;
    }else{/*background*/
        pid=fork();
        if(pid==-1){
            perror("fork");
            exit(1);
        }else if(pid==0){//child
            ish_pipe(args,argc-1,NumBuiltin,NumExternalCommand);
            exit(0);
        }else{//parent
            return;
        }
    }
    return;
}

/*--------original functions----------*/
void str_replace(char *buf,char *str1,char *str2){//replace str1 to str2 in buf strings
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
/*change ! or !string to past command*/
int complemental_replace(char *buf,histlinkedList *node){
  char tmp[1024];
  char *p;
  int  match=0;
  while ((p = strstr(buf,"!")) != NULL) {//bufの!を全て置換し終わるまでループ
    match=0;
    int i=0;
    char strbuf[1024];
    char *str2;
    *p='\0';
    p++;//pは!strの"s"をさす
    while((isspace(*p)==0)&&((*p)!=NULL)){//空白になるまでstrbufにstringを格納
        strbuf[i++]=*p;
        p++;
    }
    strbuf[i]='\0';//strbufに!strのstrがはいる
    i=0;
    strcpy(tmp, p);//tmpに!strの後ろ部分を格納
    if(node==NULL){
        fprintf(stderr,"ish: event not found: %s\n",strbuf);
        return -1;
    }
    while(node!=NULL){//strbufがhiststackに含まれるかチェック
        int matchNum=0;
        for(int i=0;i<strlen(strbuf);i++){
            if(strbuf[i]==node->name[i]){
                matchNum++;
            }
        }
        if(matchNum==strlen(strbuf)){//マッチした文字数がstrbufと等しければそれに置換するフラグ
            match=1;
            break;
        }
        node=node->next;
    }
    if(match==1){//置換
        strcat(buf,node->name);
    }else{
        fprintf(stderr,"ish: event not found: %s\n",strbuf);
        return -1;
    }
    strcat(buf, tmp);//!strの後ろ部分を結合
  }
  return 0;
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
void wildcard(char *args[],char *newcommand_buffer){
    newcommand_buffer[0]='\0';
    int argc=0;
    int isWild=0;
    char fileList[256];
    for(int i=0;args[i]!=NULL;i++){
        argc++;
    }
    for(int i=0;i<argc;i++){
        if(strcmp(args[i],"*")==0){
            isWild=1;
            fileList[0]='\0';
            get_cwd_files(fileList);
            args[i]=fileList;
        }
        strcat(newcommand_buffer,args[i]);
        strcat(newcommand_buffer," ");
    }
    if(isWild){
        parse(newcommand_buffer,args);
    }
}
void redirect(char *args[],int pipenum,int savefd[2]){
    int fd1,fd2;
    for(int i=0;args[i]!=NULL;i++){
        if(strcmp(args[i],"<")==0){
            if(args[i+1]!=NULL){
                if((fd1=open(args[i+1],O_RDONLY))==-1){
                    perror("open");
                    exit(1);
                }
                args[i]=NULL;
                if(pipenum==0||pipenum==1){
                    if((savefd[0]=fcntl(0,F_DUPFD,10))==-1){
                        perror("fcntl");
                        exit(1);
                    }
                }
                dup2(fd1,0);
                ish_close(fd1);
                i++;
            }else{
                fprintf(stderr,"redirect error\n");
            }
        }
        if(strcmp(args[i],">")==0){
            if(args[i+1]!=NULL){
                if((fd2=open(args[i+1],O_WRONLY|O_CREAT|O_TRUNC, 0666))==-1){
                    perror("open");
                    exit(1);
                }
                args[i]=NULL;
                if(pipenum==0||pipenum==1){
                    if((savefd[1]=fcntl(1,F_DUPFD,10))==-1){
                        perror("fcntl");
                        exit(1);
                    }
                }
                dup2(fd2,1);
                ish_close(fd2);
                i++;
            }else{
                fprintf(stderr,"redirect usage: command </> filename");

            }
        }
    }
    return;
}

void ish_pipe(char *args[],int commandPosition,int NumBuiltin,int NumExternalCommand){
    int pipefd[2] = {0};
    int status=0;
    int pipenum=0;
    int saveinputfd;
    for(int i=commandPosition;i>=0;i--){
        if(strcmp(args[i],"|")==0){
            pipenum++;
            args[i]=NULL;
            if(pipe(pipefd)==-1){
                perror("pipe");
                exit(1);
            }
            int pid = fork();
            if(pid==-1){//fork error
                perror("fork");
                exit(1);
            }else if(pid == 0) {//child
                ish_close(pipefd[0]);
                dup2(pipefd[1], 1);
                ish_close(pipefd[1]);
                ish_pipe(args,i-1,NumBuiltin,NumExternalCommand);
                exit(0);
            }else {//parent
            //出力パイプを閉じる, stdinをパイプの出力先にする.
            // "|"の右側のコマンドを実行
                ish_close(pipefd[1]);
                if(args[i+1]==NULL){
                    fprintf(stderr,"usage: command | command\n");
                    if(pipenum==1){//./ishをexitしないようにreturn
                        return;
                    }else{
                        exit(1);
                    }
                }
                //ishと同じプロセス(一番右のコマンド)はstdinを退避
                if(pipenum==1){
                    if((saveinputfd=fcntl(0,F_DUPFD,10))==-1){
                        perror("fcntl");
                        exit(1);
                    }
                }
                dup2(pipefd[0], 0);//0をパイプにむける
                ish_close(pipefd[0]);
                for(;;){
                    if((waitpid(pid,&status,WUNTRACED))==-1){
                        perror("waitpid");
                        break;
                    }
                    if(WIFEXITED(status)||WIFSIGNALED(status)){
                        break;
                    }
                }//パイプから子プロセスの標準出力が入ってくる
                int savefd[2]={0,1};
                redirect(&(args[i+1]),pipenum,savefd);
                child_exec_command(&args[i+1],NumBuiltin,NumExternalCommand,saveinputfd,savefd);
                if(pipenum==1){//一番右のコマンドの時
                    if(savefd[0]!=0){//stdinがリダイレクトされていれば戻す
                        ish_close(savefd[0]);
                    }
                    if(savefd[1]!=1){//stdoutがリダイレクトされていれば戻す
                        dup2(savefd[1],1);
                        ish_close(savefd[1]);
                    }
                //一番右のコマンドなら保存したstdinファイルをstdinに割り当てる
                    dup2(saveinputfd,0);
                    ish_close(saveinputfd);
                    return;//一番右コマンドならreturnで抜ける
                }
                ish_close(savefd[0]);
                ish_close(savefd[1]);
                exit(1);//子プロセスならプロセスを終える
            }
        }
    }
    //一番左のコマンドまたは単一コマンドを実行
    int savefd[2]={0,1};//stdin, stdoutの退避場所
    redirect(args,pipenum,savefd);//リダイレクトがあればリダイレクト(savefdにstdin, stdoutはセーブされる)
    child_exec_command(args,NumBuiltin,NumExternalCommand,savefd[0],savefd);
    if(pipenum==0){//単一コマンドの場合
        if(savefd[0]!=0){
            dup2(savefd[0],0);
            ish_close(savefd[0]);
        }
        if(savefd[1]!=1){
            dup2(savefd[1],1);
            ish_close(savefd[1]);
        }
        return;
    }
    exit(0);
}

void child_exec_command(char *args[],int NumBuiltin,int NumExternalCommand,int saveinputfd,int savefd[2]){
    for(int i=0;i<NumBuiltin;i++){
        if(strcmp(args[0],BuiltinCommand[i])==0){
            (*builtin_func[i])(args);
            return;
        }
    }
    int pid=fork();
    if(pid==-1){
        perror("fork");
        exit(1);
    }else if(pid==0){//child
        /*ish_func*/
        if(saveinputfd!=savefd[0] && saveinputfd!=0){
            ish_close(saveinputfd);
        }
        if(savefd[0]!=0){
            ish_close(savefd[0]);
        }
        if(savefd[1]!=1){
            ish_close(savefd[1]);
        }
        for(int i=0;i<NumExternalCommand;i++){
            if(strcmp(args[0],ExternalCommand[i])==0){
                (*external_func[i])(args);
                exit(0);
            }
        }
        execvp(args[0],args);
        fprintf(stderr, "error :execve failed at main.c\n");
        fprintf(stderr,"ish : perhaps command not found : %s\n",args[0]);
        exit(1);
    }else{//parent
        int status;
        for(;;){
            if((waitpid(pid,&status,WUNTRACED))==-1){
                perror("waitpid");
                break;
            }
            if(WIFEXITED(status)||WIFSIGNALED(status)){
                break;
            }
        }
        return;
    }
    return;
}

void ish_close(int fd){
    if(close(fd)==-1){
        fprintf(stderr,"fd=%d\n",fd);
        perror("close");
        exit(1);
    }
}
/*-- END OF FILE -----------------------------------------------------------*/
