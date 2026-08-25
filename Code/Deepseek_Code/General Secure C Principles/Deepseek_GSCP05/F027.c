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
    if (!r) return;
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = reminders;
    reminders = r;
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL, *cur = reminders;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else reminders = cur->next;
            free(cur->title);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

int compare_reminder(const void *a, const void *b) {
    Reminder *ra = *(Reminder **)a;
    Reminder *rb = *(Reminder **)b;
    return rb->priority - ra->priority; // descending
}

void top_reminders(int n) {
    int count = 0;
    Reminder *cur = reminders;
    while (cur) { count++; cur = cur->next; }
    if (count == 0 || n <= 0) return;
    if (n > count) n = count;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) return;
    cur = reminders;
    for (int i = 0; i < count; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    qsort(arr, count, sizeof(Reminder *), compare_reminder);
    for (int i = 0; i < n; i++) {
        Reminder *r = arr[i];
        printf("%d %s: ", r->priority, r->title);
        char *s = r->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = reminders;
    while (r) {
        printf("%d %s: ", r->priority, r->title);
        char *s = r->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        r = r->next;
    }
}

void free_all(void) {
    Reminder *r = reminders;
    while (r) {
        Reminder *next = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            int priority;
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &priority) != 1) continue;
            char *title_start = strchr(p, ' ');
            if (!title_start) continue;
            while (*title_start == ' ') title_start++;
            char *title = title_start;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            add_reminder(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            int delta;
            if (sscanf(space + 1, "%d", &delta) != 1) continue;
            bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &n) == 1) top_reminders(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}