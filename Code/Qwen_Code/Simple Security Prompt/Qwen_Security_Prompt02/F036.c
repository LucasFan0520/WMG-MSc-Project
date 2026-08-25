// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
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
    char spec[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", name, spec);
            char *colon = strchr(spec, ':');
            if (colon == NULL) {
                continue;
            }
            *colon = '\0';
            int len = atoi(spec);
            if (len <= 0) {
                continue;
            }
            char *data_part = colon + 1;
            if ((int)strlen(data_part) != len) {
                continue;
            }
            int idx = find_name(name);
            if (idx != -1) {
                free(records[idx].data);
                records[idx].data = strdup(data_part);
            } else {
                records = realloc(records, sizeof(Record) * (r_count + 1));
                records[r_count].name = strdup(name);
                records[r_count].data = strdup(data_part);
                r_count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(records[idx].name);
                free(records[idx].data);
                for (int i = idx; i < r_count - 1; i++) {
                    records[i] = records[i + 1];
                }
                r_count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                for (int i = 0; records[idx].data[i]; i++) {
                    if (records[idx].data[i] == ' ') {
                        putchar('_');
                    } else {
                        putchar(records[idx].data[i]);
                    }
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }

    for (int i = 0; i < r_count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);

    return 0;
}