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

void add_record(const char *name, const char *note) {
    Record *r = malloc(sizeof(Record));
    if (!r) exit(EXIT_FAILURE);
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *curr = records;
    Record *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_record(const char *name) {
    Record *curr = records;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            printf("%s %s\n", curr->name, curr->note);
            return;
        }
        curr = curr->next;
    }
}

void report_records() {
    Record *curr = records;
    while (curr) {
        printf("%s %s\n", curr->name, curr->note);
        curr = curr->next;
    }
}

void free_all() {
    Record *curr = records;
    while (curr) {
        Record *next = curr->next;
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[3800] = {0};
        int n = 0;
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, arg1, arg2);
        if (matched >= 1) sscanf(arg1, "%d", &n);
        
        if (strcmp(cmd, "BATCH") == 0) {
            Record *batch_head = NULL;
            Record *batch_tail = NULL;
            int valid = 1;
            
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = 0;
                
                char b_name[256] = {0};
                char b_note[3800] = {0};
                int b_matched = sscanf(line, "%255s %[^\n]", b_name, b_note);
                if (b_matched != 2) {
                    valid = 0;
                    break;
                }
                
                Record *r = malloc(sizeof(Record));
                if (!r) exit(EXIT_FAILURE);
                r->name = strdup(b_name);
                r->note = strdup(b_note);
                r->next = NULL;
                
                if (!batch_head) {
                    batch_head = r;
                    batch_tail = r;
                } else {
                    batch_tail->next = r;
                    batch_tail = r;
                }
            }
            
            if (valid) {
                if (batch_tail) {
                    batch_tail->next = records;
                    records = batch_head;
                }
            } else {
                Record *curr = batch_head;
                while (curr) {
                    Record *next = curr->next;
                    free(curr->name);
                    free(curr->note);
                    free(curr);
                    curr = next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}