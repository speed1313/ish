#include "main.h"
void (*ish_builtin_func[]) (char *args[]) = {
    &ish_cd
    /*&history,
    &pushd,
    &dirs,
    &popd,
    &prompt,
    &alias,
    &unalias,
    &!!,
    &!string
    */
};
void ish_cd(char *args[]){
    if(args[1]==NULL){
        if(chdir(getenv("HOME")) != 0){
            perror("cd error");
        }
    }else {
         if(chdir(args[1]) != 0){
            perror("cd error");
         }
    }
    return ;
}
