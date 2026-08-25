// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

Record *recs = NULL;
int rcount = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            scanf("%d", &n);
            char **names = malloc(sizeof(char*) * n);
            char **notes = malloc(sizeof(char*) * n);
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char line[2000];
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = 0;
                char *space = strchr(line, ' ');
                if (!space) { valid = 0; break; }
                *space = '\0';
                names[i] = strdup(line);
                notes[i] = strdup(space + 1);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    recs = realloc(recs, sizeof(Record) * (rcount + 1));
                    recs[rcount].name = names[i];
                    recs[rcount].note = notes[i];
                    rcount++;
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (names[i]) free(names[i]);
                    if (notes[i]) free(notes[i]);
                }
            }
            free(names);
            free(notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            for (int i = 0; i < rcount; i++) {
                if (strcmp(recs[i].name, name) == 0) {
                    free(recs[i].name);
                    free(recs[i].note);
                    for (int j = i; j < rcount - 1; j++) recs[j] = recs[j+1];
                    rcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[100];
            scanf("%s", name);
            for (int i = 0; i < rcount; i++) {
                if (strcmp(recs[i].name, name) == 0) {
                    printf("%s %s\n", recs[i].name, recs[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(recs[i].name);
        free(recs[i].note);
    }
    free(recs);
    return 0;
}