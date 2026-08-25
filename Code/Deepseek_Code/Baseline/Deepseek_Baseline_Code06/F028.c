/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;

Stop* find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

int count_stops() {
    int c = 0;
    Stop *s = head;
    while (s) { c++; s = s->next; }
    return c;
}

Stop* get_stop_at(int index) {
    int c = 0;
    Stop *s = head;
    while (s) {
        if (c == index) return s;
        c++;
        s = s->next;
    }
    return NULL;
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!head) head = s;
    else {
        Stop *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = s;
    }
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > count_stops()) return;
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
    } else {
        Stop *prev = get_stop_at(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
}

void move_stop(int from, int to) {
    int len = count_stops();
    if (from < 0 || from >= len || to < 0 || to >= len) return;
    if (from == to) return;
    Stop *prev_from = NULL, *from_node = head;
    for (int i = 0; i < from; i++) { prev_from = from_node; from_node = from_node->next; }
    if (prev_from) prev_from->next = from_node->next;
    else head = from_node->next;
    from_node->next = NULL;
    if (to == 0) {
        from_node->next = head;
        head = from_node;
    } else {
        Stop *prev_to = get_stop_at(to - 1);
        from_node->next = prev_to->next;
        prev_to->next = from_node;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= count_stops()) return;
    Stop *s = head, *prev = NULL;
    for (int i = 0; i < index; i++) { prev = s; s = s->next; }
    if (prev) prev->next = s->next;
    else head = s->next;
    free(s->name);
    free(s->note);
    free(s);
}

void find_stop_print(const char *name) {
    Stop *s = find_stop(name);
    if (s) printf("%s\n", s->note);
}

void print_stops() {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->name, s->note);
        s = s->next;
    }
}

void free_all() {
    while (head) {
        Stop *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[4096];
        int index, idx2;
        int n = sscanf(line, "%31s %d %255s %[^\n]", cmd, &index, a, b);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_stop(a, b);
        } else if (strcmp(cmd, "INSERT") == 0 && n >= 4) {
            insert_stop(index, a, b);
        } else if (strcmp(cmd, "MOVE") == 0 && n >= 3) {
            int from, to;
            if (sscanf(line, "%*s %d %d", &from, &to) == 2) move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            int idx;
            if (sscanf(line, "%*s %d", &idx) == 1) remove_stop(idx);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_stop_print(a);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_stops();
        }
    }
    free_all();
    return 0;
}