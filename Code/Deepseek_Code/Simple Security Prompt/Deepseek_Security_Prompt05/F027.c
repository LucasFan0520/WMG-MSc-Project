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

Reminder *head = NULL, *tail = NULL;

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
    if (!r) return;
    r->priority = prio;
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
    Reminder *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (!head) tail = NULL;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

int compare_reminder(const void *a, const void *b) {
    Reminder *ra = *(Reminder**)a, *rb = *(Reminder**)b;
    return rb->priority - ra->priority;
}

void top_n(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    int take = (n < count) ? n : count;
    Reminder **arr = malloc(count * sizeof(Reminder*));
    if (!arr) return;
    int i = 0;
    r = head;
    while (r) { arr[i++] = r; r = r->next; }
    qsort(arr, count, sizeof(Reminder*), compare_reminder);
    for (i = 0; i < take; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_all(void) {
    Reminder *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            int prio = atoi(arg1);
            char *note = line + strlen(cmd) + strlen(arg1) + strlen(arg2) + 3;
            while (*note == ' ') note++;
            add_reminder(prio, arg2, note);
        } else if (strcmp(cmd, "BUMP") == 0 && n >= 3) {
            bump_priority(arg1, atoi(arg2));
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0 && n >= 2) {
            top_n(atoi(arg1));
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    while (head) {
        Reminder *tmp = head;
        head = head->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
    return 0;
}