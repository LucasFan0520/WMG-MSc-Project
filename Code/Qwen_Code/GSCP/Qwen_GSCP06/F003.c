// F003.c
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

typedef struct Alias {
    char* name;
    char* exp;
    struct Alias* next;
} Alias;

Alias* head = NULL;

Alias* find_alias(const char* name) {
    for (Alias* a = head; a; a = a->next) {
        if (strcmp(a->name, name) == 0) return a;
    }
    return NULL;
}

void print_underscore(const char* s) {
    for (const char* p = s; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "DEFINE") == 0) {
            char* name = get_word(&p);
            Alias* a = find_alias(name);
            if (a) {
                free(a->exp);
                a->exp = strdup(p);
            } else {
                Alias* n = malloc(sizeof(Alias));
                n->name = strdup(name);
                n->exp = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char* name = get_word(&p);
            Alias* a = find_alias(name);
            if (a) print_underscore(a->exp);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char* old = get_word(&p);
            char* new_name = p;
            Alias* a = find_alias(old);
            if (a) {
                free(a->name);
                a->name = strdup(new_name);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char* name = get_word(&p);
            Alias* prev = NULL;
            for (Alias* a = head; a; prev = a, a = a->next) {
                if (strcmp(a->name, name) == 0) {
                    if (prev) prev->next = a->next;
                    else head = a->next;
                    free(a->name);
                    free(a->exp);
                    free(a);
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Alias* a = head; a; a = a->next) {
                printf("%s %s\n", a->name, a->exp);
            }
        }
    }
    return 0;
}