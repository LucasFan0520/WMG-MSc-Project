// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} KeyValue;

int main() {
    KeyValue *list = NULL;
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
            char key[256], val[256];
            if (sscanf(ptr, "%255s %255s", key, val) == 2) {
                list = realloc(list, (count + 1) * sizeof(KeyValue));
                list[count].key = strdup(key);
                list[count].value = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], val[256];
            if (sscanf(ptr, "%255s %255s", key, val) == 2) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0 && strcmp(list[i].value, val) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].key);
                    free(list[found].value);
                    for (int i = found; i < count - 1; i++) {
                        list[i] = list[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        list = realloc(list, count * sizeof(KeyValue));
                    } else {
                        free(list);
                        list = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                int i = 0;
                while (i < count) {
                    if (strcmp(list[i].key, key) == 0) {
                        free(list[i].key);
                        free(list[i].value);
                        for (int j = i; j < count - 1; j++) {
                            list[j] = list[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count > 0) {
                    list = realloc(list, count * sizeof(KeyValue));
                } else {
                    free(list);
                    list = NULL;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                int first = 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0) {
                        if (!first) printf(" ");
                        printf("%s", list[i].value);
                        first = 0;
                    }
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            char **printed = NULL;
            int p_count = 0;
            for (int i = 0; i < count; i++) {
                int already_printed = 0;
                for (int j = 0; j < p_count; j++) {
                    if (strcmp(printed[j], list[i].key) == 0) {
                        already_printed = 1;
                        break;
                    }
                }
                if (!already_printed) {
                    printf("%s\n", list[i].key);
                    printed = realloc(printed, (p_count + 1) * sizeof(char *));
                    printed[p_count] = list[i].key;
                    p_count++;
                }
            }
            free(printed);
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}
