// F032.c
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
    char *key;
    char *value;
} Setting;

int main(void) {
    Setting *committed = NULL;
    int c_count = 0;
    int c_capacity = 0;
    Setting *staged = NULL;
    int s_count = 0;
    int s_capacity = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                while (*val == ' ') val++;
                int found = 0;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        free(committed[i].value);
                        committed[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (c_count >= c_capacity) {
                        c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                        Setting *new_c = realloc(committed, c_capacity * sizeof(Setting));
                        if (new_c) committed = new_c;
                    }
                    if (c_count < c_capacity) {
                        committed[c_count].key = strdup(key);
                        committed[c_count].value = strdup(val);
                        c_count++;
                    }
                }
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 1;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_tx) {
                char *p = line + 5;
                while (*p == ' ') p++;
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *val = p + 1;
                    while (*val == ' ') val++;
                    int found = 0;
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            free(staged[i].value);
                            staged[i].value = strdup(val);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (s_count >= s_capacity) {
                            s_capacity = s_capacity == 0 ? 4 : s_capacity * 2;
                            Setting *new_s = realloc(staged, s_capacity * sizeof(Setting));
                            if (new_s) staged = new_s;
                        }
                        if (s_count < s_capacity) {
                            staged[s_count].key = strdup(key);
                            staged[s_count].value = strdup(val);
                            s_count++;
                        }
                    }
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    int found = 0;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = staged[i].value;
                            staged[i].value = NULL;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_capacity) {
                            c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                            Setting *new_c = realloc(committed, c_capacity * sizeof(Setting));
                            if (new_c) committed = new_c;
                        }
                        if (c_count < c_capacity) {
                            committed[c_count].key = staged[i].key;
                            committed[c_count].value = staged[i].value;
                            staged[i].key = NULL;
                            staged[i].value = NULL;
                            c_count++;
                        }
                    }
                }
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    printf("%s\n", committed[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (int i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}
