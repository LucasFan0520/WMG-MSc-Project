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

Reminder *reminders = NULL;

void add_reminder(int priority, const char *title, const char *note) {
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = reminders;
    reminders = r;
}

void bump(const char *title, int delta) {
    for (Reminder *r = reminders; r; r = r->next) {
        if (strcmp(r->title, title) == 0) {
            r->priority += delta;
            return;
        }
    }
}

void remove_reminder(const char *title) {
    Reminder **rp = &reminders;
    while (*rp) {
        if (strcmp((*rp)->title, title) == 0) {
            Reminder *tmp = *rp;
            *rp = tmp->next;
            free(tmp->title);
            free(tmp->note);
            free(tmp);
            return;
        }
        rp = &(*rp)->next;
    }
}

int cmp_reminder(const void *a, const void *b) {
    Reminder *ra = *(Reminder **)a;
    Reminder *rb = *(Reminder **)b;
    return rb->priority - ra->priority;
}

void top_n(int n) {
    int count = 0;
    for (Reminder *r = reminders; r; r = r->next) count++;
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    Reminder *r = reminders;
    for (int i = 0; i < count; i++) {
        arr[i] = r;
        r = r->next;
    }
    qsort(arr, count, sizeof(Reminder *), cmp_reminder);
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders() {
    for (Reminder *r = reminders; r; r = r->next) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
    }
}

void cleanup() {
    while (reminders) {
        Reminder *tmp = reminders->next;
        free(reminders->title);
        free(reminders->note);
        free(reminders);
        reminders = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        int priority, delta;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s", &priority, a1) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) add_reminder(priority, a1, t + 1); } }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %255s %d", a1, &delta) == 2) bump(a1, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) remove_reminder(a1);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &priority) == 1) top_n(priority);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    cleanup();
    return 0;
}