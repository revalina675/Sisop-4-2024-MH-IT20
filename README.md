# PRAKTIKUM MODUL 04 SISTEM OPERASI
## Kelompok IT20
### Anggota Kelompok :
|             Nama              |     NRP    |
|-------------------------------|------------|
| Revalina Fairuzy Azhari Putri | 5027231001 |
| Farida Qurrotu 'Ayuna         | 5027231015 |
| Gallant Damas Hayuaji         | 5027231037 |

## Soal 1
Adfi merupakan seorang CEO agency creative bernama Ini Karya Kita. Ia sedang melakukan inovasi pada manajemen project photography Ini Karya Kita. Salah satu ide yang dia kembangkan adalah tentang pengelolaan foto project dalam sistem arsip Ini Karya Kita. Dalam membangun sistem ini, Adfi tidak bisa melakukannya sendirian, dia perlu bantuan mahasiswa Departemen Teknologi Informasi angkatan 2023 untuk membahas konsep baru yang akan mengubah project fotografinya lebih menarik untuk dilihat. Adfi telah menyiapkan portofolio hasil project fotonya yang bisa didownload dan diakses di www.inikaryakita.id . Silahkan eksplorasi web Ini Karya Kita dan temukan halaman untuk bisa mendownload projectnya. Setelah kalian download terdapat folder gallery dan bahaya.
- Pada folder “gallery”:
    - Membuat folder dengan prefix "wm." Dalam folder ini, setiap gambar yang dipindahkan ke dalamnya akan diberikan watermark bertuliskan inikaryakita.id. 
			Ex: "mv ikk.jpeg wm-foto/" 
      Output: 
      Before: (tidak ada watermark bertuliskan inikaryakita.id)
      After: (terdapat watermark tulisan inikaryakita.id)
- Pada folder "bahaya," terdapat file bernama "script.sh." Adfi menyadari pentingnya menjaga keamanan dan integritas data dalam folder ini. 
    - Mereka harus mengubah permission pada file "script.sh" agar bisa dijalankan, karena jika dijalankan maka dapat menghapus semua dan isi dari  "gallery"
    - Adfi dan timnya juga ingin menambahkan fitur baru dengan membuat file dengan prefix "test" yang ketika disimpan akan mengalami pembalikan (reverse) isi dari file tersebut.  

## Jawab

Pada soal ini diminta membuat program untuk memberikan watermark pada foto yang disediakan. 

1. Membuat direktori untuk menyimpan pengerjaan `mkdir portofolio`
2. Masuk ke dalam direktori `cd portofolio`
3. Mendownload file yang disediakan di website www.inikaryakita.id
4. Melakukan unzip file yang telah didownload dan mendapatkan 2 file, yaitu `bahaya` dan `gallery`
5. Membuat file `inikaryakita.c` dengan menggunakan `nano inikaryakita.c`

Berikut ini adalah isi dari `inikaryakita.c`

```
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
```

Melakukan compile file
`gcc -o inikaryakita inikaryakita.c $(pkg-config --cflags --libs fuse MagickWand)`

Mengecek hasil compile dengan command `ls`

Program mengalami kesalahan
![333819454-2bb57597-57ae-4833-b96c-76be4dfe7631](https://github.com/revalina675/Sisop-4-2024-MH-IT20/assets/149811596/25f6b15e-36b5-408c-9039-00169d213c3b)


## Soal 2

Masih dengan Ini Karya Kita, sang CEO ingin melakukan tes keamanan pada folder sensitif Ini Karya Kita. Karena Teknologi Informasi merupakan departemen dengan salah satu fokus di Cyber Security, maka dia kembali meminta bantuan mahasiswa Teknologi Informasi angkatan 2023 untuk menguji dan mengatur keamanan pada folder sensitif tersebut. Untuk mendapatkan folder sensitif itu, mahasiswa IT 23 harus kembali mengunjungi website Ini Karya Kita pada www.inikaryakita.id/schedule . Silahkan isi semua formnya, tapi pada form subject isi dengan nama kelompok_SISOP24 , ex: IT01_SISOP24 . Lalu untuk form Masukkan Pesanmu, ketik “Mau Foldernya” . Tunggu hingga 1x24 jam, maka folder sensitif tersebut akan dikirimkan melalui email kalian. Apabila folder tidak dikirimkan ke email kalian, maka hubungi sang CEO untuk meminta bantuan.

- Pada folder "pesan" Adfi ingin meningkatkan kemampuan sistemnya dalam mengelola berkas-berkas teks dengan menggunakan fuse.

      - Jika sebuah file memiliki prefix "base64," maka sistem akan langsung mendekode isi file tersebut dengan algoritma Base64.

      - Jika sebuah file memiliki prefix "rot13," maka isi file tersebut akan langsung di-decode dengan algoritma ROT13.

      - Jika sebuah file memiliki prefix "hex," maka isi file tersebut akan langsung di-decode dari representasi heksadesimalnya.

      - Jika sebuah file memiliki prefix "rev," maka isi file tersebut akan langsung di-decode dengan cara membalikkan teksnya.

Contoh:

File yang belum didecode/ dienkripsi rot_13:

![image](https://github.com/Faridaqurr/lapres/assets/150933246/e3b669db-b875-49b9-85de-bddc4c46ab7f)

File yang sudah didecode/ dienkripsi rot_13:

![image](https://github.com/Faridaqurr/lapres/assets/150933246/a8848789-c08e-4c5a-a74b-cb78ec1829c0)


- Pada folder “rahasia-berkas”, Adfi dan timnya memutuskan untuk menerapkan kebijakan khusus. Mereka ingin memastikan bahwa folder dengan prefix "rahasia" tidak dapat diakses tanpa izin khusus.

   - Jika seseorang ingin mengakses folder dan file pada “rahasia”, mereka harus memasukkan sebuah password terlebih dahulu (password bebas)

- Setiap proses yang dilakukan akan tercatat pada logs-fuse.log dengan format :

  [SUCCESS/FAILED]::dd/mm/yyyy-hh:mm:ss::[tag]::[information]

  Ex:

  [SUCCESS]::01/11/2023-10:43:43::[moveFile]::[File moved successfully]

## Jawab

### Pesan

Pada soal ini, diminta untuk mendekrip semua file sesuai tipe nya pada folder sensitif/pesan

      static void rot13(char *str) {
          for (char *c = str; *c; ++c) {
              if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z')) {
                  if ((*c >= 'a' && *c <= 'm') || (*c >= 'A' && *c <= 'M'))
                      *c += 13;
                  else
                      *c -= 13;
              }
          }
      }

`static void rot13(char *str)` untuk deklarasi fungsi dari ROT13

`if ((*c >= 'a' && *c <= 'z')...` untuk memeriksa apakah ada huruf a-z kecil maupun kapital

` if ((*c >= 'a' && *c <= 'm')...` untuk proses penggeseran huruf sesuai rentangnya yang kemudian ditambahkan 13 ke nilai ASCII-nya
      
      static void hex_decode(const char *input, char *output) {
          size_t len = strlen(input) / 2;
          for (size_t i = 0; i < len; i++) {
              sscanf(input + 2 * i, "%2hhx", &output[i]);
          }
          output[len] = '\0';
      }

`static void hex_decode(const char *input, char *output)` untuk deklarasi fungsi heksadecimal

`size_t len = strlen(input) / 2;` untuk menghitung panjang outputnya

`for (size_t i = 0;...` untuk proses perulangan konversi heksadesimal ke biner
      
      static void reverse_string(char *str) {
          int len = strlen(str);
          for (int i = 0; i < len / 2; i++) {
              char temp = str[i];
              str[i] = str[len - i - 1];
              str[len - i - 1] = temp;
          }
      }

`static void reverse_string(char *str)` untuk deklarasi fungsi reverse

`int len = strlen(str);` untuk menghitung panjang stringnya

`for (int i = 0; i < ...` untuk perulangan ketika membalikkan/reverse kata
      
      static void base64_decode(const char *input, char *output) {
          BIO *bio, *b64;
          int input_len = strlen(input);
          int output_len = (input_len * 3) / 4 + 1;
          char *buffer = malloc(output_len);
          if (!buffer) return;
          
          bio = BIO_new_mem_buf(input, input_len);
          b64 = BIO_new(BIO_f_base64());
          bio = BIO_push(b64, bio);
          BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
          int decoded_size = BIO_read(bio, buffer, input_len);
          buffer[decoded_size] = '\0';
          
          BIO_free_all(bio);
          strcpy(output, buffer);
          free(buffer);
      }

`static void base64_decode(const char *input, char *output)` untuk deklarasi fungsi base64

`int input_len = strlen(input);` untuk hitung panjang input

`int output_len = (input_len * 3) / 4 + 1;` untuk hitung panjang output

`char *buffer = malloc(output_len);` untuk alokasi memori buffer-nya

`int decoded_size = BIO_read(bio, buffer, input_len);` untuk membaca dan dekode base64-nya

`BIO_free_all(bio);` untuk membersihkan BIO dan menyalin ke outputnya

      static void decrypt_message(const char *path, char *content) {
          if (strstr(path, "rev") != NULL) {
              reverse_string(content);
          } else if (strstr(path, "hex") != NULL) {
              char *decoded = malloc(strlen(content) / 2 + 1);
              if (decoded) {
                  hex_decode(content, decoded);
                  strcpy(content, decoded);
                  free(decoded);
              }
          } else if (strstr(path, "base64") != NULL) {
              char *decoded = malloc(strlen(content) * 3 / 4 + 1);
              if (decoded) {
                  base64_decode(content, decoded);
                  strcpy(content, decoded);
                  free(decoded);
              }
          } else if (strstr(path, "rot13") != NULL) {
              rot13(content);
          }
          log_message("decryptMessage", path, 1);
      }

`static void decrypt_message(const char *path, char *content)` untuk deklarasi fungsi dekripnya

`if (strstr(path, "rev")...` untuk periksa fungsi reverse

`else if (strstr(path, "hex")...` untuk periksa fungsi heksadesimal

`else if (strstr(path, "base64")...` untuk periksa fungsi base64

`else if (strstr(path, "rot13")...` untuk periksa fungsi rot13

`log_message("decryptMessage", path, 1);` untuk fungsi log massage

      static int xmp_getattr(const char *path, struct stat *stbuf) {
          int res;
          char fpath[PATH_MAX];
      
          snprintf(fpath, sizeof(fpath), "%s%s", SENSITIVE_DIR, path);
          res = lstat(fpath, stbuf);
      
          if (res == -1)
              return -errno;
      
          return 0;
      }

`static int xmp_getattr(const char *path, struct stat *stbuf)` untuk deklarasi fungsi get atribut (informasi dari direktori lain)

`snprintf(fpath, sizeof(fpath), "%s%s", SENSITIVE_DIR, path);` untuk memetakan path yang lengkap agar fuse berjalan dengan benar
      
      static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
          DIR *dp;
          struct dirent *de;
          char fpath[PATH_MAX];
      
          (void) offset;
          (void) fi;
      
          snprintf(fpath, sizeof(fpath), "%s%s", SENSITIVE_DIR, path);
          dp = opendir(fpath);
          if (dp == NULL)
              return -errno;
      
          while ((de = readdir(dp)) != NULL) {
              struct stat st;
              memset(&st, 0, sizeof(st));
              st.st_ino = de->d_ino;
              st.st_mode = de->d_type << 12;
              if (filler(buf, de->d_name, &st, 0))
                  break;
          }
      
          closedir(dp);
          return 0;
      }

` static int xmp_readdir(const char *path, void *buf,...` untuk deklarasi fungsi read direktori

`dp = opendir(fpath);` untuk membuka direktori yang ditunjukkan path

`while ((de = readdir(dp)) != NULL)...` untuk membaca isi direktori keseluruhan

            int main(int argc, char *argv[]) {
                log_file = fopen(LOG_FILE, "a");
                if (!log_file) {
                    perror("Failed to open log file");
                    return 1;
                }
            
                int ret = fuse_main(argc, argv, &xmp_oper, NULL);
                fclose(log_file);
                return ret;
            }

`int main(int argc, char *argv[])` untuk deklarasi fungsi main

`log_file = fopen(LOG_FILE, "a");` untuk membuka file log yang akan diisi oleh fungsi

### Rahasia-berkas

            static int ask_password() {
                char input[100];
                printf("Enter the password to access the secret files: ");
                scanf("%s", input);
                if (strcmp(input, PASSWORD) == 0) {
                    return 1;
                } else {
                    printf("Incorrect password. Access denied.\n");
                    return 0;
                }
            }

`static int ask_password()` untuk deklarasi fungsi ask password yang menampilkan tampilan masukkan password sebelum akses file

`if (strcmp(input, PASSWORD) == 0)` untuk input password yang benar

`else...` untuk input password yang salah

            void split_path(const char *path, char *dir, char *file) {
                const char *last_slash = strrchr(path, '/');
                if (last_slash == NULL) {
                    strcpy(dir, "");
                    strcpy(file, path);
                } else {
                    strncpy(dir, path, last_slash - path + 1);
                    dir[last_slash - path + 1] = '\0';
                    strcpy(file, last_slash + 1);
                }
            }

`void split_path(const char *path, char *dir, char *file)` untuk fungsi split path direktori agar tidak terjadi crash akses

`const char *last_slash = strrchr(path, '/');` untuk mencari tanda "/" akhir

`if (last_slash == NULL)` untuk slash jika tidak di temukan

`else...` untuk slash jika ditemukan

            static int xmp_open(const char *path, struct fuse_file_info *fi) {
                if (strstr(path, RAHASIA_PREFIX) != NULL) {
                    char dir[PATH_MAX];
                    char file[NAME_MAX];
                    split_path(path, dir, file);
                    
                    if (strstr(dir, SENSITIVE_DIR) != NULL) {
                        if (!ask_password()) {
                            return -EACCES;
                        }
                    }
                }
                
                int res;
                char fpath[PATH_MAX];
            
                snprintf(fpath, sizeof(fpath), "%s%s", SENSITIVE_DIR, path);
            
                res = open(fpath, fi->flags);
                if (res == -1)
                    return -errno;
            
                close(res);
                return 0;
            }

`static int xmp_open(const char *path, struct fuse_file_info *fi)` untuk deklarasi fungsi open rahasia-berkas

`if (strstr(path, RAHASIA_PREFIX) != NULL)...` untuk cek apakah file mengandung prefix rahasia

`char dir[PATH_MAX];` untuk memisahkan path

`if (!ask_password())` untuk file yang mengandung prefix akan dimintai password

            static int xmp_chmod(const char *path, mode_t mode) {
                char fpath[PATH_MAX];
                snprintf(fpath, sizeof(fpath), "%s%s", SENSITIVE_DIR, path);
            
                int res = chmod(fpath, mode);
                if (res == -1)
                    return -errno;
            
                return 0;
            }

`static int xmp_chmod(const char *path, mode_t mode)` untuk deklarasi izin akses ke direktorinya (menghindari permission denied)

`char fpath[PATH_MAX];` untuk membuat path lengkap dan benarnya

`int res = chmod(fpath, mode);` untuk mengubah izin akses (mode)

### Log file

            static FILE *log_file;


            static void log_message(const char *tag, const char *info, int success) {
                time_t now = time(NULL);
                struct tm *t = localtime(&now);
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%d/%m/%Y-%H:%M:%S", t);
                fprintf(log_file, "[%s]::%s::[%s]::[%s]\n", success ? "SUCCESS" : "FAILED", timestamp, tag, info);
                fflush(log_file);
            }

## Revisi

- saya mengubah definenya menjadi :

            #define LOG_FILE "/home/farida/sisop4/logs-fuse.log"
            #define SENSITIVE_DIR "/home/farida/sisop4/sensitif"
            #define RAHASIA_PREFIX "rahasia-berkas"
            #define PASSWORD "secret_password"

  hal ini untuk memudahkan mendeteksi folder sumber yang ingin diberikan izin akses dan dekrip isi filenya

- saya menambahkan fungsi split path

              void split_path(const char *path, char *dir, char *file) {
                            const char *last_slash = strrchr(path, '/');
                            if (last_slash == NULL) {
                                strcpy(dir, "");
                                strcpy(file, path);
                            } else {
                                strncpy(dir, path, last_slash - path + 1);
                                dir[last_slash - path + 1] = '\0';
                                strcpy(file, last_slash + 1);
                            }
                  }

     hal ini agar tidak terjadi tabrakan antar path yang sedang dijalankan, sehingga fuse bisa menuju sumber file dan mengelola file dengan baik

  - saya menambahkan fungsi chmod

                static int xmp_chmod(const char *path, mode_t mode) {
                            char fpath[PATH_MAX];
                            snprintf(fpath, sizeof(fpath), "%s%s", SENSITIVE_DIR, path);
                        
                            int res = chmod(fpath, mode);
                            if (res == -1)
                                return -errno;
                        
                            return 0;
                        }

    hal ini agar tidak terjadi permission denied terus menerus ketika ingin mengakses folder, seperti sebelumnya saat akses rahasia-berkas selalu permission denied

## Hasil Dokumentasi

![sisop](https://github.com/Faridaqurr/lapres/assets/150933246/b5c596cc-4b3f-4ed5-ae4e-de4cce356154)

![sisop1](https://github.com/Faridaqurr/lapres/assets/150933246/078010ff-33dd-4961-bf00-47f6c0734317)

![sisop2](https://github.com/Faridaqurr/lapres/assets/150933246/53ec25fc-cfe3-4a0e-82fd-ada7d9593bc0)

NB: mohon maaf untuk hasil dokumentasinya menggunakan kamera hp dikarenakan kali linux saya error sehingga tidak bisa record dari laptop dan saya belum sempat untuk screenshoot hasil pekerjaan saya sebelum linuxnya error


## Soal 3
Seorang arkeolog menemukan sebuah gua yang didalamnya tersimpan banyak relik dari zaman praaksara, sayangnya semua barang yang ada pada gua tersebut memiliki bentuk yang terpecah belah akibat bencana yang tidak diketahui. Sang arkeolog ingin menemukan cara cepat agar ia bisa menggabungkan relik-relik yang terpecah itu, namun karena setiap pecahan relik itu masih memiliki nilai tersendiri, ia memutuskan untuk membuat sebuah file system yang mana saat ia mengakses file system tersebut ia dapat melihat semua relik dalam keadaan utuh, sementara relik yang asli tidak berubah sama sekali.
Ketentuan :

a. Buatlah sebuah direktori dengan ketentuan seperti pada tree berikut
.
├── [nama_bebas]

├── relics

│   ├── relic_1.png.000

│   ├── relic_1.png.001

│   ├── dst dst…

│   └── relic_9.png.010

└── report

b. Direktori [nama_bebas] adalah direktori FUSE dengan direktori asalnya adalah direktori relics. Ketentuan Direktori [nama_bebas] adalah sebagai berikut :

- Ketika dilakukan listing, isi dari direktori [nama_bebas] adalah semua relic dari relics yang telah tergabung.
- Ketika dilakukan copy (dari direktori [nama_bebas] ke tujuan manapun), file yang disalin adalah file dari direktori relics yang sudah tergabung.
- Ketika ada file dibuat, maka pada direktori asal (direktori relics) file tersebut akan dipecah menjadi sejumlah pecahan dengan ukuran maksimum tiap pecahan adalah 10kb
- File yang dipecah akan memiliki nama [namafile].000 dan seterusnya sesuai dengan jumlah pecahannya.
- Ketika dilakukan penghapusan, maka semua pecahannya juga ikut terhapus.

c. Direktori report adalah direktori yang akan dibagikan menggunakan Samba File Server. Setelah kalian berhasil membuat direktori [nama_bebas], jalankan FUSE dan salin semua isi direktori [nama_bebas] pada direktori report.

d. Catatan:
- pada contoh terdapat 20 relic, namun pada zip file hanya akan ada 10 relic
- [nama_bebas] berarti namanya dibebaskan
- pada soal 3c, cukup salin secara manual. File Server hanya untuk membuktikan bahwa semua file pada direktori [nama_bebas] dapat dibuka dengan baik.

## Jawab

Dibuat suatu folder bernama `modul_4`, di dalamnya diinstall dan diunzip link .zip yang disediakan pada soal. File yang telah didownload, lalu diekstrak dan dipindahkan ke dalam suatu folder bernama `relics`

```
$ wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=1BJkaBvGaxqiwPWvXRdYNXzxxmIYQ8FKf' -O relics.zip
$ unzip relics.zip -d /home/revalina/modul_4 
```
Setelah itu, kita bisa menghapus folder.zip yang tertinggal menggunakan command
```
rm -r relics.zip`
```
Lalu kita buat suatu konfigurasi dengan nama `archeology.c`, berikut adalah codenya :
```
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
```
*- relicfs_getattr* : mengambil atribut dari file atau direktori. Ketika path yang diminta adalah root ("/"), fungsi ini mengatur atribut direktori root dengan mode direktori (S_IFDIR) dan izin akses 0755 serta jumlah hardlink sebanyak 2. Jika path bukan root, fungsi ini membangun jalur lengkap (full_path) dengan menggabungkan relics_path dan path, kemudian mencoba membuka direktori tersebut. Jika berhasil dibuka, fungsi ini menginisialisasi atribut file sebagai file reguler (S_IFREG) dengan izin baca 0444 dan menghitung ukuran total dari semua file yang berakhiran ".000" di dalam direktori tersebut. Ukuran total ini ditambahkan ke st_size dari struktur stbuf. Jika direktori tidak dapat dibuka, fungsi ini mengembalikan kode kesalahan -ENOENT.

*- relicfs_readdir* : membaca isi dari direktori. Jika path yang diminta bukan root ("/"), fungsi ini segera mengembalikan -ENOENT. Untuk root, fungsi ini mengisi buffer dengan entri "." dan ".." yang merupakan entri default untuk direktori. Selanjutnya, fungsi ini membuka direktori relics_path dan membaca setiap entri di dalamnya. Untuk setiap file yang berakhiran ".000", fungsi ini menghilangkan ekstensi ".000" dan menambahkan nama file ke buffer. Setelah selesai, direktori ditutup dan fungsi mengembalikan 0 yang menandakan keberhasilan.

*- relicfs_open* : membuka file dan tidak melakukan operasi khusus. Fungsi ini selalu mengembalikan 0, yang menandakan bahwa operasi membuka file selalu berhasil.

*- relicfs_read* : membaca isi file. Fungsi ini membangun full_path dengan menggabungkan relics_path dan path, kemudian mencoba membuka direktori yang sesuai dengan full_path. Jika direktori tidak dapat dibuka, fungsi ini mengembalikan -ENOENT. Jika berhasil dibuka, fungsi ini membaca semua file yang berakhiran ".000" di dalam direktori tersebut. Isi dari setiap file diambil dan digabungkan ke dalam buffer buf, dengan mempertimbangkan offset dan ukuran data yang diminta. Fungsi ini menggabungkan isi file tersebut ke dalam buffer hingga ukuran total data yang dibaca mencukupi atau tidak ada lagi data yang tersisa untuk dibaca. Setelah selesai, fungsi ini mengembalikan total ukuran data yang berhasil dibaca.

*- main* : titik masuk untuk aplikasi FUSE ini. Fungsi ini memanggil fuse_main dengan operasi yang telah didefinisikan (relicfs_oper). Struktur relicfs_oper mendefinisikan operasi filesystem yang diimplementasikan, yang mencakup getattr, readdir, open, dan read. Operasi ini dikaitkan dengan fungsi-fungsi yang telah dijelaskan sebelumnya untuk menangani operasi filesystem yang sesuai.

Jangan lupa untuk membuat folder mount_point dan report sesuai pada soal dengan command `mkdir report` dan `mkdir mount_point`

Setelah itu, file diaktivasi dengan menggunakan
```
gcc -Wall archeology.c -o archeology pkg-config fuse3 --cflags --libs  sudo ./archeology mount_point
```
Pada tahap ini, file yang ada pada folder relics tidak mau terpindah dan terkelompokkan pada folder `mount_point`. Terbukti saat dilakukan `sudo ./archeology mount_point`, saat dibuka pada file explorer, foto-foto tersebut tidak mau terpindah.

![be3b6427-fabe-4585-af31-5cc6e62fc6f8](https://github.com/revalina675/Sisop-4-2024-MH-IT20/assets/150936800/7e8547d3-b467-4093-9449-002fafdba5ed)
