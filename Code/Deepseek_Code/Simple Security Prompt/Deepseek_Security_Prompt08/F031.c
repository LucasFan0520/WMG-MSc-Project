// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *head = NULL, *tail = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *note) {
    Record *r = malloc(sizeof(Record));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->next = NULL;
    if (tail) {
        tail->next = r;
        tail = r;
    } else {
        head = tail = r;
    }
}

void delete_record(const char *name) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_record_print(const char *name) {
    Record *r = find_record(name);
    if (r) printf("%s %s\n", r->name, r->note);
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_str = strtok(NULL, " ");
            if (!n_str) continue;
            int n = atoi(n_str);
            char **names = malloc(n * sizeof(char *));
            char **notes = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char *batch_line = NULL;
                size_t batch_len = 0;
                ssize_t br = getline(&batch_line, &batch_len, stdin);
                if (br == -1) { valid = 0; break; }
                if (batch_line[br - 1] == '\n') batch_line[br - 1] = '\0';
                char *bname = strtok(batch_line, " ");
                char *bnote = strtok(NULL, "");
                if (!bname || !bnote) { valid = 0; free(batch_line); break; }
                names[i] = malloc(strlen(bname) + 1);
                strcpy(names[i], bname);
                notes[i] = malloc(strlen(bnote) + 1);
                strcpy(notes[i], bnote);
                free(batch_line);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    add_record(names[i], notes[i]);
                }
            }
            for (int i = 0; i < n; i++) {
                free(names[i]);
                free(notes[i]);
            }
            free(names);
            free(notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free(line);
    free_all();
    return 0;
}