// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} LPrefixedRecord;

LPrefixedRecord *records = NULL;
int count = 0;
int capacity = 0;

int find_record(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *prefix = sp + 1;
                char *colon = strchr(prefix, ':');
                if (colon) {
                    *colon = 0;
                    int len = atoi(prefix);
                    char *data = colon + 1;
                    if ((int)strlen(data) >= len && len >= 0) {
                        char *stored_data = malloc(len + 1);
                        strncpy(stored_data, data, len);
                        stored_data[len] = 0;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            records = realloc(records, capacity * sizeof(LPrefixedRecord));
                        }
                        records[count].name = strdup(p1);
                        records[count].data = stored_data;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_record(name);
            if (idx != -1) {
                free(records[idx].name);
                free(records[idx].data);
                for (int i = idx; i < count - 1; i++) {
                    records[i] = records[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            int idx = find_record(name);
            if (idx != -1) {
                char *d = records[idx].data;
                while (*d) {
                    if (*d == ' ') {
                        putchar('_');
                    } else {
                        putchar(*d);
                    }
                    d++;
                }
                putchar('\n');
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}
