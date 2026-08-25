/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    struct Reminder *next;
} Reminder;

Reminder *reminders = NULL;
int reminder_count = 0;

Reminder *find_reminder(const char *title) {
    Reminder *r = reminders;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_reminder(Reminder *r) {
    free(r->title);
    free(r->note);
    free(r);
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = reminders;
    reminders = r;
    reminder_count++;
}

void bump_reminder(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL, *curr = reminders;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else reminders = curr->next;
            free_reminder(curr);
            reminder_count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int compare_priority(const void *a, const void *b) {
    Reminder *A = *(Reminder **)a;
    Reminder *B = *(Reminder **)b;
    return B->priority - A->priority;
}

void top_n(int n) {
    if (reminder_count == 0 || n <= 0) return;
    if (n > reminder_count) n = reminder_count;
    Reminder **arr = malloc(reminder_count * sizeof(Reminder *));
    Reminder *r = reminders;
    int i = 0;
    while (r) {
        arr[i++] = r;
        r = r->next;
    }
    qsort(arr, reminder_count, sizeof(Reminder *), compare_priority);
    for (i = 0; i < n; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = reminders;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    int priority, delta, n;
    char title[256], note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %255s", &priority, title) != 2) continue;
            p += (priority >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            p += strlen(title);
            while (*p == ' ') p++;
            add_reminder(priority, title, p);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", title, &delta) == 2) bump_reminder(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &n) == 1) top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }

    while (reminders) {
        Reminder *tmp = reminders;
        reminders = reminders->next;
        free_reminder(tmp);
    }
    return 0;
}