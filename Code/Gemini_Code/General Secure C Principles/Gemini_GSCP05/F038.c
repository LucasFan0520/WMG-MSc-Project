// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *encoded;
} RLE;

int main(void) {
    RLE *records = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *enc = p + 1;
                while (*enc == ' ') enc++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RLE *new_r = realloc(records, capacity * sizeof(RLE));
                    if (new_r) records = new_r;
                }
                if (count < capacity) {
                    records[count].name = strdup(name);
                    records[count].encoded = strdup(enc);
                    count++;
                }
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    char *enc = records[i].encoded;
                    int valid = 1;
                    while (*enc) {
                        if (*enc >= '0' && *enc <= '9') {
                            char *endptr;
                            long cnt = strtol(enc, &endptr, 10);
                            if (cnt < 0 || cnt > 10000) {
                                valid = 0;
                                break;
                            }
                            if (*endptr) {
                                char ch = *endptr;
                                if (ch == ' ') ch = '_';
                                for (long j = 0; j < cnt; j++) {
                                    putchar(ch);
                                }
                                enc = endptr + 1;
                            } else {
                                valid = 0;
                                break;
                            }
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (valid) {
                        putchar('\n');
                    } else {
                        printf("INVALID\n");
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].encoded);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].encoded);
    }
    free(records);
    return 0;
}
