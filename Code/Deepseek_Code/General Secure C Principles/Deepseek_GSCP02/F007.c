// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Activity {
    char *time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *head = NULL;
Activity *tail = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (tail) {
        tail->next = a;
        tail = a;
    } else {
        head = tail = a;
    }
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL;
    Activity *a = head;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            if (a == tail) tail = prev;
            Activity *tofree = a;
            a = a->next;
            free(tofree->time);
            free(tofree->label);
            free(tofree->note);
            free(tofree);
            continue;
        }
        prev = a;
        a = a->next;
    }
}

void find_label(const char *label) {
    Activity *a = head;
    Activity *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0) {
                earliest = a;
            }
        }
        a = a->next;
    }
    if (!earliest) return;
    printf("%s %s ", earliest->time, earliest->label);
    for (char *p = earliest->note; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

int cmp_activity(const void *p1, const void *p2) {
    Activity *a1 = *(Activity **)p1;
    Activity *a2 = *(Activity **)p2;
    int t = strcmp(a1->time, a2->time);
    if (t != 0) return t;
    return strcmp(a1->label, a2->label);
}

void sort_activities(void) {
    int n = 0;
    Activity *a = head;
    while (a) { n++; a = a->next; }
    if (n == 0) return;
    Activity **arr = malloc(n * sizeof(Activity *));
    if (!arr) return;
    a = head;
    for (int i = 0; i < n; i++) {
        arr[i] = a;
        a = a->next;
    }
    qsort(arr, n, sizeof(Activity *), cmp_activity);
    for (int i = 0; i < n; i++) {
        printf("%s %s ", arr[i]->time, arr[i]->label);
        for (char *p = arr[i]->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
    }
    free(arr);
}

void clear_all(void) {
    while (head) {
        Activity *next = head->next;
        free(head->time);
        free(head->label);
        free(head->note);
        free(head);
        head = next;
    }
    tail = NULL;
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
            char time[256], label[256], note[1024];
            if (sscanf(line, "%*s %255s %255s %1023[^\n]", time, label, note) == 3) {
                add_activity(time, label, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time[256], label[256];
            if (sscanf(line, "%*s %255s %255s", time, label) == 2) {
                delete_activity(time, label);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[256];
            if (sscanf(line, "%*s %255s", label) == 1) {
                find_label(label);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }

    free(line);
    clear_all();
    return 0;
}