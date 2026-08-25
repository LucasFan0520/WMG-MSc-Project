// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *body; } Rec;
Rec *r = NULL;
int n = 0;

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            char title[256] = "", body[4096] = "";
            int in_rec = 0;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) {
                    if (in_rec && title[0]) {
                        r = realloc(r, (n + 1) * sizeof(Rec));
                        r[n].title = strdup(title); r[n].body = strdup(body); n++;
                    }
                    title[0] = 0; body[0] = 0; in_rec = 1;
                } else if (in_rec && !title[0]) {
                    strcpy(title, line);
                } else if (in_rec) {
                    if (body[0]) strcat(body, "\n");
                    strcat(body, line);
                }
            }
            if (in_rec && title[0]) {
                r = realloc(r, (n + 1) * sizeof(Rec));
                r[n].title = strdup(title); r[n].body = strdup(body); n++;
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], title[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) if (strcmp(r[i].title, title) == 0) { printf("%s\n%s\n", r[i].title, r[i].body); break; }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) if (strcmp(r[i].title, title) == 0) {
                free(r[i].title); free(r[i].body);
                for (int j = i; j < n - 1; j++) r[j] = r[j + 1];
                n--; break;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", title);
            char body[4096] = "";
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                if (body[0]) strcat(body, "\n");
                strcat(body, line);
            }
            r = realloc(r, (n + 1) * sizeof(Rec));
            r[n].title = strdup(title); r[n].body = strdup(body); n++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < n; i++) printf("%s\n%s\n--\n", r[i].title, r[i].body);
        }
    }
    for (int i = 0; i < n; i++) { free(r[i].title); free(r[i].body); }
    free(r);
    return 0;
}