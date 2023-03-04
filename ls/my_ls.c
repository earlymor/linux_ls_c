#include <dirent.h>
#include <grp.h>  //struct group
#include <pwd.h>  //struct passwd
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>  //extern char *ctime (const time_t *__timer) __THROW;
#include <unistd.h>
//-a、-l、-R、-t、-r、-i、-s
// -a：–all的缩写，显示所有的文件，包括隐藏文件(以.开头的文件)
// -l：列出长数据串，显示出文件的属性与权限等数据信息(常用)
// -t：以修改时间排序
// -r：–reverse，将排序结果以倒序方式显示
// -i：结合-l参数，列出每个文件的inode
// -s: –size 以块大小为单位列出所有文件的大小
// -R: –recursive 同时列出所有子目录层
#define SIZE 100

void mode_to_letters(int mode, char str[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);
void display(char* path);
void sort(char** a, int n);
int color(char* path);
void getdir_l(char* path);
void getdir(char* path);
void getfile(char* path);
void rsort(char** a, int n);
void show_inode(char* pathname);
void show_info(char* path, char* filename);
void tsort(char** a, int n);
void show_block(char* pathname);
// void upper(char** a, char** b, int n);
int ls_a = 0;
int ls_l = 0;
int ls_R = 0;
int ls_r = 0;
int ls_t = 0;
int ls_i = 0;
int ls_s = 0;
int per_R = 0;
int per_r = 0;
int count = 0;

int main(int argc, char** argv) {
    // 判断-参数
    int cnt = 0;  // -参数组数
    int num = 0;  // 文件名参数个数
    struct stat info;
    char pathname[1024];
    char* dirname[SIZE] = {};
    char* filename[SIZE] = {};
    int k = 0,  // dirname
        l = 0;  // filename
    // 目录文件及普通文件个数
    for (int i = 1; i < argc; i++) {  // 读参 存储文件名目录名
        //-参数
        if (argv[i][0] == '-') {
            for (int j = 1; j < strlen(argv[i]); j++) {
                switch (argv[i][j]) {
                    case 'a':
                        ls_a = 1;
                        // printf("ls_a:%d\n", ls_a);
                        break;
                    case 'l':
                        // printf("ls_l:%d\n", ls_l);
                        ls_l = 1;
                        break;
                    case 'R':
                        // printf("ls_R:%d\n", ls_R);
                        ls_R = 1;
                        break;
                    case 'r':
                        // printf("ls_r:%d\n", ls_r);
                        ls_r = 1;
                        break;
                    case 't':
                        // printf("ls_t:%d\n", ls_t);
                        ls_t = 1;
                        break;
                    case 'i':
                        // printf("ls_i:%d\n", ls_i);
                        ls_i = 1;
                        break;
                    case 's':
                        // printf("ls_s:%d\n", ls_s);
                        ls_s = 1;
                        break;
                    default:
                        printf("ls: 不适用的选项 -- %c\n", argv[i][j]);
                        printf("请尝试执行 \"ls --help\" 来获取更多信息。\n");
                        exit(0);
                }
            }
            cnt++;
        } else {
            // 文件名参数 排除不正确文件名 获取文件名数组
            sprintf(pathname, "%s", argv[i]);
            if (stat(pathname, &info) == 0) {
                if (info.st_mode & S_IFDIR) {  // 目录文件
                    dirname[k] =
                        (char*)malloc(sizeof(char) * strlen(argv[i]) + 1);
                    strcpy(dirname[k], argv[i]);
                    dirname[k++][strlen(argv[i])] = '\0';
                } else {  // 普通文件
                    filename[l] =
                        (char*)malloc(sizeof(char) * strlen(argv[i]) + 1);
                    strcpy(filename[l], argv[i]);
                    filename[l++][strlen(argv[i])] = '\0';
                }
                num++;
            } else {
                printf("ls: 无法访问 '%s': 没有那个文件或目录", argv[i]);
            }
        }
    }

    // 排序

    // 展示函数
    if (cnt + 1 == argc) {  // 纯参数 当前文件夹
        strcpy(pathname, ".");
        pathname[1] = '\0';
        display(pathname);
    } else {
        if (ls_r == 1) {
            rsort(filename, cnt);
            rsort(dirname, num);
        } else if (ls_t == 1) {
            tsort(filename, cnt);
            tsort(dirname, num);
        } else {
            sort(filename, cnt);
            sort(dirname, num);
        }
        for (int i = 0; i < k; i++) {  // 文件/目录文件
            pathname[strlen(dirname[i])] = '\0';
            sprintf(pathname, "%s/%s", ".", dirname[i]);
            display(pathname);
        }
        for (int i = 0; i < l; i++) {  // 文件/目录文件
            pathname[strlen(filename[i])] = '\0';
            sprintf(pathname, "%s", filename[i]);
            display(pathname);
        }
    }
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
void display(char* path) {  // 路径 及参数
    struct stat info;
    if (stat(path, &info) != 0) {
        printf("ls: 无法访问 '%s': 没有那个文件或目录\n", path);
        exit(1);
    }
    if (info.st_mode & S_IFDIR) {  // 目录文件
        if (ls_l == 1) {
            getdir_l(path);

        } else {
            getdir(path);
        }
    } else {
        getfile(path);
    }
}
void sort(char** a, int n) {
    char temp[201];
    // char* b[n+1];
    // upper(a,b,n);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (strcmp(a[i], a[j]) > 0) {
                strcpy(temp, a[i]);
                strcpy(a[i], a[j]);
                strcpy(a[j], temp);
            }
        }
    }
}
void getdir_l(char* path) {
    DIR* pdir;
    char pathname[1024];
    char* filename[SIZE] = {};
    char* dirname[SIZE] = {};
    int cnt = 0;  // 总文件名数量
    int num = 0;  // 目录文件数量
    struct stat info;
    struct dirent* pdirent = NULL;
    pdir = opendir(path);
    while ((pdirent = readdir(pdir)) != NULL) {
        filename[cnt] =
            (char*)malloc(sizeof(char) *
                          (strlen(pdirent->d_name) + 1));  // copy文件名结尾'\0'
        strcpy(filename[cnt], pdirent->d_name);
        filename[cnt][strlen(pdirent->d_name)] = '\0';
        sprintf(pathname, "%s/%s", path, filename[cnt]);
        cnt++;
        if (stat(pathname, &info) != 0) {  // 调用失败
            perror("stat1");
            exit(0);
        } else {
            count += info.st_blocks / 2;
            if (info.st_mode & S_IFDIR) {
                dirname[num] =
                    (char*)malloc(sizeof(char) * strlen(pdirent->d_name) +
                                  1);  // copy目录文件名结尾'\0'
                strcpy(dirname[num], pdirent->d_name);
                dirname[num++][strlen(pdirent->d_name)] = '\0';
            }
        }
    }
    // 排序
    if (ls_r == 1) {
        rsort(filename, cnt);
        rsort(dirname, num);
    } else if (ls_t == 1) {
        tsort(filename, cnt);
        tsort(dirname, num);
    } else {
        sort(filename, cnt);
        sort(dirname, num);
    }

    // 总输出
    if (ls_R == 1) {
        if (per_R == 0)
            printf("%s:\n", path);
        else {
            printf("\n%s:\n", path);
        }
    }
    printf("总计 %d\n", count);
    count = 0;
    for (int i = 0; i < cnt; i++) {
        sprintf(pathname, "%s/%s", path, filename[i]);
        if (ls_a == 0)
            if (filename[i][0] == '.' || !strcmp(filename[i], ".."))
                continue;
        if (ls_i == 1) {
            show_inode(pathname);
        }
        if (ls_s == 1) {
            show_block(pathname);
        }
        show_info(pathname, filename[i]);
    }
    // 递归
    if (ls_R == 1) {
        // 0->1
        // if (per_r == 1 && per_R == 0) {
        //     printf("\n");
        // }

        // // 1->2
        // if (per_R == 1) {
        //     printf("\n");
        // }

        // 遍历目录
        for (int i = 0; i < num; i++) {
            if (dirname[i][0] == '.' || !strcmp(dirname[i], ".."))
                continue;
            sprintf(pathname, "%s/%s", path, dirname[i]);
            per_R = 1;  // 第一次递归
            getdir_l(pathname);
        }
    }
    closedir(pdir);
}
void getdir(char* path) {
    DIR* pdir;
    char pathname[1024];
    char* filename[SIZE] = {};
    char* dirname[SIZE] = {};
    int cnt = 0;  // 总文件名数量
    int num = 0;  // 目录文件数量
    struct stat info;
    struct dirent* pdirent = NULL;
    // 非目录返回上级函数
    pdir = opendir(path);
    if (pdir == NULL)
        return;
    // 读取目录下文件->filename[cnt]->pathname
    while ((pdirent = readdir(pdir)) != NULL) {
        filename[cnt] =
            (char*)malloc(sizeof(char) *
                          (strlen(pdirent->d_name) + 1));  // copy文件名结尾'\0'
        strcpy(filename[cnt], pdirent->d_name);
        filename[cnt][strlen(pdirent->d_name)] = '\0';
        sprintf(pathname, "%s/%s", path, filename[cnt]);
        cnt++;

        if (stat(pathname, &info) != 0) {  // 调用失败
            perror("stat2");
            exit(0);
        }
        // 提取下级目录文件->dirname[num]
        else {
            if (info.st_mode & S_IFDIR) {
                dirname[num] = (char*)malloc(
                    sizeof(char) *
                    (strlen(pdirent->d_name) + 1));  // copy目录文件名结尾'\0'
                strcpy(dirname[num], pdirent->d_name);
                dirname[num++][strlen(pdirent->d_name)] = '\0';
            }
        }
    }
    // 排序
    if (ls_r == 1) {
        rsort(filename, cnt);
        rsort(dirname, num);
    } else if (ls_t == 1) {
        tsort(filename, cnt);
        tsort(dirname, num);
    } else {
        sort(filename, cnt);
        sort(dirname, num);
    }

    // 总输出

    if (ls_R == 1) {
        if (per_R == 0)
            printf("%s:\n", path);
        else {
            printf("\n\n%s:\n", path);
        }
    }
    // 目录文件
    if (ls_a == 1) {  //-a
        printf("\033[34m.  ");
        printf("..  ");
    }
    for (int i = 0; i < cnt; i++) {
        sprintf(pathname, "%s/%s", path, filename[i]);
        if (filename[i][0] == '.' || !strcmp(filename[i], ".."))
            continue;
        if (ls_i == 1) {
            show_inode(pathname);
        }
        if (ls_s == 1) {
            show_block(pathname);
        }
        if (ls_l == 1) {
            show_info(path, filename[i]);
        }
        printf("\033[%d;1m%s  ", color(pathname), filename[i]);
    }

    // 递归
    if (ls_R == 1) {
        // 遍历目录
        for (int i = 0; i < num; i++) {
            if (!strcmp(dirname[i], ".") || !strcmp(dirname[i], ".."))
                continue;
            sprintf(pathname, "%s/%s", path, dirname[i]);
            per_R = 1;  // 第一次递归
            getdir(pathname);
        }
    }
    closedir(pdir);
}
void getfile(char* path) {
    if (ls_i == 1) {
        show_inode(path);
    }
    if (ls_s == 1) {
        show_block(path);
    }
    if (ls_l == 1) {
        show_info(path, path);
    } else {
        printf("\033[%d;1m%s  ", color(path), path);
    }
    per_r = 1;  // 切换
}
int color(char* path) {
    struct stat st;
    stat(path, &st);
    if (S_ISDIR(st.st_mode)) {
        return 34;
    } else if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
        return 32;
    }
    return 37;
}

// void upper(char** a,char** b,int n){
//     for(int i=0;i<n;i++){
//         int len=strlen(a[i]);
//         b[i]=(char*)malloc(sizeof(char)*(len+1));
//         strcpy(b[i],a[i]);
//         b[len]='\0';
//         for(int j=0;j<len;j++){
//             if(b[i][j]>='a'&&b[i][j]<='z')b[i][j]-=32;
//         }
//     }
// }
void rsort(char** a, int n) {
    char temp[201];
    // char* b[n+1];
    // upper(a,b,n);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (strcmp(a[i], a[j]) < 0) {
                strcpy(temp, a[i]);
                strcpy(a[i], a[j]);
                strcpy(a[j], temp);
            }
        }
    }
}
void show_inode(char* pathname) {
    struct stat info;
    stat(pathname, &info);
    printf("%d ", info.st_ino);
}
void show_info(char* path, char* filename) {
    struct stat info;
    if (stat(path, &info) == 0) {
        char modestr[11];
        mode_to_letters(info.st_mode, modestr);
        printf("%s", modestr);
        printf("%4d ", (int)info.st_nlink);
        printf("%-8s ", uid_to_name(info.st_uid));
        printf("%-8s ", gid_to_name(info.st_gid));
        printf("%8ld ", (long)info.st_size);
        printf("%.12s ", 4 + ctime(&info.st_mtime));
        printf("\033[%d;1m%s\033[0m\n", color(path), filename);
    }
}
void tsort(char** a, int n) {
    char temp[201];
    // char* b[n+1];
    // upper(a,b,n);
    struct stat info1;
    struct stat info2;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            stat(a[i], &info1);
            stat(a[j ], &info2);
            if (info1.st_mtim.tv_sec < info2.st_mtim.tv_sec) {
                strcpy(temp, a[i]);
                strcpy(a[i], a[j]);
                strcpy(a[j], temp);
            }
        }
    }
}
void show_block(char* pathname) {
    struct stat info;
    stat(pathname, &info);
    printf("%d ", info.st_blocks / 2);
}