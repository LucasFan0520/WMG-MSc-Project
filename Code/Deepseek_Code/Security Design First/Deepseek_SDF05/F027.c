// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    struct Reminder *next;
} Reminder;

Reminder *reminders = NULL;
Reminder *insertion_tail = NULL;

Reminder *find_reminder(const char *title) {
    Reminder *r = reminders;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    if (!r) return;
    r->priority = priority;
    r->title = safe_strdup(title);
    r->note = safe_strdup(note);
    r->next = NULL;
    if (insertion_tail) {
        insertion_tail->next = r;
        insertion_tail = r;
    } else {
        reminders = insertion_tail = r;
    }
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
            if (cur == insertion_tail) insertion_tail = prev;
            free(cur->title);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void top_n(int n) {
    // Count reminders
    int count = 0;
    Reminder *cur = reminders;
    while (cur) { count++; cur = cur->next; }
    if (count == 0) return;
    // Create array of pointers
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) return;
    cur = reminders;
    for (int i = 0; i < count; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    // Sort by priority descending (stable not needed)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->priority < arr[j]->priority) {
                Reminder *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    int limit = (n < count) ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *cur = reminders;
    while (cur) {
        printf("%d %s %s\n", cur->priority, cur->title, cur->note);
        cur = cur->next;
    }
}

void free_all(void) {
    Reminder *cur = reminders;
    while (cur) {
        Reminder *next = cur->next;
        free(cur->title);
        free(cur->note);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            int priority = atoi(arg1);
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            add_reminder(priority, arg2, p);
        } else if (strcmp(cmd, "BUMP") == 0 && n >= 3) {
            int delta = atoi(arg2);
            bump_priority(arg1, delta);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0 && n >= 2) {
            int ntop = atoi(arg1);
            top_n(ntop);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}