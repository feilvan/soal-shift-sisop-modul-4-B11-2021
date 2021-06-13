# soal-shift-sisop-modul-4-B11-2021

Anggota kelompok :
* 05111940000181 - Cliffton Delias Perangin Angin
* 05111940000095 - Fuad Elhasan Irfani
* 05111940000107 - Sabrina Lydia Simanjuntak

## Soal 1
```a``` Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.<p/>
```b``` Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.<p/>
```c``` Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.<p/>
```d``` Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]<p/>
```e``` Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya (rekursif)<p/>

### Penyelesaian

Berikut code untuk ecnrypt
```c
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
        if (filePath[index] != '/' && isalpha(filePath[index])) {
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
```

Dan berikut untuk decrypt
```c
void decAtbash(char *path)
{
    if (!strcmp(path, ".") || !strcmp(path, "..")) return;

    printf("Decrypt file with atbash: %s\n", path);
    fflush(stdout);

    int endIndex = splitExtension(path);
    if (endIndex == strlen(path))
        endIndex = getExtension(path);
    int startIndex = getSlashFile(path, endIndex);

    for (int index = startIndex; index < endIndex; index++)
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
```

Didalamnya terdapat fungsi2 di luar encrypt dan decrypt
- splitExtension = memisah index string untuk extension
- getExtension = mengambil index extension
- getSlashFile = mengambil index char '/'

Berikut screenshot saat folder dienkripsi<br/>
![Screenshot 2021-06-13 195356](https://user-images.githubusercontent.com/73324192/121809202-1aaa8980-cc86-11eb-96ee-ff4afa3b2938.png)
<br/>
Isi folder sebenarnya<br/>
![Screenshot 2021-06-13 202837](https://user-images.githubusercontent.com/73324192/121809217-272ee200-cc86-11eb-91f4-e8afd70a582b.png)

### Kendala
Masih kurang paham dengan bagaimana cara kerja fuse jadi kami kesulitan menentukan kapan dan dimana dilakukan encrypt dan decrypt

## Soal 2
### Kendala
Kami kesulitan mengerjakannya karena kami belum terlalu memahami tentang bagaimana fuse bekerja.

## Soal 3
### Kendala
Kami kesulitan mengerjakannya karena kami belum terlalu memahami tentang bagaimana fuse bekerja.

## Soal 4
```a``` Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.<p/>
```b``` Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.<p/>
```c``` Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.<p/>
```d``` Sisanya, akan dicatat pada level INFO.<p/>
```e``` Format untuk logging yaitu: ```[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]```<br/>
Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan<p/>

Berikut code untuk logging
```c
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
```

Dan berikut untuk logging dengan nama file. Digunakan di kondisi seperti "RENAME".
```c
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
```
Keduanya hampir sama. Hanya berbeda saat mencetak "INFO".<p/>

Berikut potongan isi dari file log
![Screenshot 2021-06-13 204649](https://user-images.githubusercontent.com/73324192/121809812-868df180-cc88-11eb-95a6-838b3392db6d.png)

### Kendala
Tidak ada
