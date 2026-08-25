// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char **body; int b_count, b_cap; } Record;
Record *records = NULL;
int rec_count = 0, rec_cap = 0;

int find_record(const char *title) {
    for (int i = 0; i < rec_count; i++)
        if (strcmp(records[i].title, title) == 0) return i;
    return -1;
}

void add_record_from_file(FILE *f, const char *title) {
    if (find_record(title) >= 0) return;
    if (rec_count == rec_cap) {
        rec_cap = rec_cap ? rec_cap * 2 : 4;
        records = realloc(records, sizeof(Record) * rec_cap);
    }
    records[rec_count].title = strdup(title);
    records[rec_count].body = NULL;
    records[rec_count].b_count = 0;
    records[rec_count].b_cap = 0;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        Record *r = &records[rec_count];
        if (r->b_count == r->b_cap) {
            r->b_cap = r->b_cap ? r->b_cap * 2 : 4;
            r->body = realloc(r->body, sizeof(char *) * r->b_cap);
        }
        r->body[r->b_count] = strdup(line);
        r->b_count++;
    }
    rec_count++;
}

void delete_record(const char *title) {
    int idx = find_record(title);
    if (idx < 0) return;
    for (int i = 0; i < records[idx].b_count; i++) free(records[idx].body[i]);
    free(records[idx].body);
    free(records[idx].title);
    for (int i = idx; i < rec_count - 1; i++) records[i] = records[i + 1];
    rec_count--;
}

void find_record_print(const char *title) {
    int idx = find_record(title);
    if (idx < 0) return;
    printf("%s\n", records[idx].title);
    for (int i = 0; i < records[idx].b_count; i++)
        printf("%s\n", records[idx].body[i]);
}

void print_records(void) {
    for (int i = 0; i < rec_count; i++) {
        printf("%s\n", records[i].title);
        for (int j = 0; j < records[i].b_count; j++)
            printf("%s\n", records[i].body[j]);
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) continue;
        char title[4096];
        strcpy(title, line);
        add_record_from_file(f, title);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %4095s", title) == 1) find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %4095s", title) == 1) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %4095s", title) == 1) {
                char buf[4096];
                while (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\n")] = '\0';
                    if (strcmp(buf, "--") == 0) break;
                }
                if (find_record(title) >= 0) {
                    while (fgets(buf, sizeof(buf), stdin)) {
                        buf[strcspn(buf, "\n")] = '\0';
                        if (strcmp(buf, "--") == 0) break;
                    }
                } else {
                    if (rec_count == rec_cap) {
                        rec_cap = rec_cap ? rec_cap * 2 : 4;
                        records = realloc(records, sizeof(Record) * rec_cap);
                    }
                    records[rec_count].title = strdup(title);
                    records[rec_count].body = NULL;
                    records[rec_count].b_count = 0;
                    records[rec_count].b_cap = 0;
                    while (fgets(buf, sizeof(buf), stdin)) {
                        buf[strcspn(buf, "\n")] = '\0';
                        if (strcmp(buf, "--") == 0) break;
                        Record *r = &records[rec_count];
                        if (r->b_count == r->b_cap) {
                            r->b_cap = r->b_cap ? r->b_cap * 2 : 4;
                            r->body = realloc(r->body, sizeof(char *) * r->b_cap);
                        }
                        r->body[r->b_count] = strdup(buf);
                        r->b_count++;
                    }
                    rec_count++;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) print_records();
    }
    for (int i = 0; i < rec_count; i++) {
        for (int j = 0; j < records[i].b_count; j++) free(records[i].body[j]);
        free(records[i].body);
        free(records[i].title);
    }
    free(records);
    return 0;
}