/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct reminder {
    int priority;
    char *title;
    char *note;
    struct reminder *next;
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

void add_reminder(int prio, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = prio;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = head;
    head = r;
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
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

int cmp_reminder(const void *a, const void *b) {
    Reminder *ra = *(Reminder **)a, *rb = *(Reminder **)b;
    return rb->priority - ra->priority; // descending
}

void top_n(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    r = head;
    for (int i = 0; i < count; i++) { arr[i] = r; r = r->next; }
    qsort(arr, count, sizeof(Reminder *), cmp_reminder);
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s ", arr[i]->priority, arr[i]->title);
        for (char *c = arr[i]->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
    free(arr);
}

void list_reminders() {
    Reminder *r = head;
    while (r) {
        printf("%d %s ", r->priority, r->title);
        for (char *c = r->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        r = r->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], title[256], note[1024];
        int prio, delta, n;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_reminders(); continue; }
        if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1) top_n(n);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s", &prio, title) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + (prio >= 0 ? snprintf(NULL,0,"%d",prio) : 0) + 1 + strlen(title) + 1;
            if (*rest == ' ') rest++;
            add_reminder(prio, title, rest);
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &delta) == 2)
                bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) remove_reminder(title);
        }
    }
    return 0;
}