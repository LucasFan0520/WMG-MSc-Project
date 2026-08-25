// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLE;

RLE rles[1000];
int rc = 0;

int find_name(char *name) {
    for (int i = 0; i < rc; i++) if (strcmp(rles[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "STORE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *enc = rest + strlen(name);
            while (*enc == ' ') enc++;
            if (find_name(name) == -1) {
                rles[rc].name = strdup(name);
                rles[rc].encoded = strdup(enc);
                rc++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                char *p = rles[idx].encoded;
                while (*p) {
                    if (!isdigit(*p)) break;
                    int count = 0;
                    while (isdigit(*p)) {
                        count = count * 10 + (*p - '0');
                        p++;
                        if (count > 1000000) {
                            count = -1;
                            break;
                        }
                    }
                    if (count < 0 || !*p) break;
                    char c = *p++;
                    for (int i = 0; i < count; i++) {
                        if (c == ' ') putchar('_');
                        else putchar(c);
                    }
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                free(rles[idx].name);
                free(rles[idx].encoded);
                for (int i = idx; i < rc - 1; i++) rles[i] = rles[i+1];
                rc--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s\n", rles[i].name);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(rles[i].name);
        free(rles[i].encoded);
    }
    return 0;
}