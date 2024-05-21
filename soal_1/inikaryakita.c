#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <wand/MagickWand.h>

static const char *galleryPath = "/home/gallant/portofolio/gallery";
static const char *bahayaPath = "/home/gallant/portofolio/bahaya";

//fungsi menambahkan watermark
void add_wm(const char *src, const char *dest)
{
    MagickWandGenesis();
    MagickWand *wand = NewMagickWand();
    MagickReadImage(wand, src);

    DrawingWand *draw = NewDrawingWand();
    PixelWand *pixel = NewPixelWand();
    PixelSetColor(pixel, "white");
    DrawSetFillColor(draw, pixel);
    DrawSetFontSize(draw, 36);
    DrawAnnotation(draw, 10, MagickGetImageHeight(wand) - 10, (const unsigned char *)"inikaryakita.id");
    MagickDrawImage(wand, draw);
    MagickWriteImage(wand, dest);
    DestroyMagickWand(wand);
    DestroyDrawingWand(draw);
    DestroyPixelWand(pixel);
    MagickWandTerminus();
}

//fungsi untuk reverse text pada file
char *rev_text(const char *text)
{
    int len = strlen(text);
    char *reversed = malloc(len + 1);
    for (int i = 0; i < len; ++i) {
        reversed[i] = text[len - i - 1];
    }
    reversed[len] = '\0';
    return reversed;
}

//melakukan rename implementasi pada FUSE
static int inikaryakita_rename(const char *oldpath, const char *newpath)
{
    if (strstr(newpath, "wm.") != NULL) {
        char dest[512];
        snprintf(dest, sizeof(dest), "%s/%s", galleryPath, strrchr(newpath, '/') + 1);
        add_wm(oldpath, dest);
        remove(oldpath);
        return 0;
    } else if (strstr(newpath, "test") != NULL) {
        FILE *file = fopen(oldpath, "r");
        if (!file) return -errno;

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *content = malloc(fileSize + 1);
        fread(content, 1, fileSize, file);
        fclose(file);

        content[fileSize] = '\0';
        char *reversed = rev_text(content);

        file = fopen(newpath, "w");
        if (!file) {
            free(content);
            free(reversed);
            return -errno;
        }
        fwrite(reversed, 1, fileSize, file);
        fclose(file);
        free(content);
        free(reversed);
        return 0;
    }
    return rename(oldpath, newpath);
}

//mengubah permission untuk FUSE
static int inikaryakita_chmod(const char *path, mode_t mode)
{
    if (strstr(path, "bahaya/script.sh") != NULL) {
        if (chmod(path, mode) == -1) {
            return -errno;
        }
        return 0;
    }
    return -EPERM;
}

static struct fuse_operations ikk_operations =
{
    .rename = inikaryakita_rename,
    .chmod = inikaryakita_chmod,
};
int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &ikk_operations, NULL);
}
