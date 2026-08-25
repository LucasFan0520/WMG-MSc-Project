// F038.c
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *store = NULL;
    int count = 0;
    int cap = 0;
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
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    store = realloc(store, cap * sizeof(RLEString));
                }
                store[count].name = strdup(name);
                store[count].encoded = strdup(enc);
                count++;
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *p = store[i].encoded;
                    int valid = 1;
                    while (*p) {
                        if (*p >= '0' && *p <= '9') {
                            char *endptr;
                            long cnt = strtol(p, &endptr, 10);
                            if (cnt <= 0 || cnt > 100000 || *endptr == '\0') {
                                valid = 0;
                                break;
                            }
                            p = endptr + 1;
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (valid) {
                        p = store[i].encoded;
                        while (*p) {
                            char *endptr;
                            long cnt = strtol(p, &endptr, 10);
                            char ch = *endptr;
                            for (long j = 0; j < cnt; j++) {
                                if (ch == ' ') putchar('_');
                                else putchar(ch);
                            }
                            p = endptr + 1;
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(store[found].name);
                free(store[found].encoded);
                for (int i = found; i < count - 1; i++) {
                    store[i] = store[i + 1];
                }
                count--;
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
