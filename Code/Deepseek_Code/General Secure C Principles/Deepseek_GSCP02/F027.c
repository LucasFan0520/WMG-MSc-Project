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

Reminder *r_head = NULL;

Reminder *find_reminder(const char *title) {
    Reminder *r = r_head;
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
    r->next = r_head;
    r_head = r;
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL;
    Reminder *r = r_head;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else r_head = r->next;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int cmp_reminder(const void *p1, const void *p2) {
    Reminder *r1 = *(Reminder **)p1;
    Reminder *r2 = *(Reminder **)p2;
    return r2->priority - r1->priority; // descending
}

void top_n(int n) {
    int count = 0;
    Reminder *r = r_head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) return;
    r = r_head;
    for (int i = 0; i < count; i++) {
        arr[i] = r;
        r = r->next;
    }
    qsort(arr, count, sizeof(Reminder *), cmp_reminder);
    int limit = (n < count) ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s ", arr[i]->priority, arr[i]->title);
        for (char *p = arr[i]->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
    }
    free(arr);
}

void list_all(void) {
    Reminder *r = r_head;
    while (r) {
        printf("%d %s ", r->priority, r->title);
        for (char *p = r->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        r = r->next;
    }
}

void free_all(void) {
    while (r_head) {
        Reminder *next = r_head->next;
        free(r_head->title);
        free(r_head->note);
        free(r_head);
        r_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            int priority;
            char title[256], note[1024];
            if (sscanf(line, "%*s %d %255s %1023[^\n]", &priority, title, note) == 3) {
                add_reminder(priority, title, note);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta;
            if (sscanf(line, "%*s %255s %d", title, &delta) == 2) {
                bump_priority(title, delta);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                remove_reminder(title);
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            if (sscanf(line, "%*s %d", &n) == 1) {
                top_n(n);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }

    free(line);
    free_all();
    return 0;
}