/*
 * F007.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct activity {
    char *time;
    char *label;
    char *note;
    struct activity *next;
} Activity;

Activity *head = NULL, *tail = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (tail) tail->next = a;
    else head = a;
    tail = a;
}

void delete_activities(const char *time, const char *label) {
    Activity *prev = NULL, *a = head;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            if (a == tail) tail = prev;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            if (prev) a = prev->next;
            else a = head;
        } else {
            prev = a;
            a = a->next;
        }
    }
}

void find_label(const char *label) {
    Activity *a = head, *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0)
                earliest = a;
        }
        a = a->next;
    }
    if (!earliest) {
        printf("NOT_FOUND\n");
    } else {
        printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

int cmp_activity(const void *x, const void *y) {
    Activity **a = (Activity**)x, **b = (Activity**)y;
    int t = strcmp((*a)->time, (*b)->time);
    if (t != 0) return t;
    return strcmp((*a)->label, (*b)->label);
}

void sort_activities(void) {
    int n = 0;
    Activity *a = head;
    while (a) { n++; a = a->next; }
    if (n == 0) return;
    Activity **arr = malloc(sizeof(Activity*) * n);
    a = head;
    for (int i = 0; i < n; i++, a = a->next) arr[i] = a;
    qsort(arr, n, sizeof(Activity*), cmp_activity);
    for (int i = 0; i < n; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_activities(void) {
    Activity *a = head;
    while (a) {
        Activity *tmp = a;
        a = a->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
    head = tail = NULL;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], time[256], label[256];
        if (sscanf(line, "%31s %255s %255s", cmd, time, label) < 3) continue;
        char *rest = line + strlen(cmd) + strlen(time) + strlen(label) + 2;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            add_activity(time, label, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_activities(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_label(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    return 0;
}