// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImportRecord;

ImportRecord *registry = NULL;
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
            if (strcmp(cmd, "BATCH") == 0) {
                int n = atoi(arg1);
                if (n > 0) {
                    char **batch_lines = malloc(n * sizeof(char*));
                    if (!batch_lines) {
                        free(line);
                        return 1;
                    }
                    int all_valid = 1;
                    for (int i = 0; i < n; i++) {
                        batch_lines[i] = NULL;
                        size_t blen = 0;
                        if (getline(&batch_lines[i], &blen, stdin) == -1) {
                            all_valid = 0;
                        } else {
                            size_t bl = strlen(batch_lines[i]);
                            if (bl > 0 && batch_lines[i][bl - 1] == '\n') {
                                batch_lines[i][bl - 1] = '\0';
                            }
                            char *s = strchr(batch_lines[i], ' ');
                            if (!s || s == batch_lines[i] || strlen(s + 1) == 0) {
                                all_valid = 0;
                            }
                        }
                    }
                    if (all_valid) {
                        for (int i = 0; i < n; i++) {
                            char *s = strchr(batch_lines[i], ' ');
                            *s = '\0';
                            char *name = batch_lines[i];
                            char *note = s + 1;
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                ImportRecord *tmp = realloc(registry, capacity * sizeof(ImportRecord));
                                if (!tmp) {
                                    free(line);
                                    return 1;
                                }
                                registry = tmp;
                            }
                            registry[count].name = strdup(name);
                            registry[count].note = strdup(note);
                            if (registry[count].name && registry[count].note) {
                                count++;
                            }
                        }
                    }
                    for (int i = 0; i < n; i++) {
                        free(batch_lines[i]);
                    }
                    free(batch_lines);
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(registry[i].name, arg1) == 0) {
                        free(registry[i].name);
                        free(registry[i].note);
                        for (int j = i; j < count - 1; j++) {
                            registry[j] = registry[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].name, arg1) == 0) {
                        printf("%s\n", registry[i].note);
                        break;
                    }
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", registry[i].name, registry[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(registry[i].name);
        free(registry[i].note);
    }
    free(registry);
    return 0;
}