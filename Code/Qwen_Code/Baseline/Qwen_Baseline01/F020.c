// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int nb;
} Rec;

Rec *recs = NULL;
int n = 0;

int find_rec(const char *title) {
    for (int i = 0; i < n; i++)
        if (strcmp(recs[i].title, title) == 0) return i;
    return -1;
}

void load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) continue;
        recs = realloc(recs, sizeof(Rec) * (n + 1));
        recs[n].title = strdup(line);
        recs[n].body = NULL;
        recs[n].nb = 0;
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, "--") == 0) break;
            recs[n].body = realloc(recs[n].body, sizeof(char*) * (recs[n].nb + 1));
            recs[n].body[recs[n].nb++] = strdup(line);
        }
        n++;
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_rec(arg1);
            if (i != -1) {
                printf("%s\n", recs[i].title);
                for (int j = 0; j < recs[i].nb; j++) printf("%s\n", recs[i].body[j]);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_rec(arg1);
            if (i != -1) {
                free(recs[i].title);
                for (int j = 0; j < recs[i].nb; j++) free(recs[i].body[j]);
                free(recs[i].body);
                for (int j = i; j < n - 1; j++) recs[j] = recs[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", arg1);
            recs = realloc(recs, sizeof(Rec) * (n + 1));
            recs[n].title = strdup(arg1);
            recs[n].body = NULL;
            recs[n].nb = 0;
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                recs[n].body = realloc(recs[n].body, sizeof(char*) * (recs[n].nb + 1));
                recs[n].body[recs[n].nb++] = strdup(line);
            }
            n++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < n; i++) {
                printf("%s\n", recs[i].title);
                for (int j = 0; j < recs[i].nb; j++) printf("%s\n", recs[i].body[j]);
                printf("--\n");
            }
        }
    }
    for (int i = 0; i < n; i++) {
        free(recs[i].title);
        for (int j = 0; j < recs[i].nb; j++) free(recs[i].body[j]);
        free(recs[i].body);
    }
    free(recs);
    return 0;
}