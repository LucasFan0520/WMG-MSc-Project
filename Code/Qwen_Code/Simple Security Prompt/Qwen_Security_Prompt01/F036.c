// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name;
    char *data;
    struct Rec *next;
} Rec;

Rec *head = NULL;

void add_rec(const char *name, const char *raw) {
    char *colon = strchr(raw, ':');
    if (!colon) return;
    int len = atoi(raw);
    if (len < 0 || len != (int)strlen(colon + 1)) return;
    Rec *r = malloc(sizeof(Rec));
    r->name = strdup(name);
    r->data = strdup(colon + 1);
    r->next = head;
    head = r;
}

void delete_rec(const char *name) {
    Rec *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name); free(p->data); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void show_rec(const char *name) {
    for (Rec *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            for (char *c = r->data; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            return;
        }
    }
}

void report_rec() {
    for (Rec *r = head; r; r = r->next) {
        printf("%s\n", r->name);
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
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_rec(a);
        } else if (strcmp(cmd, "SHOW") == 0) {
            sscanf(line, "%*s %s", a);
            show_rec(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_rec();
        }
    }
    while (head) {
        Rec *n = head->next;
        free(head->name); free(head->data); free(head);
        head = n;
    }
    return 0;
}