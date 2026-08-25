// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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
    char *data;
} Record;

int main() {
    Record *items = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *len_str = p;
            char *colon = strchr(len_str, ':');
            if (colon) {
                *colon = '\0';
                int expected_len = atoi(len_str);
                char *data = colon + 1;
                if ((int)strlen(data) >= expected_len && expected_len >= 0) {
                    char *data_buf = malloc(expected_len + 1);
                    if (data_buf) {
                        memcpy(data_buf, data, expected_len);
                        data_buf[expected_len] = '\0';
                        if (count >= cap) {
                            cap = cap == 0 ? 16 : cap * 2;
                            Record *nb = realloc(items, cap * sizeof(Record));
                            if (nb) items = nb;
                        }
                        items[count].name = strdup(name);
                        items[count].data = data_buf;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].data);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    char *dptr = items[i].data;
                    while (*dptr) {
                        if (*dptr == ' ') {
                            putchar('_');
                        } else {
                            putchar(*dptr);
                        }
                        dptr++;
                    }
                    putchar('\n');
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].data);
    }
    free(items);
    return 0;
}
