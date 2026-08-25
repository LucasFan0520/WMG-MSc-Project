// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key;
    char *value;
    struct Var *next;
} Var;

Var *head = NULL;

void set_var(const char *key, const char *value) {
    for (Var *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->key, key) == 0) {
            free(curr->value);
            curr->value = strdup(value);
            return;
        }
    }
    Var *v = malloc(sizeof(Var));
    if (!v) return;
    v->key = strdup(key);
    v->value = strdup(value);
    v->next = head;
    head = v;
}

void unset_var(const char *key) {
    Var *curr = head;
    Var *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            Var *next = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void render_text(const char *text) {
    size_t len = strlen(text);
    for (size_t i = 0; i < len; i++) {
        if (text[i] == '{') {
            size_t j = i + 1;
            while (j < len && text[j] != '}') j++;
            if (j < len) {
                char key[1024];
                size_t klen = j - i - 1;
                if (klen < sizeof(key)) {
                    strncpy(key, text + i + 1, klen);
                    key[klen] = '\0';
                    int found = 0;
                    for (Var *curr = head; curr; curr = curr->next) {
                        if (strcmp(curr->key, key) == 0) {
                            printf("%s", curr->value);
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        i = j;
                        continue;
                    }
                }
            }
        }
        putchar(text[i]);
    }
    putchar('\n');
}

void vars() {
    for (Var *curr = head; curr; curr = curr->next) {
        printf("%s %s\n", curr->key, curr->value);
    }
}

void free_all() {
    Var *curr = head;
    while (curr) {
        Var *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char arg1[1024], arg2[4096];
        
        int n = sscanf(line, "%15s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_var(arg1, arg2);
        } else if (strcmp(cmd, "UNSET") == 0 && n >= 2) {
            unset_var(arg1);
        } else if (strcmp(cmd, "RENDER") == 0 && n >= 2) {
            render_text(arg2);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars();
        }
    }
    free_all();
    return 0;
}