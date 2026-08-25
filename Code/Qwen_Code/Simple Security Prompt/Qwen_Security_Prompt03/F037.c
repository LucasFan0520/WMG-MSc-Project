// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char *name; unsigned char *data; int len; } Buf;
Buf *bufs = NULL;
int nb = 0;

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int parse_hex(const char *s, unsigned char **out) {
    int l = strlen(s);
    if (l % 2 != 0) return -1;
    *out = malloc(l / 2);
    for (int i = 0; i < l; i += 2) {
        int h = hex_val(s[i]), l_val = hex_val(s[i+1]);
        if (h == -1 || l_val == -1) { free(*out); return -1; }
        (*out)[i/2] = (h << 4) | l_val;
    }
    return l / 2;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "NEW") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            unsigned char *data;
            int len = parse_hex(t, &data);
            if (len >= 0) {
                bufs = realloc(bufs, (nb + 1) * sizeof(Buf));
                bufs[nb].name = strdup(a);
                bufs[nb].data = data;
                bufs[nb].len = len;
                nb++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            unsigned char *data;
            int len = parse_hex(t, &data);
            if (len >= 0) {
                for (int i = 0; i < nb; i++) {
                    if (strcmp(bufs[i].name, a) == 0) {
                        bufs[i].data = realloc(bufs[i].data, bufs[i].len + len);
                        memcpy(bufs[i].data + bufs[i].len, data, len);
                        bufs[i].len += len;
                        break;
                    }
                }
                free(data);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            int off, slen;
            sscanf(line, "%*s %s %d %d", a, &off, &slen);
            for (int i = 0; i < nb; i++) {
                if (strcmp(bufs[i].name, a) == 0) {
                    if (off >= 0 && off + slen <= bufs[i].len) {
                        for (int j = 0; j < slen; j++) {
                            printf("%02x", bufs[i].data[off + j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nb; i++) {
                if (strcmp(bufs[i].name, a) == 0) {
                    free(bufs[i].name); free(bufs[i].data);
                    for (int j = i; j < nb - 1; j++) bufs[j] = bufs[j + 1];
                    nb--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < nb; i++) {
                printf("%s %d\n", bufs[i].name, bufs[i].len);
            }
        }
    }
    for (int i = 0; i < nb; i++) { free(bufs[i].name); free(bufs[i].data); }
    free(bufs);
    return 0;
}