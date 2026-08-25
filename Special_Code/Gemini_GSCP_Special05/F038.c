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
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *store = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *p = line + 6;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *encoded = "";
            if (*p == ' ') {
                *p = '\0';
                encoded = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                RLEString *nb = realloc(store, cap * sizeof(RLEString));
                if (!nb) {
                    free(line);
                    break;
                }
                store = nb;
            }
            store[count].name = safe_dup(name);
            store[count].encoded = safe_dup(encoded);
            count++;
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *p = store[i].encoded;
                    int valid = 1;
                    while (*p && valid) {
                        if (*p >= '0' && *p <= '9') {
                            long long rep = 0;
                            while (*p >= '0' && *p <= '9') {
                                rep = rep * 10 + (*p - '0');
                                if (rep > 10000) {
                                    valid = 0;
                                    break;
                                }
                                p++;
                            }
                            if (valid && *p) {
                                char ch = *p;
                                if (ch == ' ') ch = '_';
                                for (long long k = 0; k < rep; k++) {
                                    putchar(ch);
                                }
                                p++;
                            }
                        } else {
                            char ch = *p;
                            if (ch == ' ') ch = '_';
                            putchar(ch);
                            p++;
                        }
                    }
                    if (!valid) {
                        printf("INVALID");
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
