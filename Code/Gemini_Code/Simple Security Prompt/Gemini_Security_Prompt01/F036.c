// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} PrefixedRecord;

int main() {
    PrefixedRecord *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *len_str = space2 + 1;
                while (*len_str == ' ') len_str++;
                char *colon = strchr(len_str, ':');
                if (colon) {
                    *colon = '\0';
                    int data_len = atoi(len_str);
                    char *data_ptr = colon + 1;
                    if (data_len >= 0 && (int)strlen(data_ptr) >= data_len) {
                        records = realloc(records, (count + 1) * sizeof(PrefixedRecord));
                        if (!records) return 1;
                        records[count].name = strdup(name);
                        records[count].data = malloc(data_len + 1);
                        if (!records[count].data) return 1;
                        strncpy(records[count].data, data_ptr, data_len);
                        records[count].data[data_len] = '\0';
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, args) == 0) {
                    free(records[i].name);
                    free(records[i].data);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    records = realloc(records, count * sizeof(PrefixedRecord));
                    if (count > 0 && !records) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, args) == 0) {
                    for (int j = 0; records[i].data[j] != '\0'; j++) {
                        if (records[i].data[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(records[i].data[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}
