# ish memo

## Useful C standard function
- strtok\
コマンド引数を分割するとき.(strtokだと ls|grep .txt などができない気がする)
- chdir\
カレントディレクトリを変える時
- getenv\
環境変数を得るとき($HOMEなど)
- getopt\
コマンドラインのオプション解析
- strrchr
## system call
- fork
- exec
- wait
- open
- pipe\
pipelineで必要
- select
- mmap
- dupa
- chdir
- execvp


## Candidates for original features
- wc
- echo
- diff
- cp
- pipeline
- redirectaion
- tab補完
- implement option
- daemon (logd)
    - httpd

# 注意
- cdするとプロセスが増えてhistoryも受け継がれない？？historyにbuiltinコマンドが追加されない
子プロセスでしっかりexitしたら治った
- printfデバッグなどして差し込むときにif()hoge; のところに差し込むとおかしくなるから注意