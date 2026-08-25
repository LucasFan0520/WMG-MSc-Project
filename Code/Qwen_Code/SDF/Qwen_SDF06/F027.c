// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    struct Reminder *next;
} Reminder;

Reminder *head = NULL;

void add_reminder(int p, const char *title, const char *note) {
    Reminder *r = malloc(sizeof(Reminder));
    if (!r) return;
    r->priority = p;
    r->title = strdup(title);
    r->note = strdup(note);
    if (!r->title || !r->note) { free(r->title); free(r->note); free(r); return; }
    r->next = head;
    head = r;
}

void bump(const char *title, int delta) {
    for (Reminder *r = head; r; r = r->next) {
        if (strcmp(r->title, title) == 0) {
            r->priority += delta;
            return;
        }
    }
}

void remove_reminder(const char *title) {
    Reminder *curr = head;
    Reminder *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void top_n(int n) {
    Reminder *sorted = NULL;
    for (Reminder *r = head; r; r = r->next) {
        Reminder *copy = malloc(sizeof(Reminder));
        if (!copy) continue;
        copy->priority = r->priority;
        copy->title = r->title;
        copy->note = r->note;
        
        Reminder *curr = sorted;
        Reminder *prev = NULL;
        while (curr) {
            if (copy->priority > curr->priority) break;
            prev = curr;
            curr = curr->next;
        }
        copy->next = curr;
        if (prev) prev->next = copy;
        else sorted = copy;
    }
    
    int count = 0;
    for (Reminder *r = sorted; r && count < n; r = r->next, count++) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
    }
    
    Reminder *r = sorted;
    while (r) {
        Reminder *next = r->next;
        free(r);
        r = next;
    }
}

void list_all() {
    for (Reminder *r = head; r; r = r->next) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
    }
}

void free_all() {
    Reminder *r = head;
    while (r) {
        Reminder *next = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = next;
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
        int val = 0;
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %1023s %4095[^\n]", &val, arg1, arg2) == 3) {
                add_reminder(val, arg1, arg2);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %1023s %d", arg1, &val) == 2) {
                bump(arg1, val);
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &val) == 1) {
                top_n(val);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    free_all();
    return 0;
}