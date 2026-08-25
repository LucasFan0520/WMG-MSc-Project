// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STORE 1000
#define MAX_LEN 10000

typedef struct {
    char name[100];
    char encoded[MAX_LEN];
} Store;

Store stores[MAX_STORE];
int scount = 0;

int find_store(const char *name) {
    for (int i = 0; i < scount; i++) if (strcmp(stores[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], encoded[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "STORE") == 0) {
            scanf("%s %s", name, encoded);
            if (find_store(name) == -1 && scount < MAX_STORE) {
                strcpy(stores[scount].name, name);
                strcpy(stores[scount].encoded, encoded);
                scount++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            scanf("%s", name);
            int idx = find_store(name);
            if (idx != -1) {
                char *p = stores[idx].encoded;
                int valid = 1;
                while (*p) {
                    if (!isdigit(*p)) {
                        valid = 0;
                        break;
                    }
                    long count = strtol(p, &p, 10);
                    if (count > 10000 || count <= 0 || !*p) {
                        valid = 0;
                        break;
                    }
                    char c = *p++;
                    for (long i = 0; i < count; i++) {
                        putchar(c == ' ' ? '_' : c);
                    }
                }
                if (!valid) {
                    printf("INVALID\n");
                } else {
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_store(name);
            if (idx != -1) {
                for (int i = idx; i < scount - 1; i++) stores[i] = stores[i + 1];
                scount--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < scount; i++) {
                printf("%s\n", stores[i].name);
            }
        }
    }
    return 0;
}