// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Enc {
    char* name;
    char* data;
    struct Enc* next;
} Enc;

Enc* head = NULL;

Enc* find_enc(const char* name) {
    for (Enc* e = head; e; e = e->next)
        if (strcmp(e->name, name) == 0) return e;
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

        if (strcmp(cmd, "STORE") == 0) {
            char* name = get_word(&p);
            if (name) {
                Enc* n = malloc(sizeof(Enc));
                n->name = strdup(name);
                n->data = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char* name = get_word(&p);
            Enc* e = find_enc(name);
            if (e) {
                const char* s = e->data;
                int valid = 1;
                while (*s) {
                    if (!isdigit((unsigned char)*s)) {
                        valid = 0;
                        break;
                    }
                    long count = strtol(s, (char**)&s, 10);
                    if (count > 1000000 || count <= 0 || !*s) {
                        valid = 0;
                        break;
                    }
                    s++;
                }
                if (valid) {
                    s = e->data;
                    while (*s) {
                        long count = strtol(s, (char**)&s, 10);
                        char c = *s++;
                        for (long i = 0; i < count; i++) {
                            putchar(c == ' ' ? '_' : c);
                        }
                    }
                    putchar('\n');
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Enc* prev = NULL;
            for (Enc* e = head; e; prev = e, e = e->next) {
                if (strcmp(e->name, name) == 0) {
                    if (prev) prev->next = e->next;
                    else head = e->next;
                    free(e->name);
                    free(e->data);
                    free(e);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Enc* e = head; e; e = e->next) {
                printf("%s\n", e->name);
            }
        }
    }
    return 0;
}