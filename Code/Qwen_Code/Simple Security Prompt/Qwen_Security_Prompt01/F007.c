// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Act {
    int time;
    char *label;
    char *note;
    struct Act *next;
} Act;

Act *head = NULL;

void add_act(int t, const char *l, const char *n) {
    Act *a = malloc(sizeof(Act));
    a->time = t;
    a->label = strdup(l);
    a->note = strdup(n);
    a->next = head;
    head = a;
}

void delete_act(const char *l) {
    Act *p = head;
    Act *prev = NULL;
    while (p) {
        if (strcmp(p->label, l) == 0) {
            Act *next = p->next;
            free(p->label);
            free(p->note);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            p = next;
        } else {
            prev = p;
            p = p->next;
        }
    }
}

void find_act(const char *l) {
    Act *best = NULL;
    for (Act *p = head; p; p = p->next) {
        if (strcmp(p->label, l) == 0 && (!best || p->time < best->time)) {
            best = p;
        }
    }
    if (best) {
        printf("%d %s %s\n", best->time, best->label, best->note);
    }
}

int cmp_act(const void *a, const void *b) {
    Act *x = *(Act **)a;
    Act *y = *(Act **)b;
    if (x->time != y->time) {
        return x->time - y->time;
    }
    return strcmp(x->label, y->label);
}

void sort_act() {
    int n = 0;
    for (Act *p = head; p; p = p->next) {
        n++;
    }
    Act **arr = malloc(n * sizeof(Act *));
    int i = 0;
    for (Act *p = head; p; p = p->next) {
        arr[i++] = p;
    }
    qsort(arr, n, sizeof(Act *), cmp_act);
    for (i = 0; i < n; i++) {
        printf("%d %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_act() {
    while (head) {
        Act *n = head->next;
        free(head->label);
        free(head->note);
        free(head);
        head = n;
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char label[1000];
        char note[8000];
        int t;
        note[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %d %s %[^\n]", &t, label, note);
            add_act(t, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", label);
            delete_act(label);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", label);
            find_act(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_act();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_act();
        }
    }
    clear_act();
    return 0;
}