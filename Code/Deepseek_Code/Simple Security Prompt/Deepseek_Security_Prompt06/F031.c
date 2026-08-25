// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *name) {
    Record *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *r = records, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
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
    Record *r = find_record(name);
    if (r) printf("%s %s\n", r->name, r->note);
}

void report_all(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void process_batch(int n) {
    if (n <= 0) return;
    char **names = malloc(n * sizeof(char *));
    char **notes = malloc(n * sizeof(char *));
    int valid = 1;
    for (int i = 0; i < n; i++) {
        char line[4096];
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        line[strcspn(line, "\n")] = '\0';
        char name[256], note[4096];
        if (sscanf(line, "%255s", name) != 1) { valid = 0; break; }
        const char *nptr = line + strlen(name) + 1;
        while (*nptr == ' ') nptr++;
        if (*nptr) strcpy(note, nptr);
        else note[0] = '\0';
        // check if name already exists
        if (find_record(name)) { valid = 0; break; }
        names[i] = malloc(strlen(name) + 1);
        strcpy(names[i], name);
        notes[i] = malloc(strlen(note) + 1);
        strcpy(notes[i], note);
    }
    if (valid) {
        for (int i = 0; i < n; i++) {
            add_record(names[i], notes[i]);
        }
    }
    for (int i = 0; i < n; i++) {
        free(names[i]);
        free(notes[i]);
    }
    free(names);
    free(notes);
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *tmp = r->next;
        free(r->name);
        free(r->note);
        free(r);
        r = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], note[4096];
        int n;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "BATCH") == 0) {
            if (sscanf(rest, "%d", &n) == 1) process_batch(n);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", name) == 1) find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}