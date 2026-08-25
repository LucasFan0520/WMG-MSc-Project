// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *data;
} PrefixRecord;

int main(void) {
    PrefixRecord *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *rest = p;
            char *colon = strchr(rest, ':');
            if (colon) {
                *colon = '\0';
                int len = atoi(rest);
                char *data_start = colon + 1;
                if (len >= 0 && (size_t)len <= strlen(data_start)) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        PrefixRecord *nb = realloc(list, cap * sizeof(PrefixRecord));
                        if (nb) list = nb;
                    }
                    if (count < cap) {
                        list[count].name = strdup(name);
                        char *d = malloc(len + 1);
                        if (d) {
                            memcpy(d, data_start, len);
                            d[len] = '\0';
                            list[count].data = d;
                            count++;
                        } else {
                            free(list[count].name);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    free(list[i].data);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    for (size_t j = 0; list[i].data[j] != '\0'; j++) {
                        if (list[i].data[j] == ' ') putchar('_');
                        else putchar(list[i].data[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].data);
    }
    free(list);
    return 0;
}
