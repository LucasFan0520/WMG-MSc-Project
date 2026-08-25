// F011.c
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

typedef struct Draft {
    char* title;
    char* body;
    struct Draft* next;
} Draft;

Draft* head = NULL;
Draft* trash = NULL;

Draft* find_draft(const char* title) {
    for (Draft* d = head; d; d = d->next) {
        if (strcmp(d->title, title) == 0) return d;
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
        
        if (strcmp(cmd, "CREATE") == 0) {
            char* title = get_word(&p);
            Draft* n = malloc(sizeof(Draft));
            n->title = strdup(title);
            n->body = strdup(p);
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "REVISE") == 0) {
            char* title = get_word(&p);
            Draft* d = find_draft(title);
            if (d) {
                free(d->body);
                d->body = strdup(p);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char* title = get_word(&p);
            Draft* prev = NULL;
            for (Draft* d = head; d; prev = d, d = d->next) {
                if (strcmp(d->title, title) == 0) {
                    if (prev) prev->next = d->next;
                    else head = d->next;
                    if (trash) {
                        free(trash->title);
                        free(trash->body);
                    } else {
                        trash = malloc(sizeof(Draft));
                    }
                    trash->title = d->title;
                    trash->body = d->body;
                    trash->next = NULL;
                    free(d);
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (trash) {
                trash->next = head;
                head = trash;
                trash = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* title = get_word(&p);
            Draft* d = find_draft(title);
            if (d) printf("%s %s\n", d->title, d->body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Draft* d = head; d; d = d->next) {
                printf("%s %s\n", d->title, d->body);
            }
        }
    }
    return 0;
}