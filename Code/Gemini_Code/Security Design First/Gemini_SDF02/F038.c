/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLRec;

int main(void) {
    RLRec *store = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "STORE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *encoded = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                RLRec *temp = realloc(store, new_cap * sizeof(RLRec));
                if (!temp) break;
                store = temp;
                capacity = new_cap;
            }
            store[count].name = strdup(name);
            store[count].encoded = strdup(encoded);
            if (store[count].name && store[count].encoded) count++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *enc = store[i].encoded;
                    int valid = 1;
                    while (*enc) {
                        if (*enc >= '0' && *enc <= '9') {
                            long long repeat = 0;
                            while (*enc >= '0' && *enc <= '9') {
                                repeat = repeat * 10 + (*enc - '0');
                                if (repeat > 100000) { valid = 0; break; }
                                enc++;
                            }
                            if (!valid || *enc == '\0') { valid = 0; break; }
                            char c = *enc;
                            for (long long r = 0; r < repeat; r++) {
                                if (c == ' ') putchar('_');
                                else putchar(c);
                            }
                            enc++;
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (valid) printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
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
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
