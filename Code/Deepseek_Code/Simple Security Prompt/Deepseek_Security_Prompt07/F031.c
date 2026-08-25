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
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL;
    Record *r = records;
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
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s:", r->name);
    for (char *s = r->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_records() {
    Record *r = records;
    while (r) {
        printf("%s:", r->name);
        for (char *s = r->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        r = r->next;
    }
}

void free_all() {
    Record *r = records;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(rest, "%d", &n) != 1) continue;
            char **names = malloc(n * sizeof(char *));
            char **notes = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = '\0';
                char name[256], note[4096];
                if (sscanf(line, "%255s %[^\n]", name, note) != 2) {
                    valid = 0;
                    break;
                }
                // check if name already exists in current store
                if (find_record(name)) {
                    valid = 0;
                    break;
                }
                names[i] = strdup(name);
                notes[i] = strdup(note);
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
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_record_print(rest);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}