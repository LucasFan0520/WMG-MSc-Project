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
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = head;
    head = r;
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *r = head, *prev = NULL;
    while (r && strcmp(r->title, title) != 0) {
        prev = r;
        r = r->next;
    }
    if (!r) return;
    if (prev) prev->next = r->next;
    else head = r->next;
    free(r->title);
    free(r->note);
    free(r);
}

void top_n(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    int take = n < count ? n : count;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    r = head;
    for (int i = 0; i < count; i++) {
        arr[i] = r;
        r = r->next;
    }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->priority < arr[j]->priority) {
                Reminder *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < take; i++) {
        printf("%d %s ", arr[i]->priority, arr[i]->title);
        for (char *c = arr[i]->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
    free(arr);
}

void list_all(void) {
    Reminder *r = head;
    while (r) {
        printf("%d %s ", r->priority, r->title);
        for (char *c = r->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
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
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            int priority;
            char title[256];
            char *p = line + 4;
            if (sscanf(p, "%d %255s", &priority, title) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_reminder(priority, title, p);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta;
            if (sscanf(line + 5, "%255s %d", title, &delta) == 2)
                bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_reminder(p);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            if (sscanf(line + 4, "%d", &n) == 1) top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    free_all();
    free(line);
    return 0;
}