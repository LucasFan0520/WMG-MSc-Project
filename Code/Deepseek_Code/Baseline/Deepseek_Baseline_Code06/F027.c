/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    char *title;
    char *note;
    int priority;
    int order;
    struct Reminder *next;
} Reminder;

Reminder *head = NULL;
int next_order = 0;

Reminder* find_reminder(const char *title) {
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
    r->title = strdup(title);
    r->note = strdup(note);
    r->priority = priority;
    r->order = next_order++;
    r->next = head;
    head = r;
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
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int compare_reminder(const void *a, const void *b) {
    const Reminder *ra = *(const Reminder**)a;
    const Reminder *rb = *(const Reminder**)b;
    if (ra->priority != rb->priority) return rb->priority - ra->priority;
    return ra->order - rb->order;
}

void top_reminders(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    if (n > count) n = count;
    Reminder **arr = malloc(count * sizeof(Reminder*));
    r = head;
    for (int i = 0; i < count; i++) { arr[i] = r; r = r->next; }
    qsort(arr, count, sizeof(Reminder*), compare_reminder);
    for (int i = 0; i < n; i++) {
        printf("%s %s\n", arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders() {
    Reminder *r = head;
    while (r) {
        printf("%s %s\n", r->title, r->note);
        r = r->next;
    }
}

void free_all() {
    while (head) {
        Reminder *tmp = head;
        head = head->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], note[4096];
        int priority, delta, n;
        int m = sscanf(line, "%31s %d %255s %[^\n]", cmd, &priority, title, note);
        if (strcmp(cmd, "ADD") == 0 && m >= 3) {
            add_reminder(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char t[256]; int d;
            if (sscanf(line, "%*s %255s %d", t, &d) == 2) bump_reminder(t, d);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char t[256];
            if (sscanf(line, "%*s %255s", t) == 1) remove_reminder(t);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1) top_reminders(n);
        } else if (strcmp(cmd, "LIST") == 0 && m == 1) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}