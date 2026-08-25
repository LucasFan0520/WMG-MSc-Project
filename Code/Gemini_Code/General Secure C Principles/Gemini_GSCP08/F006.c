// F006.c
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
    char *item;
    long count;
} InvItem;

int main(void) {
    InvItem *arr = NULL;
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
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %ld\n", arr[i].item, arr[i].count);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *item = args;
            char *p2 = strchr(item, ' ');
            if (p2) {
                *p2 = '\0';
                long amt = strtol(p2 + 1, NULL, 10);
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    arr[found].count += amt;
                } else {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        InvItem *narr = realloc(arr, ncap * sizeof(InvItem));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].item = strdup(item);
                        arr[count].count = amt;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = args;
            char *p2 = strchr(item, ' ');
            if (p2) {
                *p2 = '\0';
                long amt = strtol(p2 + 1, NULL, 10);
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        arr[i].count -= amt;
                        if (arr[i].count < 0) {
                            arr[i].count = 0;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    free(arr[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = args;
            long val = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    val = arr[i].count;
                    break;
                }
            }
            printf("%ld\n", val);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].item);
    }
    free(arr);
    return 0;
}
