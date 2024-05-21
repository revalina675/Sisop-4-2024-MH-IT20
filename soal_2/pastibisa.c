#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdbool.h>

#define RAHASIA_PASSWORD "your_password_here"

// Deklarasi fungsi-fungsi dekripsi
char* base64_decode(const char* input, size_t len, size_t* out_len);
void rot13(char* str);
char* hex_decode(const char* input, size_t len);
void strrev(char* str);

// Deklarasi fungsi-fungsi utilitas
bool is_encrypted(const char* filename);
char* read_file(const char* path, size_t* size);
int write_file(const char* path, const char* data, size_t size);
int move_file(const char* source, const char* dest_dir);

// Fungsi base64_decode
char* base64_decode(const char* input, size_t len, size_t* out_len) {
    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    *out_len = (len * 3) / 4;
    if (input[len - 1] == '=') (*out_len)--;
    if (input[len - 2] == '=') (*out_len)--;

    char* decoded = malloc(*out_len);
    if (decoded == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0, j = 0; i < len;) {
        int sextet_a = input[i] == '=' ? 0 & i++ : strchr(base64_chars, input[i++]) - base64_chars;
        int sextet_b = input[i] == '=' ? 0 & i++ : strchr(base64_chars, input[i++]) - base64_chars;
        int sextet_c = input[i] == '=' ? 0 & i++ : strchr(base64_chars, input[i++]) - base64_chars;
        int sextet_d = input[i] == '=' ? 0 & i++ : strchr(base64_chars, input[i++]) - base64_chars;

        decoded[j++] = (sextet_a << 2) | (sextet_b >> 4);
        if (i < len) decoded[j++] = ((sextet_b & 15) << 4) | (sextet_c >> 2);
        if (i < len) decoded[j++] = ((sextet_c & 3) << 6) | sextet_d;
    }

    return decoded;
}

// Fungsi rot13
void rot13(char* str) {
    while (*str) {
        if (('a' <= *str && *str <= 'z') || ('A' <= *str && *str <= 'Z')) {
            if ((*str >= 'a' && *str <= 'm') || (*str >= 'A' && *str <= 'M'))
                *str += 13;
            else
                *str -= 13;
        }
        str++;
    }
}

// Fungsi hex_decode
char* hex_decode(const char* input, size_t len) {
    char* decoded = malloc(len / 2);
    if (decoded == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < len; i += 2) {
        sscanf(input + i, "%2hhx", &decoded[i / 2]);
    }

    return decoded;
}

// Fungsi strrev
void strrev(char* str) {
    if (!str || !*str) return;
    char *start = str;
    char *end = start + strlen(str) - 1;
    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
}

// Fungsi untuk memeriksa apakah file merupakan file yang terenkripsi
bool is_encrypted(const char* filename) {
    return strncmp(filename, "base64_", 7) == 0 ||
           strncmp(filename, "rot13_", 6) == 0 ||
           strncmp(filename, "hex_", 4) == 0 ||
           strncmp(filename, "rev_", 4) == 0;
}

// Fungsi untuk membaca isi file
char* read_file(const char* path, size_t* size) {
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(*size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }

    fread(content, 1, *size, file);
    content[*size] = '\0';

    fclose(file);
    return content;
}

// Fungsi untuk menulis data ke file
int write_file(const char* path, const char* data, size_t size) {
    FILE* file = fopen(path, "wb");
    if (!file) return -1;

    fwrite(data, 1, size, file);
    fclose(file);
    return 0;
}

// Fungsi untuk memindahkan file
int move_file(const char* source, const char* dest_dir) {
    char* filename = strdup(source);
    char* base = basename(filename);

    char dest_path[PATH_MAX];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, base);

    int ret = rename(source, dest_path);
    free(filename);
    return ret;
}

// Fungsi untuk dekripsi file
void decrypt_and_move_file(const char* source, const char* dest_dir) {
    char* filename = strdup(source);
    char* base = basename(filename);

    char dest_path[PATH_MAX];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, base);

    size_t content_size;
    char* content = read_file(source, &content_size);
    if (content) {
        char* decoded_content = NULL;
        size_t decoded_size = 0;

        if (strncmp(base, "base64_", 7) == 0) {
            decoded_content = base64_decode(content + 7, content_size - 7, &decoded_size);
        } else if (strncmp(base, "rot13_", 6) == 0) {
            rot13(content + 6);
            decoded_content = strdup(content + 6);
            decoded_size = strlen(decoded_content);
        } else if (strncmp(base, "hex_", 4) == 0) {
            decoded_content = hex_decode(content + 4, content_size - 4);
            decoded_size = (content_size - 4) / 2;
        } else if (strncmp(base, "rev_", 4) == 0) {
            strrev(content + 4);
            decoded_content = strdup(content + 4);
            decoded_size = strlen(decoded_content);
        }

        if (decoded_content) {
            write_file(dest_path, decoded_content, decoded_size);
            free(decoded_content);
        }
        free(content);
    }

    free(filename);
}

// Fungsi untuk mengambil atribut file
static int custom_getattr(const char* path, struct stat* stbuf) {
    int res;
    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

// Fungsi untuk membaca direktori
static int custom_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
    (void) offset;
    (void) fi;

    DIR* dp;
    struct dirent* de;

    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);
        struct stat st;
        if (stat(full_path, &st) == -1)
            continue;

        if (S_ISREG(st.st_mode) && is_encrypted(de->d_name)) {
            decrypt_and_move_file(full_path, path);
            // Hapus file yang sudah dipindahkan
            unlink(full_path);
        } else if (S_ISDIR(st.st_mode) && !is_encrypted(de->d_name)) {
            // Tidak memindahkan folder sensitif
            continue;
        }

        // Memasukkan nama file/folder ke dalam buffer filler
        filler(buf, de->d_name, NULL, 0);
    }

    closedir(dp);
    return 0;
}

// Fungsi untuk membuka file
static int custom_open(const char* path, struct fuse_file_info* fi) {
    int fd;
    fd = open(path, fi->flags);
    if (fd == -1)
        return -errno;
    close(fd);
    return 0;
}

// Fungsi untuk membaca isi file
static int custom_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    (void)fi;
    int fd;
    int res;

    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

// Fungsi untuk menulis data ke file
static int custom_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    (void)fi;
    int fd;
    int res;

    fd = open(path, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

// Struktur operasi FUSE
static struct fuse_operations custom_oper = {
    .getattr    = custom_getattr,
    .readdir    = custom_readdir,
    .open       = custom_open,
    .read       = custom_read,
    .write      = custom_write,
};

// Fungsi main
int main(int argc, char* argv[]) {
    umask(0);
    return fuse_main(argc, argv, &custom_oper, NULL);
}

