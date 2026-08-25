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
    r->title = malloc(strlen(title)+1);
    strcpy(r->title, title);
    r->note = malloc(strlen(note)+1);
    strcpy(r->note, note);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL;
    Reminder *r = head;
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

int compare_reminder(const void *a, const void *b) {
    Reminder *ra = *(Reminder**)a;
    Reminder *rb = *(Reminder**)b;
    return rb->priority - ra->priority; // descending
}

void top_n(int n) {
    if (!head) return;
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (n > count) n = count;
    Reminder **arr = malloc(count * sizeof(Reminder*));
    r = head;
    for (int i=0; i<count; i++) { arr[i] = r; r = r->next; }
    qsort(arr, count, sizeof(Reminder*), compare_reminder);
    for (int i=0; i<n; i++) {
        printf("%s %s\n", arr[i]->title, arr[i]->note);
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

void free_all(void) {
    Reminder *r = head;
    while (r) {
        Reminder *next = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *prio_str = strtok(rest, " ");
            if (!prio_str) continue;
            int prio = atoi(prio_str);
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_reminder(prio, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = strtok(rest, " ");
            if (!title) continue;
            char *delta_str = strtok(NULL, " ");
            if (!delta_str) continue;
            int delta = atoi(delta_str);
            bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(rest, " ");
            if (title) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_str = strtok(rest, " ");
            if (n_str) top_n(atoi(n_str));
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}