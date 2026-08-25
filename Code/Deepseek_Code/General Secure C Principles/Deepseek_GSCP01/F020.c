/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *title;
    char **lines;
    int line_count;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *title) {
    Record *r = records;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record_from_file(FILE *f) {
    char line[1024];
    if (!fgets(line, sizeof(line), f)) return;
    line[strcspn(line, "\n")] = '\0';
    char *title = strdup(line);
    Record *r = malloc(sizeof(Record));
    r->title = title;
    r->lines = NULL;
    r->line_count = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        r->line_count++;
        r->lines = realloc(r->lines, r->line_count * sizeof(char *));
        r->lines[r->line_count - 1] = strdup(line);
    }
    if (find_record(title)) {
        free(r->title);
        for (int i = 0; i < r->line_count; i++) free(r->lines[i]);
        free(r->lines);
        free(r);
        return;
    }
    r->next = records;
    records = r;
}

void load_records(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    while (!feof(f)) {
        char line[1024];
        if (!fgets(line, sizeof(line), f)) break;
        if (strcmp(line, "--\n") == 0) continue;
        fseek(f, -strlen(line), SEEK_CUR);
        add_record_from_file(f);
    }
    fclose(f);
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", r->title);
    for (int i = 0; i < r->line_count; i++) {
        for (char *c = r->lines[i]; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
}

void delete_record(const char *title) {
    Record *r = records, *prev = NULL;
    while (r && strcmp(r->title, title) != 0) {
        prev = r;
        r = r->next;
    }
    if (!r) return;
    if (prev) prev->next = r->next;
    else records = r->next;
    free(r->title);
    for (int i = 0; i < r->line_count; i++) free(r->lines[i]);
    free(r->lines);
    free(r);
}

void add_record_interactive(void) {
    char title[1024];
    if (!fgets(title, sizeof(title), stdin)) return;
    title[strcspn(title, "\n")] = '\0';
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->lines = NULL;
    r->line_count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        r->line_count++;
        r->lines = realloc(r->lines, r->line_count * sizeof(char *));
        r->lines[r->line_count - 1] = strdup(line);
    }
    r->next = records;
    records = r;
}

void print_all(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->title);
        for (int i = 0; i < r->line_count; i++) {
            for (char *c = r->lines[i]; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
        }
        r = r->next;
    }
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->title);
        for (int i = 0; i < r->line_count; i++) free(r->lines[i]);
        free(r->lines);
        free(r);
        r = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) load_records(argv[1]);
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_record_print(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_record(p);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_record_interactive();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    free(line);
    return 0;
}