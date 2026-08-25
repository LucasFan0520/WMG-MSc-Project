// F001.c
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

typedef struct Para {
    char* id;
    char* text;
    struct Para* next;
} Para;

Para* head = NULL;

Para* find_para(const char* id) {
    for (Para* p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) return p;
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
        
        if (strcmp(cmd, "NEW") == 0) {
            char* id = get_word(&p);
            if (id && !find_para(id)) {
                Para* n = malloc(sizeof(Para));
                n->id = strdup(id);
                n->text = strdup(p);
                n->next = NULL;
                if (!head) {
                    head = n;
                } else {
                    Para* tail = head;
                    while (tail->next) tail = tail->next;
                    tail->next = n;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char* id = get_word(&p);
            Para* n = find_para(id);
            if (n) {
                size_t len = strlen(n->text) + strlen(p) + 2;
                char* new_text = malloc(len);
                snprintf(new_text, len, "%s %s", n->text, p);
                free(n->text);
                n->text = new_text;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char* id = get_word(&p);
            Para* n = find_para(id);
            if (n) {
                free(n->text);
                n->text = strdup(p);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* id = get_word(&p);
            Para* prev = NULL;
            for (Para* n = head; n; prev = n, n = n->next) {
                if (strcmp(n->id, id) == 0) {
                    if (prev) prev->next = n->next;
                    else head = n->next;
                    free(n->id);
                    free(n->text);
                    free(n);
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char* id = get_word(&p);
            Para* n = find_para(id);
            if (n) print_underscore(n->text);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Para* n = head; n; n = n->next) {
                printf("%s\n", n->id);
            }
        }
    }
    return 0;
}