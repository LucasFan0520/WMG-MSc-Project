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
    if (!r->title || !r->note) {
        free(r->title); free(r->note); free(r);
        return;
    }
    if (!head) head = r;
    else {
        Reminder *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
}

void bump_reminder(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
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

void top_reminders(int n) {
    // simple selection sort to get top n
    Reminder *cur = head;
    while (cur) {
        Reminder *max = cur;
        Reminder *walk = cur->next;
        while (walk) {
            if (walk->priority > max->priority) max = walk;
            walk = walk->next;
        }
        // swap cur and max
        if (max != cur) {
            int tmp_p = cur->priority;
            char *tmp_t = cur->title;
            char *tmp_n = cur->note;
            cur->priority = max->priority;
            cur->title = max->title;
            cur->note = max->note;
            max->priority = tmp_p;
            max->title = tmp_t;
            max->note = tmp_n;
        }
        cur = cur->next;
    }
    int count = 0;
    Reminder *r = head;
    while (r && count < n) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        count++;
        r = r->next;
    }
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
        Reminder *next = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], title[256], note[4096];
        int prio, delta, n;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &prio, title, note) == 3)
                add_reminder(prio, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &delta) == 2)
                bump_reminder(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1)
                top_reminders(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free(line);
    free_all();
    return 0;
}