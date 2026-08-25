// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    struct Reminder *next;
    struct Reminder *insert_next; // for insertion order
} Reminder;

Reminder *head = NULL; // insertion order
Reminder *last = NULL;

Reminder *find_reminder(const char *title) {
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->insert_next;
    }
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = NULL;
    r->insert_next = NULL;
    if (!head) head = last = r;
    else {
        last->insert_next = r;
        last = r;
    }
    // insert into priority order list (next)
    Reminder *cur = head, *prev = NULL;
    while (cur && cur->priority >= priority) {
        prev = cur;
        cur = cur->next;
    }
    if (prev) {
        prev->next = r;
        r->next = cur;
    } else {
        r->next = head;
        head = r;
    }
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
    // reinsert in priority order
    // remove from next list
    Reminder *cur = head, *prev = NULL;
    while (cur) {
        if (cur == r) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // reinsert
    cur = head;
    prev = NULL;
    while (cur && cur->priority >= r->priority) {
        prev = cur;
        cur = cur->next;
    }
    if (prev) {
        prev->next = r;
        r->next = cur;
    } else {
        r->next = head;
        head = r;
    }
}

void remove_reminder(const char *title) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    // remove from insertion order (insert_next)
    Reminder *cur = head, *prev = NULL;
    while (cur) {
        if (cur == r) {
            if (prev) prev->insert_next = cur->insert_next;
            else head = cur->insert_next;
            if (cur == last) last = prev;
            break;
        }
        prev = cur;
        cur = cur->insert_next;
    }
    // remove from priority list
    cur = head;
    prev = NULL;
    while (cur) {
        if (cur == r) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(r->title);
    free(r->note);
    free(r);
}

void top_n(int n) {
    Reminder *r = head;
    int count = 0;
    while (r && count < n) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
        count++;
    }
}

void list_reminders(void) {
    Reminder *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->insert_next;
    }
}

void cleanup_reminders(void) {
    Reminder *r = head;
    while (r) {
        Reminder *tmp = r;
        r = r->insert_next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %2047s", cmd, arg1, arg2, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            int priority = atoi(arg1);
            char *title = arg2;
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(title) + 1;
            add_reminder(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0 && n >= 3) {
            int delta = atoi(arg2);
            bump_priority(arg1, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = atoi(arg1);
            top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    cleanup_reminders();
    return 0;
}