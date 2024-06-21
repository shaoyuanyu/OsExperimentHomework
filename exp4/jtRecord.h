//
// jlu操作系统实验4
// 散列结构文件
// yusy 21221116
//

#ifndef OSEXPERIMENTHOMEWORK_JTRECORD_H
#define OSEXPERIMENTHOMEWORK_JTRECORD_H

#define RECORDLEN 32

struct jtRecord {
    int key;
    char other[RECORDLEN - sizeof(int)];
};

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#endif //OSEXPERIMENTHOMEWORK_JTRECORD_H
