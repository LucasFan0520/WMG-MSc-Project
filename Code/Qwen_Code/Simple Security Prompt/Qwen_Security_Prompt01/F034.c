// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *new_head = NULL;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[1000], note[8000];
        note[0] = 0;
        if (sscanf(line, "%s %[^\n]", name, note) >= 1) {
            Note *n = malloc(sizeof(Note));
            n->name = strdup(name);
            n->note = strdup(note);
            n->next = new_head;
            new_head = n;
        }
    }
    fclose(f);
    while (head) {
        Note *n = head->next;
        free(head->name); free(head->note); free(head);
        head = n;
    }
    head = new_head;
}

void add_note(const char *name, const char *note) {
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = head;
    head = n;
}

void delete_note(const char *name) {
    Note *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name); free(p->note); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void report_notes() {
    for (Note *n = head; n; n = n->next) {
        printf("%s %s\n", n->name, n->note);
    }
}

int main(int argc, char **argv) {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            add_note(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_note(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    while (head) {
        Note *n = head->next;
        free(head->name); free(head->note); free(head);
        head = n;
    }
    return 0;
}