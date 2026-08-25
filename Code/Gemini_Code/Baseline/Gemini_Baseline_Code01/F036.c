// F036.c
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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int b = 0;
            if (sscanf(args, "%255s%n", name, &b) == 1) {
                char *prefix = args + b;
                while (*prefix == ' ') prefix++;
                char *colon = strchr(prefix, ':');
                if (colon) {
                    *colon = '\0';
                    int len = atoi(prefix);
                    char *data_ptr = colon + 1;
                    if (len >= 0 && (int)strlen(data_ptr) >= len) {
                        char *data_buf = malloc(len + 1);
                        strncpy(data_buf, data_ptr, len);
                        data_buf[len] = '\0';
                        records = realloc(records, (count + 1) * sizeof(PrefixedRecord));
                        records[count].name = strdup(name);
                        records[count].data = data_buf;
                        count++;
                    }
                    *colon = ':';
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        free(records[i].name);
                        free(records[i].data);
                        for (int j = i; j < count - 1; j++) {
                            records[j] = records[j + 1];
                        }
                        count--;
                        records = realloc(records, count * sizeof(PrefixedRecord));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        for (int j = 0; records[i].data[j] != '\0'; j++) {
                            if (records[i].data[j] == ' ') printf("_");
                            else printf("%c", records[i].data[j]);
                        }
                        printf("\n");
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
