/*
 * F031.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *name;
    char *note;
    struct record *next;
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
    Record *prev = NULL, *r = records;
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

void find_record_cmd(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", r->name, r->note);
}

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(line, "%*s %d", &n) != 1) continue;
            char batch_lines[100][4096]; // assume max 100, but can be dynamic
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(batch_lines[i], sizeof(batch_lines[i]), stdin)) {
                    valid = 0;
                    break;
                }
                batch_lines[i][strcspn(batch_lines[i], "\n")] = 0;
                // check format: name note
                char name[256];
                if (sscanf(batch_lines[i], "%255s", name) != 1) {
                    valid = 0;
                }
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    char name[256], note[4096];
                    if (sscanf(batch_lines[i], "%255s", name) == 1) {
                        char *rest = batch_lines[i] + strlen(name) + 1;
                        while (*rest == ' ') rest++;
                        add_record(name, rest);
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) find_record_cmd(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}