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
    if (!r) return;
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
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
    Reminder *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void top_reminders(int n) {
    // collect all reminders
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) return;
    r = head;
    for (int i = 0; i < count; i++) {
        arr[i] = r;
        r = r->next;
    }
    // sort by priority descending
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->priority < arr[j]->priority) {
                Reminder *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

void free_all(void) {
    Reminder *r = head;
    while (r) {
        Reminder *tmp = r;
        r = r->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            int priority;
            char title[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %255s", &priority, title) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    add_reminder(priority, title, note);
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta;
            if (sscanf(line + 5, "%255s %d", title, &delta) == 2) {
                bump_priority(title, delta);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_reminder(p);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            if (sscanf(line + 4, "%d", &n) == 1) {
                top_reminders(n);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}