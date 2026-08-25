// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char *name; char *encoded; } RLE;
RLE rles[1000]; int nrles = 0;

int find_rle(char *name) {
    for (int i = 0; i < nrles; i++) if (!strcmp(rles[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], encoded[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "STORE")) {
            scanf("%s", name);
            fgets(encoded, sizeof(encoded), stdin);
            encoded[strcspn(encoded, "\n")] = 0;
            if (encoded[0] == ' ') memmove(encoded, encoded + 1, strlen(encoded));
            rles[nrles].name = strdup(name);
            rles[nrles++].encoded = strdup(encoded);
        } else if (!strcmp(cmd, "DECODE")) {
            scanf("%s", name);
            int idx = find_rle(name);
            if (idx != -1) {
                char *s = rles[idx].encoded;
                int valid = 1;
                while (*s) {
                    if (!isdigit(*s)) { valid = 0; break; }
                    long count = 0;
                    while (isdigit(*s)) {
                        count = count * 10 + (*s - '0');
                        if (count > 1000000) { valid = 0; break; }
                        s++;
                    }
                    if (!valid || !*s) break;
                    char c = *s++;
                    for (long i = 0; i < count; i++) {
                        printf("%c", c == ' ' ? '_' : c);
                    }
                }
                if (valid) printf("\n");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_rle(name);
            if (idx != -1) {
                free(rles[idx].name); free(rles[idx].encoded);
                for (int i = idx; i < nrles - 1; i++) rles[i] = rles[i + 1];
                nrles--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < nrles; i++) {
                printf("%s\n", rles[i].name);
            }
        }
    }
    for (int i = 0; i < nrles; i++) { free(rles[i].name); free(rles[i].encoded); }
    return 0;
}