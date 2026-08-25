// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} PrefixedRecord;

int main(void) {
    PrefixedRecord *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *suffix = space + 1;
                char *colon = strchr(suffix, ':');
                if (colon) {
                    *colon = '\0';
                    int d_len = atoi(suffix);
                    char *data_ptr = colon + 1;
                    if (d_len >= 0 && strlen(data_ptr) >= (size_t)d_len) {
                        char *data = malloc(d_len + 1);
                        if (data) {
                            memcpy(data, data_ptr, d_len);
                            data[d_len] = '\0';
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                PrefixedRecord *next = realloc(list, capacity * sizeof(PrefixedRecord));
                                if (!next) {
                                    free(data);
                                    break;
                                }
                                list = next;
                            }
                            list[count].name = strdup(name);
                            list[count].data = data;
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].data);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    char *d = list[i].data;
                    for (size_t k = 0; d[k] != '\0'; k++) {
                        if (d[k] == ' ') putchar('_');
                        else putchar(d[k]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].data);
    }
    free(list);
    free(line);
    return 0;
}
