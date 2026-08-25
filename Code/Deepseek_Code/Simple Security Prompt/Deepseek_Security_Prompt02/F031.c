/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_record(Record *r) {
    free(r->name);
    free(r->note);
    free(r);
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
    Record *prev = NULL, *curr = records;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            free_record(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void batch_import(int n) {
    char line[4096];
    Record *batch = NULL;
    int valid = 1;
    for (int i = 0; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        char name[256], note[4096];
        if (sscanf(line, "%255s", name) != 1) { valid = 0; break; }
        char *p = line + strlen(name);
        while (*p == ' ') p++;
        Record *r = malloc(sizeof(Record));
        r->name = strdup(name);
        r->note = strdup(p);
        r->next = batch;
        batch = r;
    }
    if (valid) {
        while (batch) {
            Record *tmp = batch;
            batch = batch->next;
            if (!find_record(tmp->name)) {
                tmp->next = records;
                records = tmp;
            } else {
                free_record(tmp);
            }
        }
    } else {
        while (batch) {
            Record *tmp = batch;
            batch = batch->next;
            free_record(tmp);
        }
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];
    int n;

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "BATCH") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &n) == 1 && n > 0) batch_import(n);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }

    while (records) {
        Record *tmp = records;
        records = records->next;
        free_record(tmp);
    }
    return 0;
}