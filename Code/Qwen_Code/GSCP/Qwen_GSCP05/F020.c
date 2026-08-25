// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char **body; int bc; } Rec;
Rec *recs = NULL; int rc = 0;

int find_rec(const char *t) {
    for (int i = 0; i < rc; i++) if (strcmp(recs[i].title, t) == 0) return i;
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) continue;
                recs = realloc(recs, (rc+1)*sizeof(Rec));
                recs[rc].title = strdup(line); recs[rc].body = NULL; recs[rc].bc = 0;
                while (fgets(line, sizeof(line), f)) {
                    line[strcspn(line, "\n")] = 0;
                    if (strcmp(line, "--") == 0) break;
                    recs[rc].body = realloc(recs[rc].body, (recs[rc].bc+1)*sizeof(char*));
                    recs[rc].body[recs[rc].bc++] = strdup(line);
                }
                rc++;
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256];
        c2[0] = 0;
        sscanf(line, "%19s %255[^\n]", c1, c2);
        
        if (strcmp(c1, "FIND") == 0) {
            int i = find_rec(c2);
            if (i >= 0) {
                printf("%s\n", recs[i].title);
                for (int j = 0; j < recs[i].bc; j++) printf("%s\n", recs[i].body[j]);
            }
        } else if (strcmp(c1, "DELETE") == 0) {
            int i = find_rec(c2);
            if (i >= 0) {
                free(recs[i].title);
                for (int j = 0; j < recs[i].bc; j++) free(recs[i].body[j]);
                free(recs[i].body);
                for (int j = i; j < rc-1; j++) recs[j] = recs[j+1];
                rc--;
            }
        } else if (strcmp(c1, "ADD") == 0) {
            recs = realloc(recs, (rc+1)*sizeof(Rec));
            recs[rc].title = strdup(c2); recs[rc].body = NULL; recs[rc].bc = 0;
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                recs[rc].body = realloc(recs[rc].body, (recs[rc].bc+1)*sizeof(char*));
                recs[rc].body[recs[rc].bc++] = strdup(line);
            }
            rc++;
        } else if (strcmp(c1, "PRINT") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s\n", recs[i].title);
                for (int j = 0; j < recs[i].bc; j++) printf("%s\n", recs[i].body[j]);
                printf("--\n");
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(recs[i].title);
        for (int j = 0; j < recs[i].bc; j++) free(recs[i].body[j]);
        free(recs[i].body);
    }
    free(recs);
    return 0;
}