// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TIME 32
#define MAX_LABEL 64
#define MAX_NOTE 4096

typedef struct Activity {
    char time[MAX_TIME];
    char label[MAX_LABEL];
    char note[MAX_NOTE];
    struct Activity *next;
} Activity;

Activity *head = NULL;

void free_all() {
    Activity *a = head;
    while (a) {
        Activity *tmp = a;
        a = a->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    char time[MAX_TIME], label[MAX_LABEL], note[MAX_NOTE];
    int n = sscanf(line, "ADD %31s %63s %4095[^\n]", time, label, note);
    if (n < 3) return;
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    strcpy(a->time, time);
    strcpy(a->label, label);
    strcpy(a->note, note);
    a->next = head;
    head = a;
}

void handle_delete(char *line) {
    char time[MAX_TIME], label[MAX_LABEL];
    if (sscanf(line, "DELETE %31s %63s", time, label) != 2) return;
    Activity *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a);
            a = prev ? prev->next : head;
        } else {
            prev = a;
            a = a->next;
        }
    }
}

void handle_find(char *line) {
    char label[MAX_LABEL];
    if (sscanf(line, "FIND %63s", label) != 1) return;
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
    if (earliest) {
        printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

int compare_activity(const void *p1, const void *p2) {
    Activity *a1 = *(Activity **)p1;
    Activity *a2 = *(Activity **)p2;
    int cmp = strcmp(a1->time, a2->time);
    if (cmp != 0) return cmp;
    return strcmp(a1->label, a2->label);
}

void handle_sort() {
    int count = 0;
    Activity *a = head;
    while (a) { count++; a = a->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    if (!arr) return;
    a = head;
    for (int i = 0; i < count; i++) {
        arr[i] = a;
        a = a->next;
    }
    qsort(arr, count, sizeof(Activity *), compare_activity);
    for (int i = 0; i < count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void handle_clear() {
    free_all();
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "SORT") == 0) handle_sort();
        else if (strcmp(line, "CLEAR") == 0) handle_clear();
    }
    free_all();
    return 0;
}