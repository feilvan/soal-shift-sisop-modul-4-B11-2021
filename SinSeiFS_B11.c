#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ctype.h>

static const char *dirpath = "/home/xa/Downloads";

char *prefix1 = "AtoZ_";
char *prefix2 = "RX_";
char *prefix3 = "A_is_a_";
int x = 0;

int splitExtension(char *filePath)
{
    int isFound = 0;
    for (int index = strlen(filePath) - 1; index >= 0; index--)
        if (filePath[index] == '.') {
            if (isFound == 1) return index;
            else isFound = 1;
        }
    return strlen(filePath);
}

int getExtension(char *filePath)
{
    for (int index = strlen(filePath) - 1; index >= 0; index--)
        if (filePath[index] == '.') return index;
    return strlen(filePath);
}

int getSlashFile(char *filePath, int end)
{
    for (int index = 0; index < strlen(filePath); index++)
        if (filePath[index] == '/') return index + 1;
    return end;
}

void encAtbash(char *filePath)
{
    if (!strcmp(filePath, ".") || !strcmp(filePath, "..")) return;

    printf("Encrypt file with atbash: %s\n", filePath);
    fflush(stdout);

    int endIndex = splitExtension(filePath);
    if (endIndex == strlen(filePath))
        endIndex = getExtension(filePath);
    int startIndex = getSlashFile(filePath, 0);

    for (int index = startIndex; index < endIndex; index++)
    {
        if (filePath[index] != '/' && isalpha(filePath[index]))
        {
            char tmp = filePath[index];
            if (isupper(filePath[index]))
                tmp -= 'A';
            else
                tmp -= 'a';
            tmp = 25 - tmp; //Atbash cipher
            if (isupper(filePath[index]))
                tmp += 'A';
            else
                tmp += 'a';
            filePath[index] = tmp;
        }
    }
}

void decAtbash(char *path)
{
    if (!strcmp(path, ".") || !strcmp(path, "..")) return;

    printf("Decrypt file with atbash: %s\n", path);
    fflush(stdout);

    int endIndex = splitExtension(path);
    if (endIndex == strlen(path))
        endIndex = getExtension(path);
    int startIndex = getSlashFile(path, endIndex);

    for (int index = startIndex; index < endIndex; index++) {
        if (path[index] != '/' && isalpha(path[index])) {
            char tmp = path[index];
            if (isupper(path[index]))
                tmp -= 'A';
            else
                tmp -= 'a';
            tmp = 25 - tmp;
            if (isupper(path[index]))
                tmp += 'A';
            else
                tmp += 'a';
            path[index] = tmp;
        }
    }
}

void setLog(char *logCategory, char *fpath)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    FILE *file;
    file = fopen("/home/xa/SinSeiFS.log", "a");
    char tmp[1000];

    if (strcmp(logCategory, "RMDIR") == 0 || strcmp(logCategory, "UNLINK") == 0)
        sprintf(tmp, "WARNING::%.2d%.2d%d-%.2d:%.2d:%.2d:%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, logCategory, fpath);
    else
        sprintf(tmp, "INFO::%.2d%.2d%d-%.2d:%.2d:%.2d:%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, logCategory, fpath);

    fputs(tmp, file);
    fclose(file);
    return;
}

void setLogWithNameFiles(char *logCategory, const char *old, const char *new)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    FILE *file;
    file = fopen("/home/xa/SinSeiFS.log", "a");
    char tmp[1000];

    if (strcmp(logCategory, "RMDIR") == 0 || strcmp(logCategory, "UNLINK") == 0)
        sprintf(tmp, "WARNING::%.2d%.2d%d-%.2d:%.2d:%.2d:%s::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, logCategory, old, new);
    else
        sprintf(tmp, "INFO::%.2d%.2d%d-%.2d:%.2d:%.2d:%s::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, logCategory, old, new);

    fputs(tmp, file);
    fclose(file);
    return;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    char *a = strstr(path, prefix1);
    if (a != NULL) decAtbash(a);

    char *b = strstr(path, prefix2);
    if (b != NULL) decAtbash(b);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else sprintf(fpath, "%s%s", dirpath, path);

    int res = lstat(fpath, stbuf);
    if (res == -1) return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char *a = strstr(path, prefix1);
    if (a != NULL) decAtbash(a);

    char *b = strstr(path, prefix2);
    if (b != NULL) decAtbash(b);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else sprintf(fpath, "%s%s", dirpath, path);

    if (x != 24) x++;
    else setLog("READDIR", fpath);

    int res = 0;
    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fpath);

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (a != NULL) encAtbash(de->d_name);
        if (b != NULL) encAtbash(de->d_name);

        res = (filler(buf, de->d_name, &st, 0));
        if (res != 0) break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char *a = strstr(path, prefix1);
    if (a != NULL) decAtbash(a);

    char *b = strstr(path, prefix2);
    if (b != NULL) decAtbash(b);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else sprintf(fpath, "%s%s", dirpath, path);

    (void)fi;
    setLog("READ", fpath);

    int fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;

    close(fd);
    return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    char *a = strstr(path, prefix1);
    if (a != NULL) decAtbash(a);

    char *b = strstr(path, prefix2);
    if (b != NULL) decAtbash(b);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else sprintf(fpath, "%s%s", dirpath, path);

    int res = mkdir(fpath, mode);
    setLog("MKDIR", fpath);

    if (res == -1) return -errno;
    return 0;
}

static int xmp_rmdir(const char *path)
{
    char *a = strstr(path, prefix1);
    if (a != NULL) decAtbash(a);

    char *b = strstr(path, prefix2);
    if (b != NULL) decAtbash(b);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else sprintf(fpath, "%s%s", dirpath, path);

    int res = rmdir(fpath);
    setLog("RMDIR", fpath);

    if (res == -1) return -errno;
    return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    char *a = strstr(to, prefix1);
    if (a != NULL) decAtbash(a);

    char *b = strstr(from, prefix2);
    if (b != NULL) decAtbash(b);

    char *c = strstr(to, prefix2);
    if (c != NULL) decAtbash(c);

    char oldName[1000], newName[1000];
    sprintf(oldName, "%s%s", dirpath, from);
    sprintf(newName, "%s%s", dirpath, to);

    int res = rename(oldName, newName);
    if (res == -1) return -errno;

    setLogWithNameFiles("RENAME", oldName, newName);

    if (c != NULL) setLogWithNameFiles("ENCRYPT2", from, to);
    if (b != NULL && c == NULL) setLogWithNameFiles("DECRYPT2", from, to);

    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

// gcc -Wall `pkg-config fuse --cflags` 01.c -o 01 `pkg-config fuse --libs`
// ./01 -f dir01
// fusermount -u test
