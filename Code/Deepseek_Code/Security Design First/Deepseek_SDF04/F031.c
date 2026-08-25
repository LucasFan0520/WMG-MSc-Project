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
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = NULL;
    if (!r->name || !r->note) {
        free(r->name); free(r->note); free(r);
        return;
    }
    if (!head) head = r;
    else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
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

void report_records(void) {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], note[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(line, "%*s %d", &n) == 1) {
                Record *batch_head = NULL;
                Record *batch_tail = NULL;
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    char *batch_line = NULL;
                    size_t blen = 0;
                    if (getline(&batch_line, &blen, stdin) == -1) {
                        valid = 0;
                        free(batch_line);
                        break;
                    }
                    batch_line[strcspn(batch_line, "\n")] = '\0';
                    char bname[256], bnote[4096];
                    if (sscanf(batch_line, "%255s %4095[^\n]", bname, bnote) != 2) {
                        valid = 0;
                        free(batch_line);
                        break;
                    }
                    if (find_record(bname)) {
                        valid = 0;
                        free(batch_line);
                        break;
                    }
                    Record *r = malloc(sizeof(Record));
                    if (!r) { valid = 0; free(batch_line); break; }
                    r->name = strdup(bname);
                    r->note = strdup(bnote);
                    r->next = NULL;
                    if (!r->name || !r->note) {
                        free(r->name); free(r->note); free(r);
                        valid = 0;
                        free(batch_line);
                        break;
                    }
                    if (!batch_head) batch_head = batch_tail = r;
                    else {
                        batch_tail->next = r;
                        batch_tail = r;
                    }
                    free(batch_line);
                }
                if (valid) {
                    // append batch to main list
                    if (!head) head = batch_head;
                    else {
                        Record *last = head;
                        while (last->next) last = last->next;
                        last->next = batch_head;
                    }
                } else {
                    // discard batch
                    Record *r = batch_head;
                    while (r) {
                        Record *next = r->next;
                        free(r->name);
                        free(r->note);
                        free(r);
                        r = next;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free(line);
    free_all();
    return 0;
}