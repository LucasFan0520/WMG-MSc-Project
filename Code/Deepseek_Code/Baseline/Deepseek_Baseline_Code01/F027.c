// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct reminder {
    int priority;
    char *title;
    char *note;
    struct reminder *next;
} Reminder;

Reminder *head = NULL;
Reminder *tail = NULL;

Reminder *find_reminder(const char *title) {
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void bump_reminder(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int cmp_priority(const void *a, const void *b) {
    Reminder *ra = *(Reminder**)a, *rb = *(Reminder**)b;
    return rb->priority - ra->priority; // descending
}

void top_n(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder*));
    r = head;
    for (int i = 0; i < count; i++, r = r->next) arr[i] = r;
    qsort(arr, count, sizeof(Reminder*), cmp_priority);
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        int p, n;
        char title[256], note[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s %[^\n]", &p, title, note) == 3)
                add_reminder(p, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &p) == 2)
                bump_reminder(title, p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1)
                top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    return 0;
}