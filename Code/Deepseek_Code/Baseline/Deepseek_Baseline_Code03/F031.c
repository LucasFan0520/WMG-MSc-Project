// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct record {
    char *name;
    char *note;
    struct record *next;
};

struct record *records = NULL;

struct record *find_record(char *name) {
    struct record *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(char *name, char *note) {
    if (find_record(name)) return;
    struct record *r = malloc(sizeof(struct record));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->next = records;
    records = r;
}

void delete_record(char *name) {
    struct record *r = records, *prev = NULL;
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

void find_record_print(char *name) {
    struct record *r = find_record(name);
    if (r) printf("%s\n", r->note);
}

void report_records() {
    struct record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        int n;
        if (strcmp(cmd, "BATCH") == 0) {
            sscanf(line, "%31s %d", cmd, &n);
            char **names = malloc(n * sizeof(char *));
            char **notes = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char buf[4096];
                if (!fgets(buf, sizeof(buf), stdin)) { valid = 0; break; }
                buf[strcspn(buf, "\n")] = 0;
                char name[256], note[4096];
                if (sscanf(buf, "%255s %4095[^\n]", name, note) < 2) {
                    valid = 0;
                    break;
                }
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
        } else {
            char arg1[256], arg2[4096];
            int num = sscanf(line, "%31s %255s %4095[^\n]", cmd, arg1, arg2);
            if (strcmp(cmd, "DELETE") == 0 && num >= 2) {
                delete_record(arg1);
            } else if (strcmp(cmd, "FIND") == 0 && num >= 2) {
                find_record_print(arg1);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_records();
            }
        }
    }
    return 0;
}