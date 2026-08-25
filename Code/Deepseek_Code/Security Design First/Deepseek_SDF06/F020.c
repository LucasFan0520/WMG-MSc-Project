// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    size_t body_count;
    size_t body_cap;
} Record;

static Record *records = NULL;
static size_t record_count = 0;
static size_t record_cap = 0;

static ssize_t find_record(const char *title) {
    for (size_t i = 0; i < record_count; i++)
        if (strcmp(records[i].title, title) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_body_line(Record *rec, const char *line) {
    if (rec->body_count == rec->body_cap) {
        rec->body_cap = rec->body_cap ? rec->body_cap * 2 : 4;
        char **new = realloc(rec->body, rec->body_cap * sizeof(char *));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        rec->body = new;
    }
    rec->body[rec->body_count] = strdup(line);
    if (!rec->body[rec->body_count]) { fprintf(stderr, "Memory error\n"); exit(1); }
    rec->body_count++;
}

static void init_record(Record *rec) {
    rec->title = NULL;
    rec->body = NULL;
    rec->body_count = 0;
    rec->body_cap = 0;
}

static void free_record(Record *rec) {
    free(rec->title);
    for (size_t i = 0; i < rec->body_count; i++)
        free(rec->body[i]);
    free(rec->body);
}

static void add_record(const char *title) {
    if (find_record(title) != -1) return;
    if (record_count == record_cap) {
        record_cap = record_cap ? record_cap * 2 : 4;
        Record *new = realloc(records, record_cap * sizeof(Record));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        records = new;
    }
    init_record(&records[record_count]);
    records[record_count].title = strdup(title);
    if (!records[record_count].title) { fprintf(stderr, "Memory error\n"); exit(1); }
    record_count++;
}

static void delete_record(const char *title) {
    ssize_t idx = find_record(title);
    if (idx == -1) return;
    free_record(&records[idx]);
    for (size_t i = idx; i < record_count - 1; i++)
        records[i] = records[i + 1];
    record_count--;
}

static void find_record_print(const char *title) {
    ssize_t idx = find_record(title);
    if (idx == -1) return;
    printf("%s\n", records[idx].title);
    for (size_t i = 0; i < records[idx].body_count; i++)
        printf("%s\n", records[idx].body[i]);
}

static void print_all_records(void) {
    for (size_t i = 0; i < record_count; i++) {
        printf("%s\n", records[i].title);
        for (size_t j = 0; j < records[i].body_count; j++)
            printf("%s\n", records[i].body[j]);
        printf("--\n");
    }
}

static void free_all(void) {
    for (size_t i = 0; i < record_count; i++)
        free_record(&records[i]);
    free(records);
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int in_record = 0;
    char current_title[256] = "";
    Record *cur = NULL;

    while ((nread = getline(&line, &len, f)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "--") == 0) {
            in_record = 0;
            cur = NULL;
            continue;
        }
        if (!in_record) {
            // new record
            if (line[0] == '\0') continue;
            strncpy(current_title, line, sizeof(current_title) - 1);
            current_title[sizeof(current_title) - 1] = '\0';
            add_record(current_title);
            cur = &records[record_count - 1];
            in_record = 1;
        } else {
            if (cur) add_body_line(cur, line);
        }
    }
    free(line);
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);

    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], title[256];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) {
                add_record(title);
                Record *cur = &records[record_count - 1];
                char *bl = NULL;
                size_t blen = 0;
                ssize_t bnr;
                while ((bnr = getline(&bl, &blen, stdin)) != -1) {
                    if (bl[bnr - 1] == '\n') bl[bnr - 1] = '\0';
                    if (strcmp(bl, "--") == 0) break;
                    add_body_line(cur, bl);
                }
                free(bl);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all_records();
        }
    }

    free(line);
    free_all();
    return 0;
}