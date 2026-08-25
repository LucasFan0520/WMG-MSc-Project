// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; unsigned char *buf; int len; } Buf;
Buf *bufs; int n_buf;

int find_buf(char *name) {
    for(int i=0; i<n_buf; i++) if(!strcmp(bufs[i].name, name)) return i;
    return -1;
}

int hex2byte(char *h) {
    int val = 0;
    for(int i=0; i<2; i++) {
        val *= 16;
        if(h[i] >= '0' && h[i] <= '9') val += h[i] - '0';
        else if(h[i] >= 'a' && h[i] <= 'f') val += h[i] - 'a' + 10;
        else if(h[i] >= 'A' && h[i] <= 'F') val += h[i] - 'A' + 10;
        else return -1;
    }
    return val;
}

int main() {
    char cmd[20], name[100], hex[2000]; int offset, length;
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "NEW")) {
            scanf("%s %s", name, hex);
            int slen = strlen(hex);
            if(slen % 2 == 0) {
                bufs = realloc(bufs, (n_buf+1)*sizeof(Buf));
                bufs[n_buf].name = strdup(name);
                bufs[n_buf].len = slen / 2;
                bufs[n_buf].buf = malloc(bufs[n_buf].len);
                for(int i=0; i<bufs[n_buf].len; i++) {
                    int b = hex2byte(hex + i*2);
                    if(b == -1) { free(bufs[n_buf].buf); free(bufs[n_buf].name); n_buf--; break; }
                    bufs[n_buf].buf[i] = b;
                }
                n_buf++;
            }
        } else if(!strcmp(cmd, "APPEND")) {
            scanf("%s %s", name, hex);
            int idx = find_buf(name);
            if(idx != -1) {
                int slen = strlen(hex);
                if(slen % 2 == 0) {
                    int add = slen / 2;
                    bufs[idx].buf = realloc(bufs[idx].buf, bufs[idx].len + add);
                    for(int i=0; i<add; i++) {
                        int b = hex2byte(hex + i*2);
                        if(b == -1) break;
                        bufs[idx].buf[bufs[idx].len + i] = b;
                    }
                    bufs[idx].len += add;
                }
            }
        } else if(!strcmp(cmd, "SLICE")) {
            scanf("%s %d %d", name, &offset, &length);
            int idx = find_buf(name);
            if(idx != -1 && offset >= 0 && offset + length <= bufs[idx].len) {
                for(int i=0; i<length; i++) {
                    printf("%02x", bufs[idx].buf[offset + i]);
                }
                printf("\n");
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_buf(name);
            if(idx != -1) {
                free(bufs[idx].name); free(bufs[idx].buf);
                for(int k=idx; k<n_buf-1; k++) bufs[k] = bufs[k+1];
                n_buf--;
            }
        } else if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<n_buf; i++) printf("%s %d\n", bufs[i].name, bufs[i].len);
        }
    }
    return 0;
}