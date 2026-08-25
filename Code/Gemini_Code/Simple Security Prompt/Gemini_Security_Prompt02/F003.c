// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main() {
    Alias *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *name_start = line + 7;
            char *space = strchr(name_start, ' ');
            char *exp_start = "";
            if (space) {
                *space = '\0';
                exp_start = space + 1;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(arr[found].expansion);
                arr[found].expansion = strdup(exp_start);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Alias));
                }
                arr[count].name = strdup(name_start);
                arr[count].expansion = strdup(exp_start);
                count++;
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    char *e = arr[i].expansion;
                    while (*e) {
                        if (*e == ' ') putchar('_');
                        else putchar(*e);
                        e++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *old_start = line + 7;
            char *space = strchr(old_start, ' ');
            if (space) {
                *space = '\0';
                char *new_start = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, old_start) == 0) {
                        free(arr[i].name);
                        arr[i].name = strdup(new_start);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name_start = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].expansion);
    }
    free(arr);
    return 0;
}
