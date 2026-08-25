// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rec {
    char *name;
    char *note;
    struct rec *next;
} Rec;

Rec *head = NULL;
Rec *tail = NULL;

Rec *find_record(const char *name) {
    Rec *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Rec *r = malloc(sizeof(Rec));
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void delete_record(const char *name) {
    Rec *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->name);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void find_record_print(const char *name) {
    Rec *r = find_record(name);
    if (r) printf("%s\n", r->note);
}

void report_records(void) {
    Rec *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void batch_command(int n) {
    char **names = malloc(n * sizeof(char*));
    char **notes = malloc(n * sizeof(char*));
    int ok = 1;
    for (int i = 0; i < n; i++) {
        char line[4096];
        if (!fgets(line, sizeof(line), stdin)) { ok = 0; break; }
        line[strcspn(line, "\n")] = '\0';
        char name[256], note[4096];
        if (sscanf(line, "%255s %[^\n]", name, note) == 2) {
            names[i] = strdup(name);
            notes[i] = strdup(note);
        } else {
            ok = 0;
        }
        if (!ok) break;
    }
    if (ok) {
        for (int i = 0; i < n; i++) {
            if (!find_record(names[i])) {
                add_record(names[i], notes[i]);
            }
        }
    }
    for (int i = 0; i < n; i++) {
        free(names[i]);
        free(notes[i]);
    }
    free(names);
    free(notes);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        int n;
        char name[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1)
                batch_command(n);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}