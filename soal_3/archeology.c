#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

static const char *relics_path = "./relics";

static int relicfs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        char *full_path;
        asprintf(&full_path, "%s%s", relics_path, path);
        DIR *dp = opendir(full_path);
        if (dp) {
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = 0;
            struct dirent *de;
            while ((de = readdir(dp)) != NULL) {
                if (strstr(de->d_name, ".000") != NULL) {
                    struct stat st;
                    char *part_path;
                    asprintf(&part_path, "%s/%s", full_path, de->d_name);
                    stat(part_path, &st);
                    stbuf->st_size += st.st_size;
                    free(part_path);
                }
            }
            closedir(dp);
        } else {
            res = -ENOENT;
        }
        free(full_path);
    }
    return res;
}

static int relicfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    DIR *dp;
    struct dirent *de;
    dp = opendir(relics_path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        if (strstr(de->d_name, ".000") != NULL) {
            char *relic_name = strndup(de->d_name, strlen(de->d_name) - 4);
            filler(buf, relic_name, NULL, 0);
            free(relic_name);
        }
    }
    closedir(dp);
    return 0;
}

static int relicfs_open(const char *path, struct fuse_file_info *fi)
{
    return 0;
}

static int relicfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    (void) fi;
    char *full_path;
    asprintf(&full_path, "%s%s", relics_path, path);
    DIR *dp = opendir(full_path);
    if (!dp)
        return -ENOENT;

    size_t total_size = 0;
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        if (strstr(de->d_name, ".000") != NULL) {
            char *part_path;
            asprintf(&part_path, "%s/%s", full_path, de->d_name);
            FILE *file = fopen(part_path, "rb");
            if (!file) {
                closedir(dp);
                free(part_path);
                free(full_path);
                return -ENOENT;
            }
            fseek(file, 0, SEEK_END);
            size_t part_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            char *part_buf = malloc(part_size);
            fread(part_buf, 1, part_size, file);
            fclose(file);

            if (offset < total_size + part_size) {
                size_t copy_size = part_size;
                if (offset > total_size)
                    copy_size -= (offset - total_size);
                if (copy_size > size)
                    copy_size = size;
                memcpy(buf, part_buf + (offset > total_size ? offset - total_size : 0), copy_size);
                size -= copy_size;
                buf += copy_size;
                offset += copy_size;
            }

            total_size += part_size;
            free(part_buf);
            free(part_path);
        }
    }
    closedir(dp);
    free(full_path);

    return total_size;
}

static struct fuse_operations relicfs_oper = {
    .getattr = relicfs_getattr,
    .readdir = relicfs_readdir,
    .open = relicfs_open,
    .read = relicfs_read,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &relicfs_oper, NULL);
}
