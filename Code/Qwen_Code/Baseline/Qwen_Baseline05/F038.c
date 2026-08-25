// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_ENC 10000
#define MAX_DEC 100000

typedef struct {
    char name[MAX_NAME];
    char encoded[MAX_ENC];
} RLE;

RLE rles[100];
int rle_count = 0;

int find_rle(const char *name) {
    for (int i = 0; i < rle_count; i++) if (strcmp(rles[i].name, name) == 0) return i;
    return -1;
}

void print_underscores(const char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] == ' ') putchar('_');
        else putchar(s[i]);
    }
    putchar('\n');
}

int main() {
    char cmd[20], name[MAX_NAME], encoded[MAX_ENC];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "STORE") == 0) {
            scanf("%s %s", name, encoded);
            strcpy(rles[rle_count].name, name);
            strcpy(rles[rle_count].encoded, encoded);
            rle_count++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            scanf("%s", name);
            int idx = find_rle(name);
            if (idx != -1) {
                char *s = rles[idx].encoded;
                char out[MAX_DEC];
                int out_len = 0;
                int valid = 1;
                int i = 0;
                while (s[i]) {
                    if (!isdigit(s[i])) { valid = 0; break; }
                    long count = 0;
                    while (isdigit(s[i])) {
                        count = count * 10 + (s[i] - '0');
                        if (count > 100000) { valid = 0; break; }
                        i++;
                    }
                    if (!valid || !s[i]) { valid = 0; break; }
                    char c = s[i++];
                    if (out_len + count >= MAX_DEC) { valid = 0; break; }
                    for (long j = 0; j < count; j++) out[out_len++] = c;
                }
                if (valid) {
                    out[out_len] = '\0';
                    print_underscores(out);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_rle(name);
            if (idx != -1) {
                for (int i = idx; i < rle_count - 1; i++) rles[i] = rles[i + 1];
                rle_count--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rle_count; i++) printf("%s\n", rles[i].name);
        }
    }
    return 0;
}