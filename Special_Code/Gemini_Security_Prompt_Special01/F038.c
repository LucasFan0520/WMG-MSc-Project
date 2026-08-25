// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *encoded;
} RLE_Record;

int main(void) {
    RLE_Record *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "STORE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *enc = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                enc++;
            }
            while (*enc == ' ') enc++;
            char *enc_end = enc;
            while (*enc_end && *enc_end != ' ') enc_end++;
            *enc_end = '\0';
            int valid = 1;
            char *t = enc;
            while (*t) {
                if (*t >= '0' && *t <= '9') {
                    long long val = 0;
                    while (*t && *t >= '0' && *t <= '9') {
                        val = val * 10 + (*t - '0');
                        if (val > 100000) {
                            valid = 0;
                            break;
                        }
                        t++;
                    }
                    if (!valid || *t == '\0') {
                        valid = 0;
                        break;
                    }
                    t++;
                } else {
                    valid = 0;
                    break;
                }
            }
            if (valid) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RLE_Record *new_list = realloc(list, capacity * sizeof(RLE_Record));
                    if (new_list) list = new_list;
                }
                list[count].name = mystrdup(name);
                list[count].encoded = mystrdup(enc);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    char *t = list[i].encoded;
                    while (*t) {
                        int num = 0;
                        while (*t && *t >= '0' && *t <= '9') {
                            num = num * 10 + (*t - '0');
                            t++;
                        }
                        char ch = *t;
                        if (ch == ' ') ch = '_';
                        for (int j = 0; j < num; j++) {
                            putchar(ch);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
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
