// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
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

int main(void) {
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
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "DUMP") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].name, arr[i].expansion);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *exp = p2 + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    char *nexp = strdup(exp);
                    if (nexp) {
                        free(arr[found].expansion);
                        arr[found].expansion = nexp;
                    }
                } else {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Alias *narr = realloc(arr, ncap * sizeof(Alias));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].name = strdup(name);
                        arr[count].expansion = strdup(exp);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    for (size_t j = 0; arr[i].expansion[j] != '\0'; j++) {
                        if (arr[i].expansion[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(arr[i].expansion[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old_name = args;
            char *p2 = strchr(old_name, ' ');
            if (p2) {
                *p2 = '\0';
                char *new_name = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, old_name) == 0) {
                        char *nname = strdup(new_name);
                        if (nname) {
                            free(arr[i].name);
                            arr[i].name = nname;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = args;
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
