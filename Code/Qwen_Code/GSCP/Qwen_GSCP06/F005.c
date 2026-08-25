// F005.c
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

typedef struct Line {
    char* text;
    struct Line* next;
} Line;

Line* head = NULL;

Line* get_node(int idx) {
    Line* n = head;
    for (int i = 0; i < idx && n; i++) n = n->next;
    return n;
}

int count_lines() {
    int c = 0;
    for (Line* n = head; n; n = n->next) c++;
    return c;
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
        
        if (strcmp(cmd, "INSERT") == 0) {
            char* idx_str = get_word(&p);
            int idx = atoi(idx_str);
            if (idx < 0 || idx > count_lines()) continue;
            Line* n = malloc(sizeof(Line));
            n->text = strdup(p);
            if (idx == 0) {
                n->next = head;
                head = n;
            } else {
                Line* prev = get_node(idx - 1);
                n->next = prev->next;
                prev->next = n;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            Line* n = malloc(sizeof(Line));
            n->text = strdup(p);
            n->next = NULL;
            if (!head) head = n;
            else {
                Line* tail = head;
                while (tail->next) tail = tail->next;
                tail->next = n;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* idx_str = get_word(&p);
            int idx = atoi(idx_str);
            if (idx < 0 || idx >= count_lines()) continue;
            if (idx == 0) {
                Line* temp = head;
                head = head->next;
                free(temp->text);
                free(temp);
            } else {
                Line* prev = get_node(idx - 1);
                Line* temp = prev->next;
                prev->next = temp->next;
                free(temp->text);
                free(temp);
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char* idx_str = get_word(&p);
            int idx = atoi(idx_str);
            Line* n = get_node(idx);
            if (n) {
                free(n->text);
                n->text = strdup(p);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Line* n = head; n; n = n->next) {
                print_underscore(n->text);
            }
        }
    }
    return 0;
}