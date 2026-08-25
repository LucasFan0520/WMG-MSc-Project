// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record { char *name; char *note; struct Record *next; } Record;
Record *records = NULL;

Record *find_record(const char *name) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL;
    for (Record *r = records; r; prev = r, r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next; else records = r->next;
            free(r->name); free(r->note); free(r);
            return;
        }
    }
}

void find_record_print(const char *name) {
    Record *r = find_record(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", r->note);
}

void report_records(void) {
    for (Record *r = records; r; r = r->next)
        printf("%s %s\n", r->name, r->note);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(rest, "%d", &n) == 1 && n > 0) {
                char **names = malloc(n * sizeof(char *));
                char **notes = malloc(n * sizeof(char *));
                if (!names || !notes) { free(names); free(notes); continue; }
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                    line[strcspn(line, "\n")] = 0;
                    char name[256], note[4096];
                    if (sscanf(line, "%255s", name) == 1) {
                        char *n = line + strlen(name);
                        while (*n == ' ') n++;
                        strcpy(note, n);
                        names[i] = strdup(name);
                        notes[i] = strdup(note);
                    } else {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    for (int i = 0; i < n; i++) {
                        if (!find_record(names[i]))
                            add_record(names[i], notes[i]);
                    }
                }
                for (int i = 0; i < n; i++) { free(names[i]); free(notes[i]); }
                free(names); free(notes);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    while (records) { Record *r = records; records = r->next; free(r->name); free(r->note); free(r); }
    return 0;
}