// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    unsigned char buf[10000];
    int len;
} Buf;

Buf bufs[1000];
int bn = 0;

int findBuf(char *name) {
    for (int i = 0; i < bn; i++) {
        if (!strcmp(bufs[i].name, name)) return i;
    }
    return -1;
}

int hex2byte(char *s) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        val *= 16;
        if (s[i] >= '0' && s[i] <= '9') val += s[i] - '0';
        else if (s[i] >= 'a' && s[i] <= 'f') val += s[i] - 'a' + 10;
        else if (s[i] >= 'A' && s[i] <= 'F') val += s[i] - 'A' + 10;
        else return -1;
    }
    return val;
}

int main() {
    char cmd[20], name[100], hex[20000];
    int offset, length;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "NEW")) {
            scanf("%s %s", name, hex);
            int l = strlen(hex);
            if (l % 2 == 0) {
                strcpy(bufs[bn].name, name);
                bufs[bn].len = 0;
                for (int i = 0; i < l; i += 2) {
                    int b = hex2byte(hex + i);
                    if (b == -1) {
                        bufs[bn].len = 0;
                        break;
                    }
                    bufs[bn].buf[bufs[bn].len++] = b;
                }
                bn++;
            }
        } else if (!strcmp(cmd, "APPEND")) {
            scanf("%s %s", name, hex);
            int i = findBuf(name);
            if (i != -1) {
                int l = strlen(hex);
                if (l % 2 == 0) {
                    for (int j = 0; j < l; j += 2) {
                        int b = hex2byte(hex + j);
                        if (b == -1) break;
                        if (bufs[i].len < 10000) bufs[i].buf[bufs[i].len++] = b;
                    }
                }
            }
        } else if (!strcmp(cmd, "SLICE")) {
            scanf("%s %d %d", name, &offset, &length);
            int i = findBuf(name);
            if (i != -1) {
                for (int j = 0; j < length; j++) {
                    if (offset + j < bufs[i].len) {
                        printf("%02x", bufs[i].buf[offset + j]);
                    }
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int i = findBuf(name);
            if (i != -1) {
                for (int j = i; j < bn - 1; j++) {
                    bufs[j] = bufs[j + 1];
                }
                bn--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < bn; i++) {
                printf("%s %d\n", bufs[i].name, bufs[i].len);
            }
        }
    }
    return 0;
}