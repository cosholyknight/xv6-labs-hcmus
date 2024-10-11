#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define MAX_SIZE 512

// like fmtname function in ls.c but remove the whitespace padding
char * fmtname(char* path)
{
    char *p;
    for(p=path + strlen(path); p >= path && *p != '/'; p--);
    p++;
    return p;
}

void find(char* path, char* file)
{
    char buffer[MAX_SIZE], *p;
    int fd;
    struct dirent de;
    struct stat st;
    fd = open(path, O_RDONLY);
    // open file failed

    if(fd < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        exit(-1);
    }

    // stat failed
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        exit(-1);
    }

    switch(st.type) {
        case T_DEVICE:
        case T_FILE:
            // print file path
            if(strcmp(fmtname(path), fmtname(file)) == 0)
            fprintf(1, "%s\n", path);
            break;
        case T_DIR:
            // path length bigger than MAX_SIZE
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buffer)) {
                fprintf(2, "find: path too long\n");
                break;
            }
            strcpy(buffer, path);
            p = buffer + strlen(buffer);
            *p++ = '/';
            // Recursion
            // check read failed
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                // invalid or empty
                if(de.inum == 0) continue;

                // check not recurse into "." and ".."
                if(strcmp(de.name, ".") && strcmp(de.name, "..")) {
                    memmove(p, de.name, DIRSIZ);
                    p[DIRSIZ] = 0;
                    find(buffer, file);
                }
            }
            break;
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: find <path> file\n");
        exit(-1);
    }
    if (argc < 3) find(".", argv[1]);
    else for(int i = 1; i < argc - 1; i++) find(argv[i], argv[argc - 1]);
}