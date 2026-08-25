/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *head = NULL;

Record* find_record(const char *name) {
    Record *r = head;
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
    r->next = head;
    head = r;
}

void delete_record(const char *name) {
    Record *r = head, *prev = NULL;
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
    if (r) printf("%s\n", r->note);
}

void report_records() {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void free_all() {
    while (head) {
        Record *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        int n;
        if (sscanf(line, "%31s %d", cmd, &n) == 2 && strcmp(cmd, "BATCH") == 0) {
            int valid = 1;
            char **names = malloc(n * sizeof(char*));
            char **notes = malloc(n * sizeof(char*));
            for (int i = 0; i < n; i++) {
                char buf[8192];
                if (!fgets(buf, sizeof(buf), stdin)) { valid = 0; break; }
                buf[strcspn(buf, "\n")] = '\0';
                char name[256], note[4096];
                if (sscanf(buf, "%255s %[^\n]", name, note) != 2) { valid = 0; break; }
                if (find_record(name)) { valid = 0; break; }
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
            continue;
        }
        char name[256], note[4096];
        int m = sscanf(line, "%31s %255s %[^\n]", cmd, name, note);
        if (strcmp(cmd, "DELETE") == 0 && m >= 2) {
            delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0 && m >= 2) {
            find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0 && m == 1) {
            report_records();
        }
    }
    free_all();
    return 0;
}