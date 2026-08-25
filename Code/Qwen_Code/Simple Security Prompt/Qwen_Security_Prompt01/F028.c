// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
int count = 0;

Stop *get_node(int idx) {
    Stop *p = head;
    for (int i = 0; i < idx && p; i++) p = p->next;
    return p;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!head) head = s;
    else {
        Stop *p = head;
        while (p->next) p = p->next;
        p->next = s;
    }
    count++;
}

void insert_stop(int idx, const char *name, const char *note) {
    if (idx < 0 || idx > count) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    if (idx == 0) {
        s->next = head;
        head = s;
    } else {
        Stop *p = get_node(idx - 1);
        s->next = p->next;
        p->next = s;
    }
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count || from == to) return;
    Stop *p = head, *prev = NULL;
    for (int i = 0; i < from; i++) { prev = p; p = p->next; }
    if (prev) prev->next = p->next;
    else head = p->next;
    
    Stop *ins = head, *ins_prev = NULL;
    for (int i = 0; i < to; i++) { ins_prev = ins; ins = ins->next; }
    if (ins_prev) {
        p->next = ins_prev->next;
        ins_prev->next = p;
    } else {
        p->next = head;
        head = p;
    }
}

void remove_stop(int idx) {
    if (idx < 0 || idx >= count) return;
    Stop *p = head, *prev = NULL;
    for (int i = 0; i < idx; i++) { prev = p; p = p->next; }
    if (prev) prev->next = p->next;
    else head = p->next;
    free(p->name); free(p->note); free(p);
    count--;
}

void find_stop(const char *name) {
    for (Stop *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
    }
}

void print_stops() {
    for (Stop *s = head; s; s = s->next) {
        printf("%s %s\n", s->name, s->note);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        int i, j;
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            add_stop(a, b);
        } else if (strcmp(cmd, "INSERT") == 0) {
            sscanf(line, "%*s %d %s %[^\n]", &i, a, b);
            insert_stop(i, a, b);
        } else if (strcmp(cmd, "MOVE") == 0) {
            sscanf(line, "%*s %d %d", &i, &j);
            move_stop(i, j);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %d", &i);
            remove_stop(i);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            find_stop(a);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    while (head) {
        Stop *n = head->next;
        free(head->name); free(head->note); free(head);
        head = n;
    }
    return 0;
}