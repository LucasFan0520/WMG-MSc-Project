// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLE;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    RLE *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "STORE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *enc = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int valid = 1;
            long total_len = 0;
            size_t idx = 0;
            while (enc[idx]) {
                if (enc[idx] >= '0' && enc[idx] <= '9') {
                    long val = 0;
                    while (enc[idx] >= '0' && enc[idx] <= '9') {
                        val = val * 10 + (enc[idx] - '0');
                        if (val > 100000) {
                            valid = 0;
                            break;
                        }
                        idx++;
                    }
                    if (!enc[idx] || val <= 0) {
                        valid = 0;
                        break;
                    }
                    total_len += val;
                    if (total_len > 100000) {
                        valid = 0;
                        break;
                    }
                    idx++;
                } else {
                    valid = 0;
                    break;
                }
            }
            if (valid && total_len > 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RLE *nl = realloc(list, capacity * sizeof(RLE));
                    if (!nl) break;
                    list = nl;
                }
                list[count].name = strdup(name);
                list[count].encoded = strdup(enc);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    char *enc = list[i].encoded;
                    size_t idx = 0;
                    while (enc[idx]) {
                        long val = 0;
                        while (enc[idx] >= '0' && enc[idx] <= '9') {
                            val = val * 10 + (enc[idx] - '0');
                            idx++;
                        }
                        char ch = enc[idx];
                        for (long k = 0; k < val; k++) {
                            if (ch == ' ') putchar('_');
                            else putchar(ch);
                        }
                        idx++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].encoded);
    }
    free(list);
    return 0;
}
