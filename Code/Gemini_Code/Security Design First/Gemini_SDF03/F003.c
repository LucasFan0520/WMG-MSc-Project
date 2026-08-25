// F003.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *exp = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                exp = p;
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].expansion);
                    arr[i].expansion = strdup(exp);
                    if (!arr[i].expansion) exit(1);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Alias *tmp = realloc(arr, capacity * sizeof(Alias));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                arr[count].expansion = strdup(exp);
                if (!arr[count].name || !arr[count].expansion) exit(1);
                count++;
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    char *t = arr[i].expansion;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *old = p;
            while (*p && *p != ' ') p++;
            char *new_name = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                new_name = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, old) == 0) {
                    free(arr[i].name);
                    arr[i].name = strdup(new_name);
                    if (!arr[i].name) exit(1);
                    break;
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].expansion);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].expansion);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].expansion);
    }
    free(arr);
    free(line);
    return 0;
}
