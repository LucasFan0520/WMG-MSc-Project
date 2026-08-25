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
int stop_count = 0;

Stop* get_stop(int index) {
    int i = 0;
    Stop *cur = head;
    while (cur && i < index) { cur = cur->next; i++; }
    return (i == index) ? cur : NULL;
}

void add_stop(const char *name, const char *note) {
    Stop *new = malloc(sizeof(Stop));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Stop *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *new = malloc(sizeof(Stop));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (index == 0) {
        new->next = head;
        head = new;
    } else {
        Stop *prev = get_stop(index - 1);
        if (!prev) { free(new->name); free(new->note); free(new); return; }
        new->next = prev->next;
        prev->next = new;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count) return;
    if (from == to) return;
    // Remove from 'from'
    Stop *prev = NULL, *cur = head;
    int i = 0;
    while (cur && i < from) { prev = cur; cur = cur->next; i++; }
    if (!cur) return;
    Stop *moving = cur;
    if (prev) prev->next = cur->next;
    else head = cur->next;
    // Insert at 'to'
    if (to == 0) {
        moving->next = head;
        head = moving;
    } else {
        Stop *before = get_stop(to - 1);
        if (!before) { // should not happen
            moving->next = head;
            head = moving;
        } else {
            moving->next = before->next;
            before->next = moving;
        }
    }
    // Note: stop_count unchanged.
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *prev = NULL, *cur = head;
    int i = 0;
    while (cur && i < index) { prev = cur; cur = cur->next; i++; }
    if (!cur) return;
    if (prev) prev->next = cur->next;
    else head = cur->next;
    free(cur->name);
    free(cur->note);
    free(cur);
    stop_count--;
}

void find_stop(const char *name) {
    Stop *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            printf("%s\n", cur->note);
            return;
        }
        cur = cur->next;
    }
}

void print_stops() {
    Stop *cur = head;
    while (cur) {
        printf("%s %s\n", cur->name, cur->note);
        cur = cur->next;
    }
}

void free_all() {
    Stop *cur = head;
    while (cur) {
        Stop *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_stop(name, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = strtok(NULL, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            insert_stop(idx, name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_str = strtok(NULL, " ");
            if (!from_str) continue;
            char *to_str = strtok(NULL, " ");
            if (!to_str) continue;
            int from = atoi(from_str);
            int to = atoi(to_str);
            move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_str = strtok(NULL, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            remove_stop(idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_stop(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}