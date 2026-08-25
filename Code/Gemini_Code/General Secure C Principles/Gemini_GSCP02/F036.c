// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} PrefixedRecord;

int main(void) {
    PrefixedRecord *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *rem = strchr(p, ' ');
                if (rem) {
                    while (*rem == ' ') rem++;
                    char *colon = strchr(rem, ':');
                    if (colon) {
                        *colon = '\0';
                        int len = atoi(rem);
                        char *data_start = colon + 1;
                        if (len >= 0) {
                            char *data_buf = malloc(len + 1);
                            if (data_buf) {
                                strncpy(data_buf, data_start, len);
                                data_buf[len] = '\0';
                                PrefixedRecord *tmp = realloc(records, (count + 1) * sizeof(PrefixedRecord));
                                if (tmp) {
                                    records = tmp;
                                    records[count].name = strdup(name);
                                    records[count].data = data_buf;
                                    count++;
                                } else {
                                    free(data_buf);
                                }
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].data);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    for (int k = 0; records[i].data[k]; k++) {
                        putchar(records[i].data[k] == ' ' ? '_' : records[i].data[k]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
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
