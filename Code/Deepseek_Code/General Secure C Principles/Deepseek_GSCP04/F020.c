/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **body;
    size_t bcount, bcap;
} Record;

static Record *records = NULL;
static size_t rcount = 0, rcap = 0;

static Record *find_record(const char *title) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(records[i].title, title) == 0)
            return &records[i];
    }
    return NULL;
}

static void add_body_line(Record *r, const char *line) {
    if (r->bcount == r->bcap) {
        r->bcap = r->bcap ? r->bcap * 2 : 4;
        r->body = realloc(r->body, r->bcap * sizeof(char *));
        if (!r->body) { perror("realloc"); exit(1); }
    }
    r->body[r->bcount] = strdup(line);
    r->bcount++;
}

static void add_record(const char *title) {
    if (rcount == rcap) {
        rcap = rcap ? rcap * 2 : 4;
        records = realloc(records, rcap * sizeof(Record));
        if (!records) { perror("realloc"); exit(1); }
    }
    records[rcount].title = strdup(title);
    records[rcount].body = NULL;
    records[rcount].bcount = records[rcount].bcap = 0;
    rcount++;
}

static void delete_record(const char *title) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(records[i].title, title) == 0) {
            free(records[i].title);
            for (size_t j = 0; j < records[i].bcount; j++) free(records[i].body[j]);
            free(records[i].body);
            for (size_t j = i; j < rcount - 1; j++)
                records[j] = records[j + 1];
            rcount--;
            return;
        }
    }
}

static void print_record(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    for (size_t i = 0; i < r->bcount; i++) {
        printf("%s\n", r->body[i]);
    }
}

static void print_all(void) {
    for (size_t i = 0; i < rcount; i++) {
        printf("%s\n", records[i].title);
        for (size_t j = 0; j < records[i].bcount; j++) {
            printf("%s\n", records[i].body[j]);
        }
    }
}

static void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    Record *current = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            current = NULL;
            continue;
        }
        if (!current) {
            // new record
            if (strlen(line) > 0) {
                add_record(line);
                current = &records[rcount - 1];
            }
        } else {
            add_body_line(current, line);
        }
    }
    fclose(f);
}

static void add_record_interactive(void) {
    char line[4096];
    if (!fgets(line, sizeof(line), stdin)) return;
    line[strcspn(line, "\n")] = '\0';
    if (strlen(line) == 0) return;
    add_record(line);
    Record *r = &records[rcount - 1];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        add_body_line(r, line);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) print_record(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_record_interactive();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    for (size_t i = 0; i < rcount; i++) {
        free(records[i].title);
        for (size_t j = 0; j < records[i].bcount; j++) free(records[i].body[j]);
        free(records[i].body);
    }
    free(records);
    return 0;
}