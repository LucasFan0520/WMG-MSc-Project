// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Var {
    char *key;
    char *value;
    struct Var *next;
} Var;

Var *vars = NULL;

Var *find_var(const char *key) {
    for (Var *v = vars; v; v = v->next) {
        if (strcmp(v->key, key) == 0) return v;
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
    Var *prev = NULL, *curr = vars;
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
    size_t cap = strlen(text) * 2 + 1;
    char *res = malloc(cap);
    size_t res_len = 0;
    
    for (size_t i = 0; i < strlen(text); ) {
        if (text[i] == '{') {
            size_t j = i + 1;
            while (j < strlen(text) && text[j] != '}') j++;
            if (j < strlen(text)) {
                char key[256] = {0};
                strncpy(key, text + i + 1, j - i - 1);
                Var *v = find_var(key);
                if (v) {
                    size_t vlen = strlen(v->value);
                    while (res_len + vlen + 1 > cap) {
                        cap *= 2;
                        res = realloc(res, cap);
                    }
                    strcpy(res + res_len, v->value);
                    res_len += vlen;
                    i = j + 1;
                    continue;
                }
            }
        }
        while (res_len + 2 > cap) {
            cap *= 2;
            res = realloc(res, cap);
        }
        res[res_len++] = text[i++];
    }
    res[res_len] = '\0';
    printf("%s\n", res);
    free(res);
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        sscanf(line, "%15s %255s", cmd, arg1);
        char *text = strchr(line, ' ');
        if (text) {
            text++;
            while (*text == ' ') text++;
            char *text2 = strchr(text, ' ');
            if (text2) {
                text2++;
                while (*text2 == ' ') text2++;
                text = text2;
            } else {
                text = NULL;
            }
        }
        
        if (strcmp(cmd, "SET") == 0 && text) {
            set_var(arg1, text);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_var(arg1);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *render_str = strchr(line, ' ');
            if (render_str) {
                render_str++;
                while (*render_str == ' ') render_str++;
                render_str = strchr(render_str, ' ');
                if (render_str) {
                    render_str++;
                    while (*render_str == ' ') render_str++;
                    render_text(render_str);
                }
            }
        } else if (strcmp(cmd, "VARS") == 0) {
            for (Var *v = vars; v; v = v->next) {
                printf("%s=%s\n", v->key, v->value);
            }
        }
        free(line);
    }
    
    Var *v = vars;
    while (v) {
        Var *next = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = next;
    }
    return 0;
}