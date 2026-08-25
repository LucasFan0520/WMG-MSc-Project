// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Entry;

Entry *index_table = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *val = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Entry *tmp = realloc(index_table, capacity * sizeof(Entry));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        index_table = tmp;
                    }
                    index_table[count].key = strdup(arg1);
                    index_table[count].value = strdup(val);
                    if (index_table[count].key && index_table[count].value) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "REMOVE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *val = space2 + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(index_table[i].key, arg1) == 0 && strcmp(index_table[i].value, val) == 0) {
                            free(index_table[i].key);
                            free(index_table[i].value);
                            for (int j = i; j < count - 1; j++) {
                                index_table[j] = index_table[j + 1];
                            }
                            count--;
                            break;
                        }
                    }
                }
            } else if (strcmp(cmd, "ERASE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(index_table[i].key, arg1) == 0) {
                        free(index_table[i].key);
                        free(index_table[i].value);
                        for (int j = i; j < count - 1; j++) {
                            index_table[j] = index_table[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "VALUES") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(index_table[i].key, arg1) == 0) {
                        printf("%s\n", index_table[i].value);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "KEYS") == 0) {
                char **printed = malloc(count * sizeof(char*));
                int p_count = 0;
                for (int i = 0; i < count; i++) {
                    int found = 0;
                    for (int j = 0; j < p_count; j++) {
                        if (strcmp(printed[j], index_table[i].key) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("%s\n", index_table[i].key);
                        if (printed) {
                            printed[p_count++] = index_table[i].key;
                        }
                    }
                }
                free(printed);
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(index_table[i].key);
        free(index_table[i].value);
    }
    free(index_table);
    return 0;
}