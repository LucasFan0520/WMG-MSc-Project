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
Record *tail = NULL;

Record *find_record(char *name) {
    for (Record *r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

void append_record(char *name, char *note) {
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = NULL;
    if (!tail) head = tail = r;
    else { tail->next = r; tail = r; }
}

void delete_record(char *name) {
    Record *prev = NULL;
    for (Record *r = head; r; r = r->next) {
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
    }
}

void find_record_print(char *name) {
    Record *r = find_record(name);
    if (r) printf("%s %s\n", r->name, r->note);
}

void report_all(void) {
    for (Record *r = head; r; r = r->next)
        printf("%s %s\n", r->name, r->note);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "BATCH") == 0 && n == 2) {
            int num = atoi(arg1);
            // read num lines, store them temporarily
            Record *batch_head = NULL, *batch_tail = NULL;
            int valid = 1;
            for (int i = 0; i < num; i++) {
                if (getline(&line, &len, stdin) == -1) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = '\0';
                char name[256], note[1024];
                int m = sscanf(line, "%255s %[^\n]", name, note);
                if (m != 2) {
                    valid = 0;
                    break;
                }
                // check if name already exists? The spec says "valid" might mean any name? We'll assume any non-empty name/note is valid.
                Record *r = malloc(sizeof(Record));
                r->name = strdup(name);
                r->note = strdup(note);
                r->next = NULL;
                if (!batch_tail) batch_head = batch_tail = r;
                else { batch_tail->next = r; batch_tail = r; }
            }
            if (valid) {
                // append all to main list
                if (batch_head) {
                    if (!tail) head = tail = batch_head;
                    else { tail->next = batch_head; tail = batch_tail; }
                }
            } else {
                // free batch
                Record *r = batch_head;
                while (r) {
                    Record *next = r->next;
                    free(r->name);
                    free(r->note);
                    free(r);
                    r = next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_record_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}