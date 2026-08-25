// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int priority; char *title; char *note; } Reminder;
Reminder *reminders = NULL;
int rc = 0, rcap = 0;

int find_reminder(const char *title) {
    for (int i = 0; i < rc; i++)
        if (strcmp(reminders[i].title, title) == 0) return i;
    return -1;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title) >= 0) return;
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; reminders = realloc(reminders, sizeof(Reminder) * rcap); }
    reminders[rc].priority = priority;
    reminders[rc].title = strdup(title);
    reminders[rc].note = strdup(note);
    rc++;
}

void bump_priority(const char *title, int delta) {
    int idx = find_reminder(title);
    if (idx < 0) return;
    reminders[idx].priority += delta;
}

void remove_reminder(const char *title) {
    int idx = find_reminder(title);
    if (idx < 0) return;
    free(reminders[idx].title);
    free(reminders[idx].note);
    for (int i = idx; i < rc - 1; i++) reminders[i] = reminders[i + 1];
    rc--;
}

int cmp_priority(const void *a, const void *b) {
    const Reminder *A = (const Reminder *)a, *B = (const Reminder *)b;
    return B->priority - A->priority;
}

void top_reminders(int n) {
    Reminder *sorted = malloc(sizeof(Reminder) * rc);
    memcpy(sorted, reminders, sizeof(Reminder) * rc);
    qsort(sorted, rc, sizeof(Reminder), cmp_priority);
    for (int i = 0; i < n && i < rc; i++)
        printf("%s %s\n", sorted[i].title, sorted[i].note);
    free(sorted);
}

void list_reminders(void) {
    for (int i = 0; i < rc; i++)
        printf("%s %s\n", reminders[i].title, reminders[i].note);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256], note[4096];
        int priority, n;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &priority, title, note) == 3)
                add_reminder(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &priority) == 2)
                bump_priority(title, priority);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1) top_reminders(n);
        } else if (strcmp(cmd, "LIST") == 0) list_reminders();
    }
    for (int i = 0; i < rc; i++) { free(reminders[i].title); free(reminders[i].note); }
    free(reminders);
    return 0;
}