/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *name;
    char *note;
    struct record *next;
} record_t;

record_t *head = NULL;
record_t *tail = NULL;

record_t *find_record(const char *name) {
    record_t *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void append_record(const char *name, const char *note) {
    if (find_record(name)) return;
    record_t *r = malloc(sizeof(record_t));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = NULL;
    if (!r->name || !r->note) {
        free(r->name); free(r->note); free(r);
        return;
    }
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void delete_record(const char *name) {
    record_t *prev = NULL, *r = head;
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
    record_t *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", r->name, r->note);
}

void report_records(void) {
    record_t *r = head;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void free_all(void) {
    record_t *r = head;
    while (r) {
        record_t *nxt = r->next;
        free(r->name);
        free(r->note);
        free(r);
        r = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256];
        if (sscanf(line, "%9s %d", cmd, &a) >= 1) {
            if (strcmp(cmd, "BATCH") == 0) {
                int n;
                if (sscanf(line, "%*s %d", &n) == 1) {
                    // read n lines, validate all
                    char **names = malloc(sizeof(char*) * n);
                    char **notes = malloc(sizeof(char*) * n);
                    int valid = 1;
                    for (int i = 0; i < n; i++) {
                        char *l = NULL;
                        size_t llen = 0;
                        if (getline(&l, &llen, stdin) != -1) {
                            l[strcspn(l, "\n")] = 0;
                            char name[256];
                            if (sscanf(l, "%255s", name) == 1) {
                                const char *note = l + strlen(name) + 1;
                                names[i] = strdup(name);
                                notes[i] = strdup(note);
                                if (!names[i] || !notes[i]) valid = 0;
                            } else {
                                valid = 0;
                            }
                            free(l);
                        } else {
                            valid = 0;
                        }
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
            } else if (strcmp(cmd, "DELETE") == 0) {
                if (sscanf(line, "%*s %255s", a) == 1)
                    delete_record(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                if (sscanf(line, "%*s %255s", a) == 1)
                    find_record_print(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_records();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}