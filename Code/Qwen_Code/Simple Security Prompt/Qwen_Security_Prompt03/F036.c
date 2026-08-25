// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *data; } Rec;
Rec *recs = NULL;
int nr = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            char *colon = strchr(t, ':');
            if (colon) {
                int len = atoi(t);
                if (len >= 0 && (int)strlen(colon + 1) == len) {
                    recs = realloc(recs, (nr + 1) * sizeof(Rec));
                    recs[nr].name = strdup(a);
                    recs[nr].data = strdup(colon + 1);
                    nr++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(recs[i].name, a) == 0) {
                    free(recs[i].name); free(recs[i].data);
                    for (int j = i; j < nr - 1; j++) recs[j] = recs[j + 1];
                    nr--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(recs[i].name, a) == 0) {
                    for (char *p = recs[i].data; *p; p++) {
                        printf("%c", *p == ' ' ? '_' : *p);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nr; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    for (int i = 0; i < nr; i++) { free(recs[i].name); free(recs[i].data); }
    free(recs);
    return 0;
}