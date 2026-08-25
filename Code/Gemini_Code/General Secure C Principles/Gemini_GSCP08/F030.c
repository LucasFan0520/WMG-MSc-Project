// F030.c
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
    char *old;
    char *new_str;
} RenameRule;

int main(void) {
    RenameRule *arr = NULL;
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
            if (strcmp(cmd, "RULES") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].old, arr[i].new_str);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *old = args;
            char *p2 = strchr(old, ' ');
            if (p2) {
                *p2 = '\0';
                char *new_str = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    RenameRule *narr = realloc(arr, ncap * sizeof(RenameRule));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].old = strdup(old);
                    arr[count].new_str = strdup(new_str);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                char *pos = strstr(args, arr[i].old);
                if (pos) {
                    *pos = '\0';
                    printf("%s%s%s\n", args, arr[i].new_str, pos + strlen(arr[i].old));
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", args);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].old, old) == 0) {
                    free(arr[i].old);
                    free(arr[i].new_str);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *idx1_s = args;
            char *p2 = strchr(idx1_s, ' ');
            if (p2) {
                *p2 = '\0';
                long idx1 = strtol(idx1_s, NULL, 10);
                long idx2 = strtol(p2 + 1, NULL, 10);
                if (idx1 >= 0 && idx1 < (long)count && idx2 >= 0 && idx2 < (long)count) {
                    RenameRule tmp = arr[idx1];
                    arr[idx1] = arr[idx2];
                    arr[idx2] = tmp;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].old);
        free(arr[i].new_str);
    }
    free(arr);
    return 0;
}
