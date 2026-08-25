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

Stop *get_stop(int index) {
    if (index < 0 || index >= stop_count) return NULL;
    Stop *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    return curr;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    if (!s->name || !s->note) { free(s->name); free(s->note); free(s); return; }
    s->next = NULL;
    if (!head) {
        head = s;
    } else {
        Stop *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = s;
    }
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    if (!s->name || !s->note) { free(s->name); free(s->note); free(s); return; }
    
    if (index == 0) {
        s->next = head;
        head = s;
    } else {
        Stop *prev = get_stop(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to > stop_count) return;
    if (from == to) return;
    
    Stop *curr = head;
    Stop *prev = NULL;
    for (int i = 0; i < from; i++) {
        prev = curr;
        curr = curr->next;
    }
    
    if (prev) prev->next = curr->next;
    else head = curr->next;
    curr->next = NULL;
    stop_count--;
    
    int new_to = to;
    if (from < to) new_to--;
    
    if (new_to == 0) {
        curr->next = head;
        head = curr;
    } else {
        Stop *p = get_stop(new_to - 1);
        curr->next = p->next;
        p->next = curr;
    }
    stop_count++;
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *curr = head;
    Stop *prev = NULL;
    for (int i = 0; i < index; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev) prev->next = curr->next;
    else head = curr->next;
    free(curr->name);
    free(curr->note);
    free(curr);
    stop_count--;
}

void find_stop(const char *name) {
    for (Stop *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
    }
}

void print_all() {
    for (Stop *s = head; s; s = s->next) {
        printf("%s %s\n", s->name, s->note);
    }
}

void free_all() {
    Stop *s = head;
    while (s) {
        Stop *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        int idx1 = 0, idx2 = 0;
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_stop(arg1, arg2);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %1023s %4095[^\n]", &idx1, arg1, arg2) == 3) {
                insert_stop(idx1, arg1, arg2);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %d %d", &idx1, &idx2) == 2) {
                move_stop(idx1, idx2);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %d", &idx1) == 1) {
                remove_stop(idx1);
            }
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_stop(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}