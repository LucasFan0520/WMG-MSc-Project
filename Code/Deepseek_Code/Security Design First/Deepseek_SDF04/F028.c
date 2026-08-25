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

int count_stops(void) {
    int c = 0;
    Stop *s = head;
    while (s) { c++; s = s->next; }
    return c;
}

Stop *get_stop_at(int index) {
    Stop *s = head;
    while (s && index--) s = s->next;
    return s;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!s->name || !s->note) {
        free(s->name); free(s->note); free(s);
        return;
    }
    if (!head) head = s;
    else {
        Stop *last = head;
        while (last->next) last = last->next;
        last->next = s;
    }
}

void insert_stop(int index, const char *name, const char *note) {
    int n = count_stops();
    if (index < 0 || index > n) return;
    Stop *new = malloc(sizeof(Stop));
    if (!new) return;
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!new->name || !new->note) {
        free(new->name); free(new->note); free(new);
        return;
    }
    if (index == 0) {
        new->next = head;
        head = new;
    } else {
        Stop *prev = get_stop_at(index - 1);
        if (!prev) { free(new->name); free(new->note); free(new); return; }
        new->next = prev->next;
        prev->next = new;
    }
}

void move_stop(int from, int to) {
    int n = count_stops();
    if (from < 0 || from >= n || to < 0 || to >= n || from == to) return;
    Stop *prev_from = NULL, *prev_to = NULL;
    Stop *s = head;
    int i = 0;
    while (s) {
        if (i == from - 1) prev_from = s;
        if (i == to - 1) prev_to = s;
        s = s->next;
        i++;
    }
    Stop *node = get_stop_at(from);
    if (!node) return;
    // remove from
    if (prev_from) prev_from->next = node->next;
    else head = node->next;
    // insert at to
    if (to == 0) {
        node->next = head;
        head = node;
    } else {
        Stop *before = get_stop_at(to - 1);
        if (!before) { /* should not happen */ return; }
        node->next = before->next;
        before->next = node;
    }
}

void remove_stop(int index) {
    int n = count_stops();
    if (index < 0 || index >= n) return;
    Stop *s = head, *prev = NULL;
    while (s && index--) { prev = s; s = s->next; }
    if (!s) return;
    if (prev) prev->next = s->next;
    else head = s->next;
    free(s->name);
    free(s->note);
    free(s);
}

void find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
        s = s->next;
    }
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->name, s->note);
        s = s->next;
    }
}

void free_all(void) {
    Stop *s = head;
    while (s) {
        Stop *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], note[4096];
        int idx1, idx2;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                add_stop(name, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &idx1, name, note) == 3)
                insert_stop(idx1, name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %d %d", &idx1, &idx2) == 2)
                move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %d", &idx1) == 1)
                remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_stop(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free(line);
    free_all();
    return 0;
}