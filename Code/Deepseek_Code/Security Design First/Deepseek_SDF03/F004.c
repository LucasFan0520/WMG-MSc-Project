// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_KEY 64
#define MAX_VAL 4096

typedef struct Var {
    char key[MAX_KEY];
    char val[MAX_VAL];
    struct Var *next;
} Var;

Var *head = NULL;

Var *find_var(const char *key) {
    Var *v = head;
    while (v) {
        if (strcmp(v->key, key) == 0) return v;
        v = v->next;
    }
    return NULL;
}

void free_all() {
    Var *v = head;
    while (v) {
        Var *tmp = v;
        v = v->next;
        free(tmp);
    }
    head = NULL;
}

void handle_set(char *line) {
    char key[MAX_KEY], val[MAX_VAL];
    int n = sscanf(line, "SET %63s %4095[^\n]", key, val);
    if (n < 2) return;
    Var *v = find_var(key);
    if (v) {
        strcpy(v->val, val);
    } else {
        Var *newv = malloc(sizeof(Var));
        if (!newv) return;
        strcpy(newv->key, key);
        strcpy(newv->val, val);
        newv->next = head;
        head = newv;
    }
}

void handle_unset(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "UNSET %63s", key) != 1) return;
    Var *v = head, *prev = NULL;
    while (v) {
        if (strcmp(v->key, key) == 0) {
            if (prev) prev->next = v->next;
            else head = v->next;
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void handle_render(char *line) {
    char text[MAX_LINE];
    if (sscanf(line, "RENDER %4095[^\n]", text) != 1) return;
    char result[MAX_LINE * 2];
    result[0] = '\0';
    char *p = text;
    while (*p) {
        if (*p == '{') {
            char key[MAX_KEY];
            int i = 0;
            p++;
            while (*p && *p != '}' && i < MAX_KEY-1) {
                key[i++] = *p++;
            }
            key[i] = '\0';
            if (*p == '}') p++;
            Var *v = find_var(key);
            if (v) {
                strcat(result, v->val);
            } else {
                strcat(result, "{");
                strcat(result, key);
                strcat(result, "}");
            }
        } else {
            char tmp[2] = {*p, 0};
            strcat(result, tmp);
            p++;
        }
    }
    printf("%s\n", result);
}

void handle_vars() {
    Var *v = head;
    while (v) {
        printf("%s %s\n", v->key, v->val);
        v = v->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "SET ", 4) == 0) handle_set(line);
        else if (strncmp(line, "UNSET ", 6) == 0) handle_unset(line);
        else if (strncmp(line, "RENDER ", 7) == 0) handle_render(line);
        else if (strcmp(line, "VARS") == 0) handle_vars();
    }
    free_all();
    return 0;
}