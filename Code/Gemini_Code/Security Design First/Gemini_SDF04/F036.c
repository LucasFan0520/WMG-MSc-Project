// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} Record36;

Record36 *store = NULL;
int count = 0;
int capacity = 0;

int find_rec(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

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
                    char *len_data = space2 + 1;
                    char *colon = strchr(len_data, ':');
                    if (colon) {
                        *colon = '\0';
                        int dlen = atoi(len_data);
                        char *data_ptr = colon + 1;
                        if (dlen >= 0 && strlen(data_ptr) >= (size_t)dlen) {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Record36 *tmp = realloc(store, capacity * sizeof(Record36));
                                if (!tmp) {
                                    free(line);
                                    return 1;
                                }
                                store = tmp;
                            }
                            store[count].name = strdup(arg1);
                            char *ds = malloc(dlen + 1);
                            if (ds) {
                                strncpy(ds, data_ptr, dlen);
                                ds[dlen] = '\0';
                                store[count].data = ds;
                                if (store[count].name) {
                                    count++;
                                } else {
                                    free(ds);
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_rec(arg1);
                if (idx != -1) {
                    free(store[idx].name);
                    free(store[idx].data);
                    for (int i = idx; i < count - 1; i++) {
                        store[i] = store[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "SHOW") == 0) {
                int idx = find_rec(arg1);
                if (idx != -1) {
                    char *p = store[idx].data;
                    for (size_t i = 0; p[i] != '\0'; i++) {
                        if (p[i] == ' ') putchar('_');
                        else putchar(p[i]);
                    }
                    putchar('\n');
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", store[i].name);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].data);
    }
    free(store);
    return 0;
}