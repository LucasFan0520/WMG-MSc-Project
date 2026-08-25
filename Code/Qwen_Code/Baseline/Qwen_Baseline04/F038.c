// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLE;

RLE *rles = NULL;
int rcount = 0;

int find_rle(const char *name) {
    for (int i = 0; i < rcount; i++) {
        if (strcmp(rles[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "STORE") == 0) {
            char name[100], encoded[2000];
            scanf("%s %s", name, encoded);
            if (find_rle(name) == -1) {
                rles = realloc(rles, sizeof(RLE) * (rcount + 1));
                rles[rcount].name = strdup(name);
                rles[rcount].encoded = strdup(encoded);
                rcount++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_rle(name);
            if (idx != -1) {
                char *p = rles[idx].encoded;
                while (*p) {
                    if (!isdigit(*p)) break;
                    int count = 0;
                    while (isdigit(*p)) {
                        count = count * 10 + (*p - '0');
                        p++;
                        if (count > 10000) {
                            count = -1;
                            break;
                        }
                    }
                    if (count == -1 || !*p) break;
                    char c = *p++;
                    for (int i = 0; i < count; i++) {
                        if (c == ' ') putchar('_');
                        else putchar(c);
                    }
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_rle(name);
            if (idx != -1) {
                free(rles[idx].name);
                free(rles[idx].encoded);
                for (int i = idx; i < rcount - 1; i++) rles[i] = rles[i+1];
                rcount--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", rles[i].name);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(rles[i].name);
        free(rles[i].encoded);
    }
    free(rles);
    return 0;
}