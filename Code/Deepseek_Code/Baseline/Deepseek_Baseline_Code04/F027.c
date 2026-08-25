/* F027.c */
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
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = reminders;
    reminders = r;
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
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

int compare_priority_desc(const void *a, const void *b) {
    Reminder **ra = (Reminder **)a;
    Reminder **rb = (Reminder **)b;
    return (*rb)->priority - (*ra)->priority;
}

void top_n(int n) {
    int count = 0;
    Reminder *r = reminders;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    r = reminders;
    for (int i = 0; i < count; i++) {
        arr[i] = r;
        r = r->next;
    }
    qsort(arr, count, sizeof(Reminder *), compare_priority_desc);
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%s %s\n", arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = reminders;
    while (r) {
        printf("%s\n", r->title);
        r = r->next;
    }
}

void free_reminders(void) {
    while (reminders) {
        Reminder *tmp = reminders;
        reminders = reminders->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char title[256];
        int priority, delta;
        int n = sscanf(line, "%s %d %s %d", cmd, &priority, title, &delta);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1;
            while (*p == ' ') p++;
            char *end = p;
            while (*end && *end != ' ') end++;
            if (!*end) continue;
            *end = '\0';
            int prio = atoi(p);
            p = end + 1;
            while (*p == ' ') p++;
            char *tit = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            if (*p) add_reminder(prio, tit, p);
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (n < 3) continue;
            bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (n < 2) continue;
            remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (n < 2) continue;
            top_n(priority);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_reminders();
    return 0;
}