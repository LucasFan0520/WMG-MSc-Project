// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Rec;
Rec *recs = NULL;
int nr = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        int n;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "BATCH") == 0) {
            sscanf(line, "%*s %d", &n);
            Rec *batch = malloc(n * sizeof(Rec));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = 0;
                char *space = strchr(line, ' ');
                if (!space) { valid = 0; break; }
                *space = 0;
                batch[i].name = strdup(line);
                batch[i].note = strdup(space + 1);
            }
            if (valid) {
                recs = realloc(recs, (nr + n) * sizeof(Rec));
                for (int i = 0; i < n; i++) recs[nr++] = batch[i];
            } else {
                for (int i = 0; i < n; i++) {
                    if (batch[i].name) free(batch[i].name);
                    if (batch[i].note) free(batch[i].note);
                }
            }
            free(batch);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(recs[i].name, a) == 0) {
                    free(recs[i].name);
                    free(recs[i].note);
                    for (int j = i; j < nr - 1; j++) recs[j] = recs[j + 1];
                    nr--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(recs[i].name, a) == 0) {
                    printf("%s %s\n", recs[i].name, recs[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nr; i++) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        }
    }
    for (int i = 0; i < nr; i++) {
        free(recs[i].name);
        free(recs[i].note);
    }
    free(recs);
    return 0;
}