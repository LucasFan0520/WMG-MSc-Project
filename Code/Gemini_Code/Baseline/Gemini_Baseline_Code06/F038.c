// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main() {
    RLEString *store = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char name[128], enc[256];
            if (sscanf(line + 6, "%127s %255s", name, enc) == 2) {
                store = realloc(store, (count + 1) * sizeof(RLEString));
                store[count].name = strdup(name);
                store[count].encoded = strdup(enc);
                count++;
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *p = store[i].encoded;
                    int valid = 1;
                    while (*p) {
                        if (*p >= '0' && *p <= '9') {
                            int cnt = 0;
                            while (*p >= '0' && *p <= '9') {
                                cnt = cnt * 10 + (*p - '0');
                                if (cnt > 10000) { valid = 0; break; }
                                p++;
                            }
                            if (!valid || *p == '\0') { valid = 0; break; }
                            char c = *p;
                            if (c == ' ') c = '_';
                            for (int k = 0; k < cnt; k++) {
                                putchar(c);
                            }
                            p++;
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (valid) printf("\n");
                    else printf("INVALID\n");
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].encoded);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    store = realloc(store, count * sizeof(RLEString));
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
