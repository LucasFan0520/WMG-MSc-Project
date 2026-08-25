// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key;
    char *value;
    struct Var *next;
} Var;

Var *vars = NULL;

Var *find_var(const char *key) {
    Var *curr = vars;
    while (curr) {
        if (strcmp(curr->key, key) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void set_var(const char *key, const char *value) {
    Var *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = strdup(value);
    } else {
        v = malloc(sizeof(Var));
        v->key = strdup(key);
        v->value = strdup(value);
        v->next = vars;
        vars = v;
    }
}

void unset_var(const char *key) {
    Var *curr = vars, *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else vars = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void render_text(const char *text) {
    char out[8192];
    int out_idx = 0;
    int i = 0;
    while (text[i] && out_idx < 8190) {
        if (text[i] == '{') {
            int j = i + 1;
            while (text[j] && text[j] != '}') j++;
            if (text[j] == '}') {
                char key[256];
                int len = j - i - 1;
                if (len < 255) {
                    strncpy(key, text + i + 1, len);
                    key[len] = 0;
                    Var *v = find_var(key);
                    if (v) {
                        int vlen = strlen(v->value);
                        if (out_idx + vlen < 8190) {
                            strcpy(out + out_idx, v->value);
                            out_idx += vlen;
                        }
                    } else {
                        strncpy(out + out_idx, text + i, len + 2);
                        out_idx += len + 2;
                    }
                    i = j + 1;
                    continue;
                }
            }
        }
        out[out_idx++] = text[i++];
    }
    out[out_idx] = 0;
    printf("%s\n", out);
}

void print_vars() {
    Var *curr = vars;
    while (curr) {
        printf("%s=%s\n", curr->key, curr->value);
        curr = curr->next;
    }
}

void free_all() {
    Var *curr = vars;
    while (curr) {
        Var *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], key[256], text[8192] = {0};
        int n = sscanf(line, "%31s %255s %8191[^\n]", cmd, key, text);
        
        if (strcmp(cmd, "SET") == 0 && n >= 2) set_var(key, text);
        else if (strcmp(cmd, "UNSET") == 0 && n >= 1) unset_var(key);
        else if (strcmp(cmd, "RENDER") == 0 && n >= 1) render_text(key); 
        else if (strcmp(cmd, "VARS") == 0) print_vars();
    }
    free_all();
    return 0;
}