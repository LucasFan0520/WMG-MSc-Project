// filename: F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char *body; // concatenated lines with newlines? We'll store as single string with newlines
    struct Record *next;
} Record;

static Record *records = NULL;

static void cleanup(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->title);
        free(tmp->body);
        free(tmp);
    }
}

static void load_records(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    Record *current = NULL;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            current = NULL;
            continue;
        }
        if (!current) {
            // new record: title is this line
            current = malloc(sizeof(Record));
            if (!current) continue;
            current->title = strdup(line);
            current->body = strdup("");
            if (!current->title || !current->body) {
                free(current->title);
                free(current->body);
                free(current);
                current = NULL;
                continue;
            }
            current->next = records;
            records = current;
        } else {
            // body line: append with newline
            size_t newlen = strlen(current->body) + strlen(line) + 2; // +1 for newline, +1 for null
            char *newbody = malloc(newlen);
            if (!newbody) continue;
            snprintf(newbody, newlen, "%s%s\n", current->body, line);
            free(current->body);
            current->body = newbody;
        }
    }
    free(line);
    fclose(f);
}

static Record *find_record(const char *title) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->title, title) == 0)
            return r;
    return NULL;
}

static void delete_record(const char *title) {
    Record *prev = NULL;
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->title);
            free(r->body);
            free(r);
            return;
        }
        prev = r;
    }
}

static void add_record_from_stdin(void) {
    // read title, then body lines until --
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, stdin) == -1) { free(line); return; }
    line[strcspn(line, "\n")] = '\0';
    char *title = line;
    if (find_record(title)) { free(line); return; }
    Record *r = malloc(sizeof(Record));
    if (!r) { free(line); return; }
    r->title = strdup(title);
    r->body = strdup("");
    if (!r->title || !r->body) {
        free(r->title);
        free(r->body);
        free(r);
        free(line);
        return;
    }
    free(line);
    // read body lines until --
    char *bodyline = NULL;
    size_t blen = 0;
    while (getline(&bodyline, &blen, stdin) != -1) {
        bodyline[strcspn(bodyline, "\n")] = '\0';
        if (strcmp(bodyline, "--") == 0) {
            free(bodyline);
            break;
        }
        size_t newlen = strlen(r->body) + strlen(bodyline) + 2;
        char *newbody = malloc(newlen);
        if (!newbody) { free(bodyline); free(r->body); free(r->title); free(r); return; }
        snprintf(newbody, newlen, "%s%s\n", r->body, bodyline);
        free(r->body);
        r->body = newbody;
        free(bodyline);
        bodyline = NULL;
        blen = 0;
    }
    r->next = records;
    records = r;
}

static void print_record(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    printf("%s", r->body);
    printf("--\n");
}

static void print_all(void) {
    for (Record *r = records; r; r = r->next) {
        printf("%s\n", r->title);
        printf("%s", r->body);
        printf("--\n");
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_records(argv[1]);
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) print_record(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_record(arg1);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_record_from_stdin();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free(line);
    return 0;
}