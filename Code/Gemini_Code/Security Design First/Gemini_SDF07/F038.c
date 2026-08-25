/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *store = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = args;
            char *enc = space + 1;
            int valid = 1;
            char *p = enc;
            while (*p) {
                if (!isdigit(*p)) { valid = 0; break; }
                long repeat_c = 0;
                while (*p && isdigit(*p)) {
                    repeat_c = repeat_c * 10 + (*p - '0');
                    if (repeat_c > 10000) { valid = 0; break; }
                    p++;
                }
                if (repeat_c == 0 || !*p) { valid = 0; break; }
                p++;
            }
            if (valid) {
                char *n_name = strdup(name);
                char *n_enc = strdup(enc);
                if (n_name && n_enc) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        RLEString *tmp = realloc(store, new_cap * sizeof(RLEString));
                        if (tmp) {
                            store = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_name); free(n_enc);
                            *space = ' ';
                            continue;
                        }
                    }
                    store[count].name = n_name;
                    store[count].encoded = n_enc;
                    count++;
                } else {
                    free(n_name); free(n_enc);
                }
            }
            *space = ' ';
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *p = store[i].encoded;
                    while (*p) {
                        int repeat_c = 0;
                        while (isdigit(*p)) {
                            repeat_c = repeat_c * 10 + (*p - '0');
                            p++;
                        }
                        char c = *p;
                        p++;
                        for (int j = 0; j < repeat_c; j++) {
                            if (c == ' ') putchar('_');
                            else putchar(c);
                        }
                    }
                    putchar('\n');
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
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
