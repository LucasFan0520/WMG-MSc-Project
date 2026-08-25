/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct line {
    char *text;
    struct line *next;
} line_t;

typedef struct record {
    char *title;
    line_t *body;
    struct record *next;
} record_t;

record_t *records = NULL;
record_t *tail = NULL;

record_t *find_record(const char *title) {
    record_t *r = records;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *title, line_t *body) {
    if (find_record(title)) return;
    record_t *r = malloc(sizeof(record_t));
    if (!r) return;
    r->title = strdup(title);
    r->body = body;
    r->next = NULL;
    if (!r->title) { free(r); return; }
    if (tail) tail->next = r;
    else records = r;
    tail = r;
}

void delete_record(const char *title) {
    record_t *prev = NULL, *r = records;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            if (r == tail) tail = prev;
            free(r->title);
            line_t *l = r->body;
            while (l) {
                line_t *nxt = l->next;
                free(l->text);
                free(l);
                l = nxt;
            }
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void find_record_print(const char *title) {
    record_t *r = find_record(title);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->title);
    line_t *l = r->body;
    while (l) {
        printf("%s\n", l->text);
        l = l->next;
    }
}

void print_records(void) {
    record_t *r = records;
    while (r) {
        printf("%s\n", r->title);
        line_t *l = r->body;
        while (l) {
            printf("%s\n", l->text);
            l = l->next;
        }
        printf("--\n");
        r = r->next;
    }
}

void free_all(void) {
    record_t *r = records;
    while (r) {
        record_t *nxt = r->next;
        free(r->title);
        line_t *l = r->body;
        while (l) {
            line_t *n = l->next;
            free(l->text);
            free(l);
            l = n;
        }
        free(r);
        r = nxt;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *line = NULL;
        size_t len = 0;
        line_t *body = NULL;
        line_t *body_tail = NULL;
        char *title = NULL;
        while (getline(&line, &len, f) != -1) {
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, "--") == 0) {
                if (title) {
                    add_record(title, body);
                    title = NULL;
                    body = NULL;
                    body_tail = NULL;
                }
            } else {
                if (!title) {
                    title = strdup(line);
                    body = NULL;
                    body_tail = NULL;
                } else {
                    line_t *l = malloc(sizeof(line_t));
                    if (l) {
                        l->text = strdup(line);
                        l->next = NULL;
                        if (!l->text) { free(l); continue; }
                        if (body_tail) { body_tail->next = l; body_tail = l; }
                        else { body = l; body_tail = l; }
                    }
                }
            }
        }
        if (title) add_record(title, body);
        free(line);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], title[256];
        if (sscanf(line, "%9s %255s", cmd, title) >= 1) {
            if (strcmp(cmd, "FIND") == 0) {
                find_record_print(title);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_record(title);
            } else if (strcmp(cmd, "ADD") == 0) {
                // read body lines until "--"
                line_t *body = NULL;
                line_t *body_tail = NULL;
                char *line2 = NULL;
                size_t len2 = 0;
                while (getline(&line2, &len2, stdin) != -1) {
                    line2[strcspn(line2, "\n")] = 0;
                    if (strcmp(line2, "--") == 0) {
                        free(line2);
                        break;
                    }
                    line_t *l = malloc(sizeof(line_t));
                    if (l) {
                        l->text = strdup(line2);
                        l->next = NULL;
                        if (!l->text) { free(l); free(line2); continue; }
                        if (body_tail) { body_tail->next = l; body_tail = l; }
                        else { body = l; body_tail = l; }
                    }
                    free(line2);
                    line2 = NULL;
                }
                add_record(title, body);
            } else if (strcmp(cmd, "PRINT") == 0) {
                print_records();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}