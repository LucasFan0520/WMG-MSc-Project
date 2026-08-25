/*
 * F027.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct reminder {
    int priority;
    char *title;
    char *note;
    struct reminder *next;
} Reminder;

Reminder *head = NULL, *tail = NULL;

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
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL, *r = head;
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

int cmp_reminder(const void *a, const void *b) {
    Reminder *ra = *(Reminder**)a, *rb = *(Reminder**)b;
    return rb->priority - ra->priority; // higher priority first
}

void top_reminders(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    if (n > count) n = count;
    Reminder **arr = malloc(sizeof(Reminder*) * count);
    r = head;
    for (int i = 0; i < count; i++, r = r->next) arr[i] = r;
    qsort(arr, count, sizeof(Reminder*), cmp_reminder);
    for (int i = 0; i < n; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = head;
    while (r) {
        printf("%s %s\n", r->title, r->note);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256];
        if (sscanf(line, "%31s %255s %255s", cmd, arg1, arg2) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            int priority = atoi(arg1);
            char *title = rest;
            char *note = title + strlen(arg2) + 1;
            while (*note == ' ') note++;
            add_reminder(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            int delta = atoi(arg2);
            bump_reminder(arg1, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = atoi(arg1);
            top_reminders(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    return 0;
}