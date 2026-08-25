// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

Record *recs = NULL;
int rc = 0;

int find_rec(const char *name) {
    for (int i = 0; i < rc; i++) {
        if (strcmp(recs[i].name, name) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char rest[2048];
        a1[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %2047[^\n]", cmd, a1, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "BATCH") == 0) {
            int batch_n = atoi(a1);
            if (batch_n <= 0) continue;
            char **names = malloc(batch_n * sizeof(char *));
            char **notes = malloc(batch_n * sizeof(char *));
            if (!names || !notes) {
                free(names);
                free(notes);
                return 1;
            }
            int valid = 1;
            for (int i = 0; i < batch_n; i++) {
                if (!fgets(line, sizeof(line), stdin)) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = '\0';
                char bname[256];
                char bnote[2048];
                bname[0] = '\0';
                bnote[0] = '\0';
                int bn = sscanf(line, "%255s %2047[^\n]", bname, bnote);
                if (bn < 2 || bname[0] == '\0') {
                    valid = 0;
                }
                if (valid) {
                    names[i] = strdup(bname);
                    notes[i] = strdup(bnote);
                }
            }
            if (valid) {
                for (int i = 0; i < batch_n; i++) {
                    recs = realloc(recs, (rc + 1) * sizeof(Record));
                    if (!recs) return 1;
                    recs[rc].name = names[i];
                    recs[rc].note = notes[i];
                    rc++;
                }
            } else {
                for (int i = 0; i < batch_n; i++) {
                    if (names[i]) free(names[i]);
                    if (notes[i]) free(notes[i]);
                }
            }
            free(names);
            free(notes);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int ri = find_rec(a1);
            if (ri >= 0) {
                free(recs[ri].name);
                free(recs[ri].note);
                for (int k = ri; k < rc - 1; k++) {
                    recs[k] = recs[k + 1];
                }
                rc--;
            }
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            int ri = find_rec(a1);
            if (ri >= 0) {
                printf("%s %s\n", recs[ri].name, recs[ri].note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(recs[i].name);
        free(recs[i].note);
    }
    free(recs);
    return 0;
}