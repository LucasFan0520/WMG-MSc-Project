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

Record* find_record(const char *name) {
    Record *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *new = malloc(sizeof(Record));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Record *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void delete_record(const char *name) {
    Record *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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
    if (!r) return;
    printf("%s %s\n", r->name, r->note);
}

void report() {
    Record *cur = head;
    while (cur) {
        printf("%s %s\n", cur->name, cur->note);
        cur = cur->next;
    }
}

void free_all() {
    Record *cur = head;
    while (cur) {
        Record *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_str = strtok(NULL, " ");
            if (!n_str) continue;
            int n = atoi(n_str);
            // read n lines
            char batch_lines[100][4096]; // assume enough for n, but n could be large; dynamic?
            // We'll use a dynamic array for safety, but for simplicity we assume n reasonable.
            // Better: store in temporary list.
            Record *temp_head = NULL;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = 0;
                char *name = strtok(line, " ");
                if (!name) { valid = 0; break; }
                char *note = strtok(NULL, "");
                if (!note) note = "";
                // store temporarily
                Record *new = malloc(sizeof(Record));
                new->name = strdup(name);
                new->note = strdup(note);
                new->next = NULL;
                if (!temp_head) temp_head = new;
                else {
                    Record *cur = temp_head;
                    while (cur->next) cur = cur->next;
                    cur->next = new;
                }
            }
            if (valid) {
                // append all temp records to main list
                if (temp_head) {
                    if (!head) head = temp_head;
                    else {
                        Record *cur = head;
                        while (cur->next) cur = cur->next;
                        cur->next = temp_head;
                    }
                }
            } else {
                // discard temp records
                Record *cur = temp_head;
                while (cur) {
                    Record *tmp = cur;
                    cur = cur->next;
                    free(tmp->name);
                    free(tmp->note);
                    free(tmp);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) print_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}