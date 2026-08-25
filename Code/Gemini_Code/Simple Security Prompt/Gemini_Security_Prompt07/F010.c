// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int value_count;
} IndexItem;

int main() {
    IndexItem *items = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            if (key && val) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    items = realloc(items, (count + 1) * sizeof(IndexItem));
                    items[count].key = strdup(key);
                    items[count].values = NULL;
                    items[count].value_count = 0;
                    found = count;
                    count++;
                }
                items[found].values = realloc(items[found].values, (items[found].value_count + 1) * sizeof(char *));
                items[found].values[items[found].value_count] = strdup(val);
                items[found].value_count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            if (key && val) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].key, key) == 0) {
                        for (int j = 0; j < items[i].value_count; j++) {
                            if (strcmp(items[i].values[j], val) == 0) {
                                free(items[i].values[j]);
                                for (int k = j; k < items[i].value_count - 1; k++) {
                                    items[i].values[k] = items[i].values[k + 1];
                                }
                                items[i].value_count--;
                                if (items[i].value_count == 0) {
                                    free(items[i].values);
                                    items[i].values = NULL;
                                } else {
                                    items[i].values = realloc(items[i].values, items[i].value_count * sizeof(char *));
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].key, key) == 0) {
                        free(items[i].key);
                        for (int j = 0; j < items[i].value_count; j++) {
                            free(items[i].values[j]);
                        }
                        free(items[i].values);
                        for (int j = i; j < count - 1; j++) {
                            items[j] = items[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(items);
                    items = NULL;
                } else {
                    items = realloc(items, count * sizeof(IndexItem));
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].key, key) == 0) {
                        for (int j = 0; j < items[i].value_count; j++) {
                            printf("%s", items[i].values[j]);
                            if (j < items[i].value_count - 1) {
                                printf(" ");
                            }
                        }
                        break;
                    }
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", items[i].key);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].key);
        for (int j = 0; j < items[i].value_count; j++) {
            free(items[i].values[j]);
        }
        free(items[i].values);
    }
    free(items);
    return 0;
}
