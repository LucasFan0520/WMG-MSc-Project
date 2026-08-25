// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct record {
    char *title;
    char *body; // concatenated body lines with newlines? We'll store as one string with newlines.
    struct record *next;
} Record;

Record *catalog = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Record *find_record(const char *title) {
    Record *r = catalog;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(Record *r) {
    r->next = catalog;
    catalog = r;
}

void delete_record(const char *title) {
    Record *r = catalog, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else catalog = r->next;
            free(r->title);
            free(r->body);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    printf("%s\n", r->body);
}

void print_catalog(void) {
    Record *r = catalog;
    while (r) {
        printf("%s\n", r->title);
        printf("%s\n", r->body);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = catalog;
    while (r) {
        Record *next = r->next;
        free(r->title);
        free(r->body);
        free(r);
        r = next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    Record *cur = NULL;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strcmp(line, "--") == 0) {
            if (cur) {
                // finalize record
                add_record(cur);
                cur = NULL;
            }
            continue;
        }
        if (!cur) {
            // new record title
            cur = malloc(sizeof(Record));
            if (!cur) { fprintf(stderr, "Memory error\n"); exit(1); }
            cur->title = strdup(line);
            cur->body = strdup("");
            if (!cur->title || !cur->body) { fprintf(stderr, "Memory error\n"); exit(1); }
            cur->next = NULL;
        } else {
            // append body line with newline
            size_t new_len = strlen(cur->body) + strlen(line) + 2;
            char *new_body = malloc(new_len);
            if (!new_body) { fprintf(stderr, "Memory error\n"); exit(1); }
            snprintf(new_body, new_len, "%s%s\n", cur->body, line);
            free(cur->body);
            cur->body = new_body;
        }
    }
    if (cur) {
        add_record(cur);
    }
    fclose(f);
}

void add_record_from_stdin(const char *title) {
    if (find_record(title)) return;
    char line[MAX_LINE];
    Record *r = malloc(sizeof(Record));
    if (!r) { fprintf(stderr, "Memory error\n"); exit(1); }
    r->title = strdup(title);
    r->body = strdup("");
    if (!r->title || !r->body) { fprintf(stderr, "Memory error\n"); exit(1); }
    r->next = NULL;
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "--") == 0) break;
        size_t new_len = strlen(r->body) + strlen(line) + 2;
        char *new_body = malloc(new_len);
        if (!new_body) { fprintf(stderr, "Memory error\n"); exit(1); }
        snprintf(new_body, new_len, "%s%s\n", r->body, line);
        free(r->body);
        r->body = new_body;
    }
    add_record(r);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "PRINT") == 0) { print_catalog(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "FIND") == 0) {
            find_record_print(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(rest);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_record_from_stdin(rest);
        }
    }
    free_all();
    return 0;
}