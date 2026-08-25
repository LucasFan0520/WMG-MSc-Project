// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLEStore;

char *read_line(FILE *f) {
    size_t size = 32;
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    RLEStore *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", list[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "STORE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *enc = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].encoded);
                    list[found].encoded = strdup(enc);
                } else {
                    list = realloc(list, (count + 1) * sizeof(RLEStore));
                    list[count].name = strdup(name);
                    list[count].encoded = strdup(enc);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].encoded);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(RLEStore));
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *enc = list[found].encoded;
                int idx = 0;
                int valid = 1;
                while (enc[idx] != '\0') {
                    long c_num = 0;
                    if (!(enc[idx] >= '0' && enc[idx] <= '9')) {
                        valid = 0;
                        break;
                    }
                    while (enc[idx] >= '0' && enc[idx] <= '9') {
                        c_num = c_num * 10 + (enc[idx] - '0');
                        if (c_num > 10000) {
                            valid = 0;
                            break;
                        }
                        idx++;
                    }
                    if (!valid) break;
                    char ch = enc[idx];
                    if (ch == '\0') {
                        valid = 0;
                        break;
                    }
                    for (int k = 0; k < c_num; k++) {
                        if (ch == ' ') putchar('_');
                        else putchar(ch);
                    }
                    idx++;
                }
                if (valid) {
                    putchar('\n');
                } else {
                    printf("INVALID\n");
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].encoded);
    }
    free(list);
    return 0;
}