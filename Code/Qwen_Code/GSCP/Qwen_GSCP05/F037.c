// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *data;
    int size;
} Buffer;

Buffer *bufs = NULL;
int bc = 0;

int find_buf(const char *name) {
    for (int i = 0; i < bc; i++) {
        if (strcmp(bufs[i].name, name) == 0) return i;
    }
    return -1;
}

int hex_to_byte(const char *s, unsigned char *out) {
    unsigned int val;
    if (sscanf(s, "%2x", &val) != 1) return -1;
    *out = (unsigned char)val;
    return 0;
}

int parse_hex(const char *hex, unsigned char **out, int *out_len) {
    int len = (int)strlen(hex);
    if (len % 2 != 0) return -1;
    int byte_count = len / 2;
    unsigned char *data = malloc(byte_count);
    if (!data && byte_count > 0) return -1;
    for (int i = 0; i < byte_count; i++) {
        if (hex_to_byte(hex + i * 2, &data[i]) != 0) {
            free(data);
            return -1;
        }
    }
    *out = data;
    *out_len = byte_count;
    return 0;
}

int main(void) {
    char line[16384];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[8192];
        a1[0] = '\0';
        a2[0] = '\0';
        int n = sscanf(line, "%31s %255s %8191[^\n]", cmd, a1, a2);
        if (n < 1) continue;

        if (strcmp(cmd, "NEW") == 0 && n >= 3) {
            unsigned char *data;
            int dlen;
            if (parse_hex(a2, &data, &dlen) != 0) continue;
            bufs = realloc(bufs, (bc + 1) * sizeof(Buffer));
            if (!bufs) {
                free(data);
                return 1;
            }
            bufs[bc].name = strdup(a1);
            bufs[bc].data = data;
            bufs[bc].size = dlen;
            bc++;
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 3) {
            int bi = find_buf(a1);
            if (bi >= 0) {
                unsigned char *newdata;
                int nlen;
                if (parse_hex(a2, &newdata, &nlen) != 0) continue;
                bufs[bi].data = realloc(bufs[bi].data, bufs[bi].size + nlen);
                if (!bufs[bi].data) {
                    free(newdata);
                    return 1;
                }
                memcpy(bufs[bi].data + bufs[bi].size, newdata, nlen);
                bufs[bi].size += nlen;
                free(newdata);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            int offset, length;
            n = sscanf(line, "%31s %255s %d %d", cmd, a1, &offset, &length);
            if (n >= 4) {
                int bi = find_buf(a1);
                if (bi >= 0) {
                    if (offset < 0 || offset >= bufs[bi].size) continue;
                    if (length < 0) continue;
                    int end = offset + length;
                    if (end > bufs[bi].size) end = bufs[bi].size;
                    for (int i = offset; i < end; i++) {
                        printf("%02x", bufs[bi].data[i]);
                    }
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int bi = find_buf(a1);
            if (bi >= 0) {
                free(bufs[bi].name);
                free(bufs[bi].data);
                for (int k = bi; k < bc - 1; k++) {
                    bufs[k] = bufs[k + 1];
                }
                bc--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bc; i++) {
                printf("%s %d\n", bufs[i].name, bufs[i].size);
            }
        }
    }
    for (int i = 0; i < bc; i++) {
        free(bufs[i].name);
        free(bufs[i].data);
    }
    free(bufs);
    return 0;
}