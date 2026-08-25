// F002.c
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

typedef struct Msg {
    char* tag;
    char* msg;
    struct Msg* next;
} Msg;

Msg* head = NULL;

Msg* find_first(const char* tag) {
    for (Msg* m = head; m; m = m->next) {
        if (strcmp(m->tag, tag) == 0) return m;
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
        
        if (strcmp(cmd, "ADD") == 0) {
            char* tag = get_word(&p);
            Msg* n = malloc(sizeof(Msg));
            n->tag = strdup(tag);
            n->msg = strdup(p);
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char* tag1 = get_word(&p);
            char* tag2 = get_word(&p);
            char* newtag = p;
            Msg* m1 = find_first(tag1);
            Msg* m2 = find_first(tag2);
            if (m1 && m2) {
                size_t len = strlen(m1->msg) + strlen(m2->msg) + 2;
                char* combined = malloc(len);
                snprintf(combined, len, "%s %s", m1->msg, m2->msg);
                Msg* n = malloc(sizeof(Msg));
                n->tag = strdup(newtag);
                n->msg = combined;
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char* tag = get_word(&p);
            Msg* prev = NULL;
            for (Msg* m = head; m; ) {
                Msg* next = m->next;
                if (strcmp(m->tag, tag) == 0) {
                    if (prev) prev->next = next;
                    else head = next;
                    free(m->tag);
                    free(m->msg);
                    free(m);
                } else {
                    prev = m;
                }
                m = next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* tag = get_word(&p);
            Msg* m = find_first(tag);
            if (m) print_underscore(m->msg);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Msg* m = head; m; m = m->next) {
                printf("%s %s\n", m->tag, m->msg);
            }
        }
    }
    return 0;
}