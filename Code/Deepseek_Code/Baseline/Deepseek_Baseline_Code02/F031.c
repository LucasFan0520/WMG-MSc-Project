// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
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

void append_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->next = head;
    head = r;
}

void batch_import(int n) {
    char line[4096];
    char **names = malloc(n * sizeof(char*));
    char **notes = malloc(n * sizeof(char*));
    int valid = 1;
    for (int i = 0; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        line[strcspn(line, "\n")] = 0;
        char *p = line;
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (!*p) { valid = 0; break; }
        *p++ = 0;
        while (*p == ' ') p++;
        names[i] = malloc(strlen(name) + 1);
        strcpy(names[i], name);
        notes[i] = malloc(strlen(p) + 1);
        strcpy(notes[i], p);
        // check if name already exists
        if (find_record(name)) { valid = 0; }
    }
    if (valid) {
        for (int i = 0; i < n; i++) {
            append_record(names[i], notes[i]);
        }
    }
    for (int i = 0; i < n; i++) {
        free(names[i]);
        free(notes[i]);
    }
    free(names);
    free(notes);
}

void delete_record(const char *name) {
    Record *prev = NULL;
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
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
    if (!r) return;
    for (char *c = r->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_records(void) {
    Record *r = head;
    while (r) {
        printf("%s ", r->name);
        for (char *c = r->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        r = r->next;
    }
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->note);
        free(r);
        r = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            int n;
            if (sscanf(p, "%d", &n) == 1 && n > 0) {
                batch_import(n);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_record(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_record_print(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}