// F036.c
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

typedef struct Rec {
    char* name;
    char* data;
    struct Rec* next;
} Rec;

Rec* head = NULL;

Rec* find_rec(const char* name) {
    for (Rec* r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
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

        if (strcmp(cmd, "ADD") == 0) {
            char* name = get_word(&p);
            char* payload = get_word(&p);
            if (name && payload) {
                char* colon = strchr(payload, ':');
                if (colon) {
                    *colon = '\0';
                    int len = atoi(payload);
                    char* data = colon + 1;
                    if ((int)strlen(data) == len) {
                        Rec* n = malloc(sizeof(Rec));
                        n->name = strdup(name);
                        n->data = strdup(data);
                        n->next = head;
                        head = n;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Rec* prev = NULL;
            for (Rec* r = head; r; prev = r, r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    if (prev) prev->next = r->next;
                    else head = r->next;
                    free(r->name);
                    free(r->data);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char* name = get_word(&p);
            Rec* r = find_rec(name);
            if (r) {
                for (const char* c = r->data; *c; c++) {
                    putchar(*c == ' ' ? '_' : *c);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec* r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    return 0;
}