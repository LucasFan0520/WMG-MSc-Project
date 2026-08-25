// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *name;
    char *note;
    struct Rec *next;
} Rec;

Rec *head = NULL;

void add_rec(const char *n, const char *nt) {
    Rec *r = malloc(sizeof(Rec));
    r->name = strdup(n);
    r->note = strdup(nt);
    r->next = head;
    head = r;
}

void delete_rec(const char *n) {
    Rec *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, n) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name); free(p->note); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void find_rec(const char *n) {
    for (Rec *r = head; r; r = r->next) {
        if (strcmp(r->name, n) == 0) {
            printf("%s %s\n", r->name, r->note);
            return;
        }
    }
}

void report_rec() {
    for (Rec *r = head; r; r = r->next) {
        printf("%s %s\n", r->name, r->note);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            sscanf(line, "%*s %d", &n);
            char **names = malloc(n * sizeof(char *));
            char **notes = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = 0;
                char name[1000], note[8000];
                note[0] = 0;
                if (sscanf(line, "%s %[^\n]", name, note) < 1) { valid = 0; break; }
                names[i] = strdup(name);
                notes[i] = strdup(note);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    add_rec(names[i], notes[i]);
                }
            }
            for (int i = 0; i < n; i++) {
                free(names[i]); free(notes[i]);
            }
            free(names); free(notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char a[1000];
            sscanf(line, "%*s %s", a);
            delete_rec(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            char a[1000];
            sscanf(line, "%*s %s", a);
            find_rec(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_rec();
        }
    }
    while (head) {
        Rec *n = head->next;
        free(head->name); free(head->note); free(head);
        head = n;
    }
    return 0;
}