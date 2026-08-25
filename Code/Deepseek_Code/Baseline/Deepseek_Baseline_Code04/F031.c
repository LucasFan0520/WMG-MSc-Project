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

void add_record(const char *name, const char *note) {
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL, *cur = records;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_record(const char *name) {
    Record *r = find_record(name);
    if (r) printf("%s %s\n", r->name, r->note);
}

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void free_records(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        int n;
        if (sscanf(line, "%s %d", cmd, &n) == 2 && strcmp(cmd, "BATCH") == 0) {
            Record *temp = NULL;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char batch_line[4096];
                if (!fgets(batch_line, sizeof(batch_line), stdin)) {
                    valid = 0;
                    break;
                }
                batch_line[strcspn(batch_line, "\n")] = '\0';
                char name[256];
                char *p = batch_line;
                while (*p == ' ') p++;
                char *name_start = p;
                while (*p && *p != ' ') p++;
                if (*p == '\0') {
                    valid = 0;
                    break;
                }
                *p++ = '\0';
                while (*p == ' ') p++;
                if (*p == '\0') {
                    valid = 0;
                    break;
                }
                // valid
                Record *r = malloc(sizeof(Record));
                r->name = strdup(name_start);
                r->note = strdup(p);
                r->next = temp;
                temp = r;
            }
            if (valid) {
                // append all temp to records
                while (temp) {
                    Record *next = temp->next;
                    temp->next = records;
                    records = temp;
                    temp = next;
                }
            } else {
                // discard temp
                while (temp) {
                    Record *tmp = temp;
                    temp = temp->next;
                    free(tmp->name);
                    free(tmp->note);
                    free(tmp);
                }
            }
        } else {
            char name[256];
            if (sscanf(line, "%s %s", cmd, name) >= 1) {
                if (strcmp(cmd, "DELETE") == 0) {
                    if (strlen(name) > 0) delete_record(name);
                } else if (strcmp(cmd, "FIND") == 0) {
                    if (strlen(name) > 0) print_record(name);
                } else if (strcmp(cmd, "REPORT") == 0) {
                    report_records();
                }
            }
        }
    }
    free_records();
    return 0;
}