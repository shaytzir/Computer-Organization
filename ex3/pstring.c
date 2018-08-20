#include <printf.h>
#include "pstring.h"
#include <stdio.h>

char pstrlen(Pstring* pstr) {
   char length = pstr->len;
    return length;
}

Pstring* replaceChar(Pstring* pstr, char oldChar, char newChar) {
    for (int i = 0; i < pstr->len) {
        if (pstr->str[i] == oldChar) {
            pstr->str[i] = newChar;
        }
    }
    return pstr;
}

Pstring* pstrijcpy(Pstring* dst, Pstring* src, char i, char j){
    if ((i < 0) || (j < 0) || (i > src->len) || (j > src->len) ||
            (i > dst->len) || (j > dst->len) || (j - i < 0)) {
        printf("invalid input!\n");
    }
    int n = j - i + 1;

    for (int l = i; l < i + n; l++) {
        dst->str[l] = src->str[l];
    }
    return dst;
}

Pstring* swapCase(Pstring* pstr) {
    for (int i =0; i < pstr->len; i++) {

    }
}