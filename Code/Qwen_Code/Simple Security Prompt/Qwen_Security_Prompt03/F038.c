// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char *name; char *encoded; } Rle;
Rle *rles = NULL;
int nr = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "STORE") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            rles = realloc(rles, (nr + 1) * sizeof(Rle));
            rles[nr].name = strdup(a);
            rles[nr].encoded = strdup(t);
            nr++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(rles[i].name, a) == 0) {
                    char *p = rles[i].encoded;
                    while (*p) {
                        if (!isdigit(*p)) { p++; continue; }
                        long count = strtol(p, &p, 10);
                        if (count > 10000) { printf("INVALID\n"); break; }
                        if (*p) {
                            for (long j = 0; j < count; j++) {
                                printf("%c", *p == ' ' ? '_' : *p);
                            }
                            p++;
                        }
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(rles[i].name, a) == 0) {
                    free(rles[i].name); free(rles[i].encoded);
                    for (int j = i; j < nr - 1; j++) rles[j] = rles[j + 1];
                    nr--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < nr; i++) {
                printf("%s\n", rles[i].name);
            }
        }
    }
    for (int i = 0; i < nr; i++) { free(rles[i].name); free(rles[i].encoded); }
    free(rles);
    return 0;
}