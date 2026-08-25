// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} LengthPrefixedRecord;

int main() {
    LengthPrefixedRecord *records = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int name_len = 0;
            if (sscanf(ptr, "%255s%n", name, &name_len) > 0) {
                char *rem = ptr + name_len;
                while (*rem == ' ') rem++;
                int d_len = -1;
                int read_chars = 0;
                if (sscanf(rem, "%d:%n", &d_len, &read_chars) > 0 && d_len >= 0) {
                    char *data_start = rem + read_chars;
                    if ((int)strlen(data_start) >= d_len) {
                        char *data_buf = malloc(d_len + 1);
                        strncpy(data_buf, data_start, d_len);
                        data_buf[d_len] = '\0';
                        records = realloc(records, (count + 1) * sizeof(LengthPrefixedRecord));
                        records[count].name = strdup(name);
                        records[count].data = data_buf;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(records[found].name);
                    free(records[found].data);
                    for (int i = found; i < count - 1; i++) {
                        records[i] = records[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        records = realloc(records, count * sizeof(LengthPrefixedRecord));
                    } else {
                        free(records);
                        records = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *t = records[found].data;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                } else {
                    printf("NOT_FOUND\n");
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
