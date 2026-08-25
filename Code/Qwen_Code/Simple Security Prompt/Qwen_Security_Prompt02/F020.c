// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int lines;
    int cap;
} Catalog;

Catalog *cats = NULL;
int count = 0;

int find_title(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(cats[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

void add_body(int idx, const char *line) {
    if (cats[idx].lines == cats[idx].cap) {
        cats[idx].cap = cats[idx].cap ? cats[idx].cap * 2 : 4;
        cats[idx].body = realloc(cats[idx].body, sizeof(char*) * cats[idx].cap);
    }
    cats[idx].body[cats[idx].lines++] = strdup(line);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[10000];
    int idx = -1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            idx = -1;
        } else if (idx == -1) {
            cats = realloc(cats, sizeof(Catalog) * (count + 1));
            cats[count].title = strdup(line);
            cats[count].body = NULL;
            cats[count].lines = 0;
            cats[count].cap = 0;
            idx = count++;
        } else {
            add_body(idx, line);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char title[1000];
    char line[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "FIND") == 0) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') {
                memmove(title, title + 1, strlen(title));
            }
            int idx = find_title(title);
            if (idx != -1) {
                printf("%s\n", cats[idx].title);
                for (int i = 0; i < cats[idx].lines; i++) {
                    printf("%s\n", cats[idx].body[i]);
                }
                printf("--\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') {
                memmove(title, title + 1, strlen(title));
            }
            int idx = find_title(title);
            if (idx != -1) {
                free(cats[idx].title);
                for (int i = 0; i < cats[idx].lines; i++) {
                    free(cats[idx].body[i]);
                }
                free(cats[idx].body);
                for (int i = idx; i < count - 1; i++) {
                    cats[i] = cats[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') {
                memmove(title, title + 1, strlen(title));
            }
            cats = realloc(cats, sizeof(Catalog) * (count + 1));
            cats[count].title = strdup(title);
            cats[count].body = NULL;
            cats[count].lines = 0;
            cats[count].cap = 0;
            int idx = count++;
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) {
                    break;
                }
                add_body(idx, line);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", cats[i].title);
                for (int j = 0; j < cats[i].lines; j++) {
                    printf("%s\n", cats[i].body[j]);
                }
                printf("--\n");
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(cats[i].title);
        for (int j = 0; j < cats[i].lines; j++) {
            free(cats[i].body[j]);
        }
        free(cats[i].body);
    }
    free(cats);

    return 0;
}