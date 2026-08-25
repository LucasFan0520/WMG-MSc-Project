/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct reminder {
    int priority;
    char *title;
    char *note;
    struct reminder *next;
} reminder_t;

reminder_t *head = NULL;
reminder_t *tail = NULL;

reminder_t *find_reminder(const char *title) {
    reminder_t *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    reminder_t *r = malloc(sizeof(reminder_t));
    if (!r) return;
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = NULL;
    if (!r->title || !r->note) {
        free(r->title); free(r->note); free(r);
        return;
    }
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void bump_reminder(const char *title, int delta) {
    reminder_t *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    reminder_t *prev = NULL, *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int cmp_priority(const void *a, const void *b) {
    reminder_t *ra = *(reminder_t**)a;
    reminder_t *rb = *(reminder_t**)b;
    return rb->priority - ra->priority; // descending
}

void top_reminders(int n) {
    int count = 0;
    reminder_t *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    reminder_t **arr = malloc(sizeof(reminder_t*) * count);
    if (!arr) return;
    int i = 0;
    r = head;
    while (r) { arr[i++] = r; r = r->next; }
    qsort(arr, count, sizeof(reminder_t*), cmp_priority);
    int limit = n < count ? n : count;
    for (i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    reminder_t *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

void free_all(void) {
    reminder_t *r = head;
    while (r) {
        reminder_t *nxt = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], title[256];
        int pr, delta, n;
        if (sscanf(line, "%9s %d %255s", cmd, &pr, title) >= 2) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %d %255s", &pr, title) == 2) {
                    const char *note = line + strlen(cmd) + 1 + (rest - line - strlen(cmd) - 1) + 1;
                    while (*note == ' ') note++;
                    add_reminder(pr, title, note);
                }
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
    }
    free(line);
    free_all();
    return 0;
}