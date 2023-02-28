#include <dirent.h>
#include <grp.h>  //struct group
#include <pwd.h>  //struct passwd
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h> //extern char *ctime (const time_t *__timer) __THROW;

#define SIZE 1000

char* filename[SIZE];//存储文件名
void ls();
void ls_a();
void ls_l();
void mode_to_letters(int mode, char str[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);
int is_R = 0;
int is_a = 0;
int is_l = 0;
int main(int argc, char* argv[]) {
    // int i = 0;
    // for (i = 0; i < argc; i++) {    // 分析参数
    //     if (strlen(argv[i]) < 2) {  // 单参数
    //         if (argv[i][0] == '-') {
    //             if (argv[i][1] == 'R') {  //-R
    //                 is_R = 1;
    //             }
    //             else if (argv[i][1] == 'a') {  //-a
    //                 is_a = 1;
    //             }
    //              else if (argv[i][1] == 'l') {  //-l
    //                 is_l = 1;
    //             }
    //         }
    //         else {  // 指定文件
    //         }
    //     }
    //     else {  // 多参数-ral len=4
    //         int len = strlen(argv[i]);
    //         for (int j = 1; j <= len; j++) {
    //             if (argv[i][0] == '-') {
    //                 if (argv[i][j] == 'R') {  //-R
    //                     is_R = 1;
    //                 }
    //                 else if (argv[i][j] == 'a') {  //-a
    //                     is_a = 1;
    //                 }
    //                 else if (argv[i][j] == 'l') {  //-l
    //                     is_l = 1;
    //                 }
    //             }
    //         }
    //     }
    // }
    ls_l();
    return 0;
}

void ls() {
    DIR* pdir = opendir(".");
    struct dirent* pdirent = NULL;
    if (pdir == NULL) {
        printf("打开失败");
    }
    while ((pdirent = readdir(pdir)) != NULL) {
        if (pdirent->d_name[0] == '.') {
            continue;
        }
        printf("%s  ", pdirent->d_name);
    }
    closedir(pdir);
}
void ls_l() {
    int cnt = 0;
    DIR* pdir = opendir(".");
    struct stat info;
    struct dirent* pdirent = NULL;
    if (pdir == NULL) {
        printf("打开失败");
    }
    while ((pdirent = readdir(pdir)) != NULL) {  // 读取文件

        if (pdirent->d_name[0] == '.') {
            continue;
        }
        filename[cnt] =(char*)malloc(sizeof(char) * strlen(pdirent->d_name) + 1);
        strcpy(filename[cnt], pdirent->d_name);
        if (stat(filename[cnt], &info) == 0) {
            char modestr[11];
            mode_to_letters(info.st_mode, modestr);
            printf("%s", modestr);
            printf("%4d ", (int)info.st_nlink);
            printf("%-8s ", uid_to_name(info.st_uid));
            printf("%-8s ", gid_to_name(info.st_gid));
            printf("%8ld ", (long)info.st_size);
            printf("%.12s ", 4 + ctime(&info.st_mtime));
            printf("%s\n", filename[cnt]);
        }
        cnt++;
    }
    closedir(pdir);
}
void mode_to_letters(int mode, char str[]) {
    strcpy(str, "----------");
    if (S_ISDIR(mode))
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';
    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';
    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';
    if (mode & S_IROTH)
        str[7] = 'r';
    if (mode & S_IWOTH)
        str[8] = 'w';
    if (mode & S_IXOTH)
        str[9] = 'x';
}
char* uid_to_name(uid_t uid) {
    struct passwd* pw_ptr;
    static char numstr[10];

    if ((pw_ptr = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    } else
        return pw_ptr->pw_name;
}

char* gid_to_name(gid_t gid) {
    struct group* grp_ptr;
    static char numstr[10];

    if ((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    } else
        return grp_ptr->gr_name;
}