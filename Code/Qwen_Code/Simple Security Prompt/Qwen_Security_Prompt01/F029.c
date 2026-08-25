// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *key;
    char *val;
    struct Rec *next;
} Rec;

Rec *head = NULL;

void add_rec(const char *k, const char *v) {
    Rec *r = malloc(sizeof(Rec));
    r->key = strdup(k);
    r->val = strdup(v);
    r->next = head;
    head = r;
}

void dedup_rec() {
    Rec *p = head;
    while (p) {
        Rec *q = p;
        while (q->next) {
            if (strcmp(q->next->key, p->key) == 0) {
                Rec *tmp = q->next;
                q->next = tmp->next;
                free(tmp->key); free(tmp->val); free(tmp);
            } else {
                q = q->next;
            }
        }
        p = p->next;
    }
}

void delete_rec(const char *k) {
    Rec *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->key, k) == 0) {
            Rec *next = p->next;
            if (prev) prev->next = next;
            else head = next;
            free(p->key); free(p->val); free(p);
            p = next;
        } else {
            prev = p; p = p->next;
        }
    }
}

void find_rec(const char *k) {
    for (Rec *r = head; r; r = r->next) {
        if (strcmp(r->key, k) == 0) {
            printf("%s\n", r->val);
        }
    }
}

void report_rec() {
    for (Rec *r = head; r; r = r->next) {
        printf("%s %s\n", r->key, r->val);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            add_rec(a, b);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_rec();
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_rec(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            find_rec(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_rec();
        }
    }
    while (head) {
        Rec *n = head->next;
        free(head->key); free(head->val); free(head);
        head = n;
    }
    return 0;
}