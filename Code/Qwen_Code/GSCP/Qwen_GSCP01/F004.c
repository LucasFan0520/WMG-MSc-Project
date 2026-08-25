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

void set_var(const char *key, const char *val) {
    Var *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = strdup(val);
    } else {
        v = malloc(sizeof(Var));
        if (!v) exit(EXIT_FAILURE);
        v->key = strdup(key);
        v->value = strdup(val);
        v->next = vars;
        vars = v;
    }
}

void unset_var(const char *key) {
    Var *curr = vars;
    Var *prev = NULL;
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
    char *out = malloc(8192);
    if (!out) exit(EXIT_FAILURE);
    size_t out_len = 0;
    
    for (size_t i = 0; text[i]; ) {
        if (text[i] == '{') {
            size_t j = i + 1;
            while (text[j] && text[j] != '}') j++;
            if (text[j] == '}') {
                char key[256] = {0};
                size_t key_len = j - i - 1;
                if (key_len < sizeof(key)) {
                    strncpy(key, text + i + 1, key_len);
                    Var *v = find_var(key);
                    if (v) {
                        size_t v_len = strlen(v->value);
                        memcpy(out + out_len, v->value, v_len);
                        out_len += v_len;
                        i = j + 1;
                        continue;
                    }
                }
            }
        }
        out[out_len++] = text[i++];
    }
    out[out_len] = '\0';
    printf("%s\n", out);
    free(out);
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
        
        char cmd[32] = {0};
        char key[256] = {0};
        
        sscanf(line, "%31s %255s", cmd, key);
        
        if (strcmp(cmd, "SET") == 0) {
            char *val_start = strchr(line, ' ');
            if (val_start) {
                val_start = strchr(val_start + 1, ' ');
                if (val_start) set_var(key, val_start + 1);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text_start = strchr(line, ' ');
            if (text_start) render_text(text_start + 1);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    free_all();
    return 0;
}