// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct reminder {
    int priority;
    char *title;
    char *note;
    struct reminder *next;
};

struct reminder *reminders = NULL;
struct reminder *tail = NULL;

void add_reminder(int priority, char *title, char *note) {
    struct reminder *r = malloc(sizeof(struct reminder));
    r->priority = priority;
    r->title = malloc(strlen(title) + 1);
    strcpy(r->title, title);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->next = NULL;
    if (tail) {
        tail->next = r;
        tail = r;
    } else {
        reminders = tail = r;
    }
}

struct reminder *find_reminder(char *title) {
    struct reminder *r = reminders;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void bump_priority(char *title, int delta) {
    struct reminder *r = find_reminder(title);
    if (r) r->priority += delta;
}

void remove_reminder(char *title) {
    struct reminder *r = reminders, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else reminders = r->next;
            if (tail == r) tail = prev;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int compare_priority(const void *a, const void *b) {
    struct reminder *ra = *(struct reminder **)a;
    struct reminder *rb = *(struct reminder **)b;
    return rb->priority - ra->priority; // descending
}

void top_reminders(int n) {
    int count = 0;
    struct reminder *r = reminders;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    int take = n < count ? n : count;
    struct reminder **arr = malloc(count * sizeof(struct reminder *));
    r = reminders;
    for (int i = 0; i < count; i++) {
        arr[i] = r;
        r = r->next;
    }
    qsort(arr, count, sizeof(struct reminder *), compare_priority);
    for (int i = 0; i < take; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders() {
    struct reminder *r = reminders;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        int priority, delta, n;
        char title[256], note[4096];
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%31s %d %255s %4095[^\n]", cmd, &priority, title, note) >= 3) {
                add_reminder(priority, title, note);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%31s %255s %d", cmd, title, &delta) >= 3) {
                bump_priority(title, delta);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%31s %255s", cmd, title) >= 2) {
                remove_reminder(title);
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%31s %d", cmd, &n) >= 2) {
                top_reminders(n);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    return 0;
}