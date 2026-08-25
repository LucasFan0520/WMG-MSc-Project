// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} LPrefixedRecord;

int main() {
    LPrefixedRecord *records = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *rest = strtok(NULL, "");
            if (name && rest) {
                char *colon = strchr(rest, ':');
                if (colon) {
                    *colon = '\0';
                    int len = atoi(rest);
                    char *data = colon + 1;
                    if (len >= 0 && (int)strlen(data) >= len) {
                        records = realloc(records, (count + 1) * sizeof(LPrefixedRecord));
                        records[count].name = strdup(name);
                        char *buf = malloc(len + 1);
                        strncpy(buf, data, len);
                        buf[len] = '\0';
                        records[count].data = buf;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        free(records[i].name);
                        free(records[i].data);
                        for (int j = i; j < count - 1; j++) {
                            records[j] = records[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(records);
                    records = NULL;
                } else {
                    records = realloc(records, count * sizeof(LPrefixedRecord));
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        char *d = records[i].data;
                        for (int j = 0; d[j]; j++) {
                            if (d[j] == ' ') putchar('_');
                            else putchar(d[j]);
                        }
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
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
