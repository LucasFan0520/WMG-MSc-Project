// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_NOTE 4096

typedef struct Record {
    char name[MAX_NAME];
    char note[MAX_NOTE];
    struct Record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_all() {
    Record *r = head;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp);
    }
    head = NULL;
}

void handle_batch(char *line) {
    int n;
    if (sscanf(line, "BATCH %d", &n) != 1) return;
    if (n <= 0) return;
    char names[MAX_LINE][MAX_NAME];
    char notes[MAX_LINE][MAX_NOTE];
    int valid = 1;
    for (int i = 0; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        line[strcspn(line, "\n")] = '\0';
        char name[MAX_NAME], note[MAX_NOTE];
        if (sscanf(line, "%63s %4095[^\n]", name, note) != 2) { valid = 0; break; }
        if (find_record(name)) { valid = 0; break; }
        strcpy(names[i], name);
        strcpy(notes[i], note);
    }
    if (valid) {
        for (int i = 0; i < n; i++) {
            Record *r = malloc(sizeof(Record));
            if (!r) { valid = 0; break; }
            strcpy(r->name, names[i]);
            strcpy(r->note, notes[i]);
            r->next = head;
            head = r;
        }
    }
    // If invalid, discard all; already not added
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_find(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "FIND %63s", name) != 1) return;
    Record *r = find_record(name);
    if (!r) return;
    printf("%s\n", r->note);
}

void handle_report() {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "BATCH ", 6) == 0) handle_batch(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}