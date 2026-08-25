// F004.c
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
    char *key;
    char *val;
} KV;

int main() {
    KV *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *key_start = line + 4;
            char *space = strchr(key_start, ' ');
            char *val_start = "";
            if (space) {
                *space = '\0';
                val_start = space + 1;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key_start) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(arr[found].val);
                arr[found].val = strdup(val_start);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(KV));
                }
                arr[count].key = strdup(key_start);
                arr[count].val = strdup(val_start);
                count++;
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key_start = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key_start) == 0) {
                    free(arr[i].key);
                    free(arr[i].val);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            while (*text) {
                if (*text == '{') {
                    char *close = strchr(text, '}');
                    if (close) {
                        size_t klen = close - text - 1;
                        char *kbuf = malloc(klen + 1);
                        strncpy(kbuf, text + 1, klen);
                        kbuf[klen] = '\0';
                        char *v = NULL;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(arr[i].key, kbuf) == 0) {
                                v = arr[i].val;
                                break;
                            }
                        }
                        if (v) {
                            printf("%s", v);
                        } else {
                            fwrite(text, 1, close - text + 1, stdout);
                        }
                        free(kbuf);
                        text = close + 1;
                    } else {
                        putchar(*text);
                        text++;
                    }
                } else {
                    putchar(*text);
                    text++;
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].key, arr[i].val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].key);
        free(arr[i].val);
    }
    free(arr);
    return 0;
}
