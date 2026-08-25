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
Record *tail = NULL;

Record *find_record(const char *name) {
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
    r->name = malloc(strlen(name)+1);
    strcpy(r->name, name);
    r->note = malloc(strlen(note)+1);
    strcpy(r->note, note);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void delete_record(const char *name) {
    Record *prev = NULL;
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
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
    printf("%s\n", r->note);
}

void report_records(void) {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void batch_import(int n) {
    Record *temp_head = NULL;
    Record *temp_tail = NULL;
    int valid = 1;
    for (int i = 0; i < n; i++) {
        char line[1024];
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        line[strcspn(line, "\n")] = 0;
        char *name = strtok(line, " ");
        if (!name) { valid = 0; break; }
        char *note = strtok(NULL, "");
        if (!note) note = "";
        Record *r = malloc(sizeof(Record));
        r->name = malloc(strlen(name)+1);
        strcpy(r->name, name);
        r->note = malloc(strlen(note)+1);
        strcpy(r->note, note);
        r->next = NULL;
        if (temp_tail) temp_tail->next = r;
        else temp_head = r;
        temp_tail = r;
    }
    if (valid) {
        // append all temp records to main list
        if (temp_head) {
            if (tail) tail->next = temp_head;
            else head = temp_head;
            tail = temp_tail;
        }
    } else {
        // free temp list
        Record *r = temp_head;
        while (r) {
            Record *next = r->next;
            free(r->name);
            free(r->note);
            free(r);
            r = next;
        }
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_str = strtok(rest, " ");
            if (n_str) batch_import(atoi(n_str));
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(rest, " ");
            if (name) find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}