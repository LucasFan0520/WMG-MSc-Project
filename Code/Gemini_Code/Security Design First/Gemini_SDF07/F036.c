/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *data;
} Record36;

int main(void) {
    Record36 *store = NULL;
    int count = 0;
    int capacity = 0;
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
            if (!space) continue;
            *space = '\0';
            char *name = args;
            char *rest = space + 1;
            char *colon = strchr(rest, ':');
            if (!colon) {
                *space = ' ';
                continue;
            }
            *colon = '\0';
            int dlen = atoi(rest);
            char *data_start = colon + 1;
            *colon = ':';
            *space = ' ';
            if (dlen < 0 || (int)strlen(data_start) < dlen) {
                continue;
            }
            char *n_name = strdup(name);
            char *n_data = malloc(dlen + 1);
            if (n_name && n_data) {
                memcpy(n_data, data_start, dlen);
                n_data[dlen] = '\0';
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Record36 *tmp = realloc(store, new_cap * sizeof(Record36));
                    if (tmp) {
                        store = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name); free(n_data);
                        continue;
                    }
                }
                store[count].name = n_name;
                store[count].data = n_data;
                count++;
            } else {
                free(n_name); free(n_data);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].data);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *p = store[i].data;
                    while (*p) {
                        if (*p == ' ') putchar('_');
                        else putchar(*p);
                        p++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
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
