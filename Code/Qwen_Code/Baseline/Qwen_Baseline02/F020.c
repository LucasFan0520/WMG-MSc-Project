// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *body; } Cat;
Cat cat[10000]; int nc = 0;

int find(char *title) {
    for (int i = 0; i < nc; i++) if (!strcmp(cat[i].title, title)) return i;
    return -1;
}

void load(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000], title[1000], body[100000] = "";
    int reading = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            if (reading) {
                cat[nc].title = strdup(title); cat[nc++].body = strdup(body);
                body[0] = 0;
            }
            reading = 1;
            if (fgets(title, sizeof(title), f)) title[strcspn(title, "\n")] = 0;
        } else if (reading) {
            if (strlen(body) > 0) strcat(body, "\n");
            strcat(body, line);
        }
    }
    if (reading) { cat[nc].title = strdup(title); cat[nc++].body = strdup(body); }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char cmd[20], title[1000], line[10000], body[100000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "FIND")) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            int idx = find(title);
            if (idx != -1) printf("%s\n%s\n", cat[idx].title, cat[idx].body);
        } else if (!strcmp(cmd, "DELETE")) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            int idx = find(title);
            if (idx != -1) {
                free(cat[idx].title); free(cat[idx].body);
                for (int i = idx; i < nc - 1; i++) cat[i] = cat[i + 1];
                nc--;
            }
        } else if (!strcmp(cmd, "ADD")) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            body[0] = 0;
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                if (strlen(body) > 0) strcat(body, "\n");
                strcat(body, line);
            }
            cat[nc].title = strdup(title); cat[nc++].body = strdup(body);
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < nc; i++) printf("%s\n%s\n--\n", cat[i].title, cat[i].body);
        }
    }
    for (int i = 0; i < nc; i++) { free(cat[i].title); free(cat[i].body); }
    return 0;
}