// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder { int priority; char *title; char *note; struct Reminder *next; } Reminder;
Reminder *reminders = NULL;

Reminder *find_reminder(const char *title) {
    for (Reminder *r = reminders; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    if (!r) return;
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = reminders;
    reminders = r;
}

void bump_reminder(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL;
    for (Reminder *r = reminders; r; prev = r, r = r->next) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next; else reminders = r->next;
            free(r->title); free(r->note); free(r);
            return;
        }
    }
}

void top_n(int n) {
    // collect all, sort by priority desc, then by insertion order (original list order)
    int count = 0;
    for (Reminder *r = reminders; r; r = r->next) count++;
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) return;
    int i = 0;
    for (Reminder *r = reminders; r; r = r->next) arr[i++] = r;
    // simple insertion sort by priority descending (stable)
    for (int i = 1; i < count; i++) {
        Reminder *key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j]->priority < key->priority) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
    int limit = (n < count) ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s ", arr[i]->priority, arr[i]->title);
        for (char *c = arr[i]->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
    free(arr);
}

void list_reminders(void) {
    for (Reminder *r = reminders; r; r = r->next)
        printf("%d %s %s\n", r->priority, r->title, r->note);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            int priority; char title[256], note[4096];
            if (sscanf(rest, "%d %255s", &priority, title) == 2) {
                char *n = rest;
                while (*n == ' ') n++;
                while (*n >= '0' && *n <= '9') n++;
                while (*n == ' ') n++;
                while (*n && *n != ' ') n++; // skip title
                while (*n == ' ') n++;
                strcpy(note, n);
                add_reminder(priority, title, note);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256]; int delta;
            if (sscanf(rest, "%255s %d", title, &delta) == 2)
                bump_reminder(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            if (sscanf(rest, "%d", &n) == 1) top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    while (reminders) { Reminder *r = reminders; reminders = r->next; free(r->title); free(r->note); free(r); }
    return 0;
}