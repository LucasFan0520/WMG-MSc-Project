// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *r_head = NULL;

Record *find_record(const char *name) {
    Record *r = r_head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = r_head;
    r_head = r;
}

void delete_record(const char *name) {
    Record *prev = NULL;
    Record *r = r_head;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else r_head = r->next;
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
    for (char *p = r->note; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void report_all(void) {
    Record *r = r_head;
    while (r) {
        printf("%s ", r->name);
        for (char *p = r->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        r = r->next;
    }
}

void free_all(void) {
    while (r_head) {
        Record *next = r_head->next;
        free(r_head->name);
        free(r_head->note);
        free(r_head);
        r_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(line, "%*s %d", &n) != 1 || n <= 0) continue;
            char **names = malloc(n * sizeof(char *));
            char **notes = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char *l = NULL;
                size_t llen = 0;
                if (getline(&l, &llen, stdin) == -1) {
                    valid = 0;
                    free(l);
                    break;
                }
                l[strcspn(l, "\n")] = '\0';
                char name[256], note[1024];
                if (sscanf(l, "%255s %1023[^\n]", name, note) != 2) {
                    valid = 0;
                }
                if (valid) {
                    names[i] = strdup(name);
                    notes[i] = strdup(note);
                } else {
                    names[i] = NULL;
                    notes[i] = NULL;
                }
                free(l);
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
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_record(name);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                find_record_print(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}