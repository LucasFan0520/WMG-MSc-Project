/* F031.c */
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

void append_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void batch_import(int n) {
    char **names = malloc(n * sizeof(char*));
    char **notes = malloc(n * sizeof(char*));
    if (!names || !notes) {
        free(names);
        free(notes);
        return;
    }
    int valid = 1;
    for (int i = 0; i < n; i++) {
        char line[4096];
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        line[strcspn(line, "\n")] = '\0';
        char name[256], note[2048];
        if (sscanf(line, "%255s %2047s", name, note) < 2) {
            valid = 0;
            break;
        }
        names[i] = strdup(name);
        notes[i] = strdup(note);
        if (!names[i] || !notes[i]) valid = 0;
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
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", r->name, r->note);
}

void report_all(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256];
        int n = sscanf(line, "%15s %255s", cmd, arg1);
        if (strcmp(cmd, "BATCH") == 0 && n >= 2) {
            batch_import(atoi(arg1));
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_record_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
    return 0;
}