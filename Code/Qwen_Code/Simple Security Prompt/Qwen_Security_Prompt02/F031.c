// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

Record *records = NULL;
int r_count = 0;

int find_name(const char *name) {
    for (int i = 0; i < r_count; i++) {
        if (strcmp(records[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char name[1024];
    char note[10240];
    int n;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BATCH") == 0) {
            scanf("%d", &n);
            if (n <= 0) {
                continue;
            }
            char **names = malloc(sizeof(char *) * n);
            char **notes = malloc(sizeof(char *) * n);
            int valid = 1;
            for (int i = 0; i < n; i++) {
                names[i] = NULL;
                notes[i] = NULL;
            }
            char line[11264];
            int consumed = 0;
            for (int i = 0; i < n; i++) {
                if (fgets(line, sizeof(line), stdin) == NULL) {
                    valid = 0;
                    consumed = i;
                    break;
                }
                line[strcspn(line, "\n")] = '\0';
                char *sp = strchr(line, ' ');
                if (sp == NULL) {
                    valid = 0;
                    consumed = i;
                    break;
                }
                *sp = '\0';
                names[i] = strdup(line);
                notes[i] = strdup(sp + 1);
                consumed = i + 1;
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    records = realloc(records, sizeof(Record) * (r_count + 1));
                    records[r_count].name = names[i];
                    records[r_count].note = notes[i];
                    r_count++;
                }
            } else {
                for (int i = 0; i < consumed; i++) {
                    free(names[i]);
                    free(notes[i]);
                }
            }
            free(names);
            free(notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(records[idx].name);
                free(records[idx].note);
                for (int i = idx; i < r_count - 1; i++) {
                    records[i] = records[i + 1];
                }
                r_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                printf("%s %s\n", records[idx].name, records[idx].note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
    }

    for (int i = 0; i < r_count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);

    return 0;
}