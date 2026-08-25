// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **lines;
    int line_count;
    struct Record *next;
} Record;

Record *catalog = NULL;

Record *find_record(const char *title) {
    Record *r = catalog;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(Record *rec) {
    rec->next = catalog;
    catalog = rec;
}

void delete_record(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    Record *cur = catalog, *prev = NULL;
    while (cur) {
        if (cur == r) {
            if (prev) prev->next = cur->next;
            else catalog = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(r->title);
    for (int i = 0; i < r->line_count; i++) free(r->lines[i]);
    free(r->lines);
    free(r);
}

void print_record(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    for (int i = 0; i < r->line_count; i++) {
        printf("%s\n", r->lines[i]);
    }
}

void print_catalog(void) {
    Record *r = catalog;
    while (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->line_count; i++) {
            printf("%s\n", r->lines[i]);
        }
        printf("--\n");
        r = r->next;
    }
}

void cleanup_catalog(void) {
    Record *r = catalog;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp->title);
        for (int i = 0; i < tmp->line_count; i++) free(tmp->lines[i]);
        free(tmp->lines);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            Record *current = NULL;
            int reading_body = 0;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) {
                    if (current) {
                        add_record(current);
                        current = NULL;
                    }
                    reading_body = 0;
                    continue;
                }
                if (!current) {
                    current = malloc(sizeof(Record));
                    current->title = strdup(line);
                    current->lines = NULL;
                    current->line_count = 0;
                    reading_body = 1;
                } else if (reading_body) {
                    current->lines = realloc(current->lines, (current->line_count + 1) * sizeof(char *));
                    current->lines[current->line_count] = strdup(line);
                    current->line_count++;
                }
            }
            if (current) {
                add_record(current);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256];
        int n = sscanf(line, "%31s %255s", cmd, title);
        if (n < 2) continue;
        if (strcmp(cmd, "FIND") == 0) {
            print_record(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            // read body lines until "--"
            Record *r = malloc(sizeof(Record));
            r->title = strdup(title);
            r->lines = NULL;
            r->line_count = 0;
            char body[4096];
            while (fgets(body, sizeof(body), stdin)) {
                body[strcspn(body, "\n")] = 0;
                if (strcmp(body, "--") == 0) break;
                r->lines = realloc(r->lines, (r->line_count + 1) * sizeof(char *));
                r->lines[r->line_count] = strdup(body);
                r->line_count++;
            }
            add_record(r);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_catalog();
        }
    }
    cleanup_catalog();
    return 0;
}