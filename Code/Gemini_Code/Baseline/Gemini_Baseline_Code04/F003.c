// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

void print_underscores(const char *s) {
    while (*s) {
        if (*s == ' ') {
            putchar('_');
        } else {
            putchar(*s);
        }
        s++;
    }
    putchar('\n');
}

int main() {
    Alias *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *p = line + 7;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *exp = p + 1;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].expansion);
                        arr[i].expansion = strdup(exp);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    arr = realloc(arr, (count + 1) * sizeof(Alias));
                    arr[count].name = strdup(name);
                    arr[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    print_underscores(arr[i].expansion);
                    break;
                }
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p = line + 7;
            char *old_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *new_name = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, old_name) == 0) {
                        free(arr[i].name);
                        arr[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].expansion);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].expansion);
    }
    free(arr);
    return 0;
}
