// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Var {
    char* key;
    char* val;
    struct Var* next;
} Var;

Var* head = NULL;

Var* find_var(const char* key) {
    for (Var* v = head; v; v = v->next) {
        if (strcmp(v->key, key) == 0) return v;
    }
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "SET") == 0) {
            char* key = get_word(&p);
            Var* v = find_var(key);
            if (v) {
                free(v->val);
                v->val = strdup(p);
            } else {
                Var* n = malloc(sizeof(Var));
                n->key = strdup(key);
                n->val = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char* key = get_word(&p);
            Var* prev = NULL;
            for (Var* v = head; v; prev = v, v = v->next) {
                if (strcmp(v->key, key) == 0) {
                    if (prev) prev->next = v->next;
                    else head = v->next;
                    free(v->key);
                    free(v->val);
                    free(v);
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char out[MAX_LINE * 2] = {0};
            char* p_text = p;
            while (*p_text) {
                if (*p_text == '{') {
                    char* end = strchr(p_text, '}');
                    if (end) {
                        char k[256] = {0};
                        size_t klen = end - p_text - 1;
                        if (klen < sizeof(k)) {
                            strncpy(k, p_text + 1, klen);
                            Var* v = find_var(k);
                            if (v) strncat(out, v->val, sizeof(out) - strlen(out) - 1);
                            else {
                                size_t plen = end - p_text + 1;
                                strncat(out, p_text, plen < sizeof(out) - strlen(out) ? plen : sizeof(out) - strlen(out) - 1);
                            }
                        } else {
                            strncat(out, p_text, end - p_text + 1);
                        }
                        p_text = end + 1;
                    } else {
                        strncat(out, "{", sizeof(out) - strlen(out) - 1);
                        p_text++;
                    }
                } else {
                    char c[2] = {*p_text, 0};
                    strncat(out, c, sizeof(out) - strlen(out) - 1);
                    p_text++;
                }
            }
            printf("%s\n", out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (Var* v = head; v; v = v->next) {
                printf("%s %s\n", v->key, v->val);
            }
        }
    }
    return 0;
}