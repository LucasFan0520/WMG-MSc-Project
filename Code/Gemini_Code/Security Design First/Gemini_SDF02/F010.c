/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} IndexPair;

int main(void) {
    IndexPair *pairs = NULL;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *key_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val_str = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                IndexPair *temp = realloc(pairs, new_cap * sizeof(IndexPair));
                if (!temp) break;
                pairs = temp;
                capacity = new_cap;
            }
            pairs[count].key = strdup(key_str);
            pairs[count].value = strdup(val_str);
            if (pairs[count].key && pairs[count].value) {
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val_str = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key_str) == 0 && strcmp(pairs[i].value, val_str) == 0) {
                    free(pairs[i].key);
                    free(pairs[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        pairs[j] = pairs[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key_str = p;
            for (size_t i = 0; i < count; ) {
                if (strcmp(pairs[i].key, key_str) == 0) {
                    free(pairs[i].key);
                    free(pairs[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        pairs[j] = pairs[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key_str = p;
            int first = 1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key_str) == 0) {
                    if (!first) {
                        printf(" ");
                    }
                    printf("%s", pairs[i].value);
                    first = 0;
                }
            }
            printf("\n");
        } else if (strcmp(cmd, "KEYS") == 0) {
            char **unique_keys = NULL;
            size_t u_count = 0;
            size_t u_capacity = 0;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < u_count; j++) {
                    if (strcmp(unique_keys[j], pairs[i].key) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (u_count >= u_capacity) {
                        size_t new_cap = u_capacity == 0 ? 4 : u_capacity * 2;
                        char **temp = realloc(unique_keys, new_cap * sizeof(char *));
                        if (!temp) break;
                        unique_keys = temp;
                        u_capacity = new_cap;
                    }
                    unique_keys[u_count] = pairs[i].key;
                    u_count++;
                }
            }
            for (size_t i = 0; i < u_count; i++) {
                printf("%s\n", unique_keys[i]);
            }
            free(unique_keys);
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(pairs[i].key);
        free(pairs[i].value);
    }
    free(pairs);
    return 0;
}
