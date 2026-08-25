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
int count = 0;

Reminder *find_reminder(const char *title) {
    Reminder *r = reminders;
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
    r->next = reminders;
    reminders = r;
    count++;
}

void bump_reminder(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL;
    Reminder *r = reminders;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else reminders = r->next;
            free(r->title);
            free(r->note);
            free(r);
            count--;
            return;
        }
        prev = r;
        r = r->next;
    }
}

int compare_priority(const void *a, const void *b) {
    Reminder *ra = *(Reminder **)a;
    Reminder *rb = *(Reminder **)b;
    return rb->priority - ra->priority;
}

void top_n(int n) {
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    Reminder *r = reminders;
    int i = 0;
    while (r) {
        arr[i++] = r;
        r = r->next;
    }
    qsort(arr, count, sizeof(Reminder *), compare_priority);
    int limit = n < count ? n : count;
    for (i = 0; i < limit; i++) {
        printf("%d %s:", arr[i]->priority, arr[i]->title);
        for (char *s = arr[i]->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
    free(arr);
}

void list_reminders() {
    Reminder *r = reminders;
    while (r) {
        printf("%d %s:", r->priority, r->title);
        for (char *s = r->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        r = r->next;
    }
}

void free_all() {
    Reminder *r = reminders;
    while (r) {
        Reminder *next = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            int pri;
            char title[256], note[4096];
            if (sscanf(rest, "%d %255s %[^\n]", &pri, title, note) == 3) {
                add_reminder(pri, title, note);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta;
            if (sscanf(rest, "%255s %d", title, &delta) == 2) {
                bump_reminder(title, delta);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_reminder(rest);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            if (sscanf(rest, "%d", &n) == 1) {
                top_n(n);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}