// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Rec;

Rec recs[10000];
int rc = 0;

int find_rec(char *name) {
    for (int i = 0; i < rc; i++) if (strcmp(recs[i].name, name) == 0) return i;
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
        
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            sscanf(rest, "%d", &n);
            Rec temp[10000];
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = 0;
                char name[1000];
                sscanf(line, "%s", name);
                char *note = line + strlen(name);
                while (*note == ' ') note++;
                if (strlen(name) == 0) {
                    valid = 0;
                } else {
                    temp[i].name = strdup(name);
                    temp[i].note = strdup(note);
                }
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    recs[rc++] = temp[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (temp[i].name) free(temp[i].name);
                    if (temp[i].note) free(temp[i].note);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_rec(rest);
            if (idx != -1) {
                free(recs[idx].name);
                free(recs[idx].note);
                for (int i = idx; i < rc - 1; i++) recs[i] = recs[i+1];
                rc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            int idx = find_rec(rest);
            if (idx != -1) {
                printf("%s %s\n", recs[idx].name, recs[idx].note);
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
    return 0;
}