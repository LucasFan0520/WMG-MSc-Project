// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Act { int time; char *label, *note; struct Act *next; } Act;
Act *head = NULL;

void add_act(int time, const char *label, const char *note) {
    Act *a = malloc(sizeof(Act));
    a->time = time;
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (!head) head = a;
    else {
        Act *p = head;
        while (p->next) p = p->next;
        p->next = a;
    }
}

void delete_act(int time, const char *label) {
    Act **p = &head;
    while (*p) {
        if ((*p)->time == time && strcmp((*p)->label, label) == 0) {
            Act *tmp = *p;
            *p = tmp->next;
            free(tmp->label);
            free(tmp->note);
            free(tmp);
        } else {
            p = &(*p)->next;
        }
    }
}

void find_act(const char *label) {
    for (Act *a = head; a; a = a->next) {
        if (strcmp(a->label, label) == 0) {
            printf("%d %s %s\n", a->time, a->label, a->note);
            return;
        }
    }
}

int cmp_act(const void *a, const void *b) {
    Act *aa = *(Act **)a, *bb = *(Act **)b;
    if (aa->time != bb->time) return aa->time - bb->time;
    return strcmp(aa->label, bb->label);
}

void sort_act() {
    int n = 0;
    for (Act *a = head; a; a = a->next) n++;
    if (n == 0) return;
    Act **arr = malloc(n * sizeof(Act *));
    Act *p = head;
    for (int i = 0; i < n; i++) { arr[i] = p; p = p->next; }
    qsort(arr, n, sizeof(Act *), cmp_act);
    for (int i = 0; i < n - 1; i++) arr[i]->next = arr[i + 1];
    arr[n - 1]->next = NULL;
    head = arr[0];
    free(arr);
}

void clear_act() {
    while (head) {
        Act *tmp = head->next;
        free(head->label);
        free(head->note);
        free(head);
        head = tmp;
    }
}

void cleanup() { clear_act(); }

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], label[256], note[8192] = "";
        int time;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s", &time, label) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) strcpy(note, t + 1); } }
                add_act(time, label, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d %255s", &time, label) == 2) delete_act(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", label) == 1) find_act(label);
        } else if (strcmp(cmd, "SORT") == 0) sort_act();
        else if (strcmp(cmd, "CLEAR") == 0) clear_act();
    }
    cleanup();
    return 0;
}