//
// jlu操作系统实验4
// 散列结构文件
// yusy 21221116
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define KEYOFFSET 0
#define KEYLEN sizeof(int)
#define FILENAME "jing.hash"

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "HashFile.h"
#include "jtRecord.h"

void showHashFile();

int main() {
    struct jtRecord rec[6] = {
            {1, "jing"}, {2, "wang"}, {3, "li"}, {4, "zhang"}, {5, "qing"}, {6, "yuan"}
    };

    for (int j = 0; j < 6; j++) {
        printf("<%d, %d>\t", rec[j].key, hash(KEYOFFSET, KEYLEN, &rec[j], 6));
    }

    int fd = hashfile_creat(FILENAME, O_RDWR | O_CREAT, RECORDLEN, 6);

    printf("\nOpen ans Save Record...\n");
    fd = hashfile_open(FILENAME, O_RDWR, 0);
    for (int i = 0; i < 6; i++) {
        hashfile_saverec(fd, KEYOFFSET, KEYLEN, &rec[i]);
    }
    hashfile_close(fd);
    showHashFile();

    // find
    printf("\nFind Record...");
    fd = hashfile_open(FILENAME, O_RDWR, 0);
    int offset = hashfile_findrec(fd, KEYOFFSET, KEYLEN, &rec[4]);
    printf("\noffset is %d\n", offset);
    hashfile_close(fd);
    struct jtRecord jt;
    struct CFTag tag;
    fd = open(FILENAME, O_RDWR);
    lseek(fd, offset, SEEK_SET);
    read(fd, &tag, sizeof(struct CFTag));
    printf("Tag is <%d, %d>\t", tag.collision, tag.free);
    read(fd, &jt, sizeof(struct jtRecord));
    printf("Record is {%d,%s}\n", jt.key, jt.other);

    // delete
    printf("\nDelete Record...");
    fd = hashfile_open(FILENAME, O_RDWR, 0);
    hashfile_delrec(fd, KEYOFFSET, KEYLEN, &rec[2]);
    hashfile_close(fd);
    showHashFile();

    // read
    fd = hashfile_open(FILENAME, O_RDWR, 0);
    char buf[32];
    memcpy(buf, &rec[1], KEYLEN);
    hashfile_read(fd, KEYOFFSET, KEYLEN, buf);
    printf("\nRead Record is {%d,%s}\n", ((struct jtRecord*)buf)->key, ((struct jtRecord*)buf)->other);
    hashfile_close(fd);

    // write
    printf("\nWrite Record...");
    fd = hashfile_open(FILENAME, O_RDWR, 0);
    hashfile_write(fd, KEYOFFSET, KEYLEN, &rec[3]);
    hashfile_close(fd);
    showHashFile();

    return 0;
}

void showHashFile() {
    int fd = open(FILENAME, O_RDWR);

    printf("\n");

    lseek(fd, sizeof(struct HashFileHeader), SEEK_SET);

    struct jtRecord jt;
    struct CFTag tag;

    while(1) {
        if(read(fd, &tag, sizeof(struct CFTag)) <= 0) {
            break;
        }

        printf("Tag is <%d,%d>\t", tag.collision,tag.free);

        if(read(fd, &jt, sizeof(struct jtRecord)) <= 0) {
            break;
        }

        printf("Record is {%d,%s}\n", jt.key, jt.other);
    }

    close(fd);
}