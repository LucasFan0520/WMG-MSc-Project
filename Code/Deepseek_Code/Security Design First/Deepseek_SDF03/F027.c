// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TITLE 64
#define MAX_NOTE 4096
#define MAX_PRIORITY 1000

typedef struct Reminder {
    int priority;
    char title[MAX_TITLE];
    char note[MAX_NOTE];
    struct Reminder *next;
} Reminder;

Reminder *head = NULL;

Reminder *find_reminder(const char *title) {
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_all() {
    Reminder *r = head;
    while (r) {
        Reminder *tmp = r;
        r = r->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    int prio;
    char title[MAX_TITLE], note[MAX_NOTE];
    int n = sscanf(line, "ADD %d %63s %4095[^\n]", &prio, title, note);
    if (n < 3) return;
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    if (!r) return;
    r->priority = prio;
    strcpy(r->title, title);
    strcpy(r->note, note);
    r->next = head;
    head = r;
}

void handle_bump(char *line) {
    char title[MAX_TITLE];
    int delta;
    if (sscanf(line, "BUMP %63s %d", title, &delta) != 2) return;
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void handle_remove(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "REMOVE %63s", title) != 1) return;
    Reminder *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int compare_priority(const void *a, const void *b) {
    Reminder **ra = (Reminder **)a;
    Reminder **rb = (Reminder **)b;
    return (*rb)->priority - (*ra)->priority; // descending
}

void handle_top(char *line) {
    int n;
    if (sscanf(line, "TOP %d", &n) != 1) return;
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    int take = n < count ? n : count;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) return;
    r = head;
    for (int i = 0; i < count; i++) {
        arr[i] = r;
        r = r->next;
    }
    qsort(arr, count, sizeof(Reminder *), compare_priority);
    for (int i = 0; i < take; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void handle_list() {
    Reminder *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "BUMP ", 5) == 0) handle_bump(line);
        else if (strncmp(line, "REMOVE ", 7) == 0) handle_remove(line);
        else if (strncmp(line, "TOP ", 4) == 0) handle_top(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}