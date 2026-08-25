// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rem {
    int priority;
    char *title;
    char *note;
    struct Rem *next;
} Rem;

Rem *head = NULL;

void add_rem(int p, const char *t, const char *n) {
    Rem *r = malloc(sizeof(Rem));
    r->priority = p;
    r->title = strdup(t);
    r->note = strdup(n);
    r->next = head;
    head = r;
}

void bump_rem(const char *t, int d) {
    for (Rem *r = head; r; r = r->next) {
        if (strcmp(r->title, t) == 0) {
            r->priority += d;
            return;
        }
    }
}

void remove_rem(const char *t) {
    Rem *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->title, t) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->title); free(p->note); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

int cmp_rem(const void *a, const void *b) {
    Rem *x = *(Rem **)a;
    Rem *y = *(Rem **)b;
    return y->priority - x->priority;
}

void top_rem(int n) {
    int count = 0;
    for (Rem *r = head; r; r = r->next) count++;
    if (count == 0) return;
    Rem **arr = malloc(count * sizeof(Rem *));
    int i = 0;
    for (Rem *r = head; r; r = r->next) arr[i++] = r;
    qsort(arr, count, sizeof(Rem *), cmp_rem);
    int limit = n < count ? n : count;
    for (i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_rem() {
    for (Rem *r = head; r; r = r->next) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        int p;
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %d %s %[^\n]", &p, a, b);
            add_rem(p, a, b);
        } else if (strcmp(cmd, "BUMP") == 0) {
            sscanf(line, "%*s %s %d", a, &p);
            bump_rem(a, p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", a);
            remove_rem(a);
        } else if (strcmp(cmd, "TOP") == 0) {
            sscanf(line, "%*s %d", &p);
            top_rem(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_rem();
        }
    }
    while (head) {
        Rem *n = head->next;
        free(head->title); free(head->note); free(head);
        head = n;
    }
    return 0;
}