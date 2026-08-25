// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body_lines;
    int line_count;
    int line_capacity;
} Record;

Record *catalog = NULL;
int count = 0;
int capacity = 0;

int find_record(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(catalog[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

void add_record_line(int idx, const char *line) {
    Record *r = &catalog[idx];
    if (r->line_count >= r->line_capacity) {
        r->line_capacity = r->line_capacity == 0 ? 4 : r->line_capacity * 2;
        r->body_lines = realloc(r->body_lines, r->line_capacity * sizeof(char *));
    }
    r->body_lines[r->line_count] = strdup(line);
    r->line_count++;
}

int create_record(const char *title) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        catalog = realloc(catalog, capacity * sizeof(Record));
    }
    catalog[count].title = strdup(title);
    catalog[count].body_lines = NULL;
    catalog[count].line_count = 0;
    catalog[count].line_capacity = 0;
    count++;
    return count - 1;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[4096];
            int current_idx = -1;
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\r\n")] = 0;
                if (strcmp(fline, "--") == 0) {
                    current_idx = -1;
                } else {
                    if (current_idx == -1) {
                        current_idx = create_record(fline);
                    } else {
                        add_record_line(current_idx, fline);
                    }
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            int idx = find_record(title);
            if (idx != -1) {
                printf("%s\n", catalog[idx].title);
                for (int i = 0; i < catalog[idx].line_count; i++) {
                    printf("%s\n", catalog[idx].body_lines[i]);
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            int idx = find_record(title);
            if (idx != -1) {
                free(catalog[idx].title);
                for (int i = 0; i < catalog[idx].line_count; i++) {
                    free(catalog[idx].body_lines[i]);
                }
                free(catalog[idx].body_lines);
                for (int i = idx; i < count - 1; i++) {
                    catalog[i] = catalog[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            int idx = create_record(title);
            char bline[4096];
            while (fgets(bline, sizeof(bline), stdin)) {
                bline[strcspn(bline, "\r\n")] = 0;
                if (strcmp(bline, "--") == 0) {
                    break;
                }
                add_record_line(idx, bline);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].line_count; j++) {
                    printf("%s\n", catalog[i].body_lines[j]);
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].line_count; j++) {
            free(catalog[i].body_lines[j]);
        }
        free(catalog[i].body_lines);
    }
    free(catalog);
    return 0;
}
