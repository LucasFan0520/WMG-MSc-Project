// F028.c
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

typedef struct Stop {
    char* name;
    char* note;
    struct Stop* next;
} Stop;

Stop* head = NULL;

Stop* get_node(int idx) {
    Stop* n = head;
    for (int i = 0; i < idx && n; i++) n = n->next;
    return n;
}

int count_stops() {
    int c = 0;
    for (Stop* n = head; n; n = n->next) c++;
    return c;
}

Stop* find_stop(const char* name) {
    for (Stop* s = head; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
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
            if (name) {
                Stop* n = malloc(sizeof(Stop));
                n->name = strdup(name);
                n->note = strdup(p);
                n->next = NULL;
                if (!head) head = n;
                else {
                    Stop* tail = head;
                    while (tail->next) tail = tail->next;
                    tail->next = n;
                }
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char* idx_str = get_word(&p);
            char* name = get_word(&p);
            int idx = atoi(idx_str);
            if (idx < 0 || idx > count_stops() || !name) continue;
            Stop* n = malloc(sizeof(Stop));
            n->name = strdup(name);
            n->note = strdup(p);
            if (idx == 0) {
                n->next = head;
                head = n;
            } else {
                Stop* prev = get_node(idx - 1);
                n->next = prev->next;
                prev->next = n;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char* from_str = get_word(&p);
            char* to_str = get_word(&p);
            int from = atoi(from_str);
            int to = atoi(to_str);
            int total = count_stops();
            if (from < 0 || from >= total || to < 0 || to >= total || from == to) continue;
            Stop* prev_from = (from == 0) ? NULL : get_node(from - 1);
            Stop* node = prev_from ? prev_from->next : head;
            if (prev_from) prev_from->next = node->next;
            else head = node->next;
            
            Stop* prev_to = (to == 0) ? NULL : get_node(to > from ? to - 1 : to);
            if (prev_to) {
                node->next = prev_to->next;
                prev_to->next = node;
            } else {
                node->next = head;
                head = node;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char* idx_str = get_word(&p);
            int idx = atoi(idx_str);
            if (idx < 0 || idx >= count_stops()) continue;
            Stop* prev = (idx == 0) ? NULL : get_node(idx - 1);
            Stop* node = prev ? prev->next : head;
            if (prev) prev->next = node->next;
            else head = node->next;
            free(node->name);
            free(node->note);
            free(node);
        } else if (strcmp(cmd, "FIND") == 0) {
            char* name = get_word(&p);
            Stop* s = find_stop(name);
            if (s) printf("%s\n", s->note);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Stop* s = head; s; s = s->next) {
                printf("%s %s\n", s->name, s->note);
            }
        }
    }
    return 0;
}