// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

Variable *vars = NULL;
int count = 0;
int capacity = 0;

int find_var(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(vars[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void set_v(const char *key, const char *val) {
    int idx = find_var(key);
    if (idx != -1) {
        free(vars[idx].value);
        vars[idx].value = strdup(val);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        vars = realloc(vars, capacity * sizeof(Variable));
    }
    vars[count].key = strdup(key);
    vars[count].value = strdup(val);
    count++;
}

void unset_v(const char *key) {
    int idx = find_var(key);
    if (idx != -1) {
        free(vars[idx].key);
        free(vars[idx].value);
        for (int i = idx; i < count - 1; i++) {
            vars[i] = vars[i + 1];
        }
        count--;
    }
}

void render_v(const char *text) {
    size_t i = 0;
    size_t len = strlen(text);
    while (i < len) {
        if (text[i] == '{') {
            size_t j = i + 1;
            while (j < len && text[j] != '}') {
                j++;
            }
            if (j < len && text[j] == '}') {
                size_t k_len = j - i - 1;
                char *k = malloc(k_len + 1);
                strncpy(k, text + i + 1, k_len);
                k[k_len] = '\0';
                int idx = find_var(k);
                if (idx != -1) {
                    printf("%s", vars[idx].value);
                } else {
                    for (size_t m = i; m <= j; m++) {
                        putchar(text[m]);
                    }
                }
                free(k);
                i = j + 1;
            } else {
                putchar(text[i]);
                i++;
            }
        } else {
            putchar(text[i]);
            i++;
        }
    }
    putchar('\n');
}

void vars_v(void) {
    for (int i = 0; i < count; i++) {
        printf("%s=%s\n", vars[i].key, vars[i].value);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key && val) set_v(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_v(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = strtok(NULL, "");
            if (text) render_v(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_v();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
