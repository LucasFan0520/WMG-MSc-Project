// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body_lines;
    size_t line_count;
    size_t line_capacity;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;

    Record *items = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        long fread_bytes;
        int next_is_title = 1;

        while ((fread_bytes = getline(&fline, &flen, f)) != -1) {
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }

            if (strcmp(fline, "--") == 0) {
                next_is_title = 1;
                continue;
            }

            if (next_is_title) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Record *new_items = realloc(items, capacity * sizeof(Record));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].title = strdup(fline);
                if (!items[count].title) exit(1);
                items[count].body_lines = NULL;
                items[count].line_count = 0;
                items[count].line_capacity = 0;
                next_is_title = 0;
                count++;
            } else {
                Record *r = &items[count - 1];
                if (r->line_count >= r->line_capacity) {
                    r->line_capacity = r->line_capacity == 0 ? 4 : r->line_capacity * 2;
                    char **nl = realloc(r->body_lines, r->line_capacity * sizeof(char *));
                    if (!nl) exit(1);
                    r->body_lines = nl;
                }
                r->body_lines[r->line_count] = strdup(fline);
                if (!r->body_lines[r->line_count]) exit(1);
                r->line_count++;
            }
        }
        free(fline);
        fclose(f);
    }

    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("Title: %s\n", items[i].title);
                    for (size_t j = 0; j < items[i].line_count; j++) {
                        printf("%s\n", items[i].body_lines[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    for (size_t j = 0; j < items[i].line_count; j++) {
                        free(items[i].body_lines[j]);
                    }
                    free(items[i].body_lines);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Record *new_items = realloc(items, capacity * sizeof(Record));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].title = strdup(title);
            if (!items[count].title) exit(1);
            items[count].body_lines = NULL;
            items[count].line_count = 0;
            items[count].line_capacity = 0;

            Record *r = &items[count];
            count++;

            char *subline = NULL;
            size_t sublen = 0;
            long subread;
            while ((subread = getline(&subline, &sublen, stdin)) != -1) {
                if (subread > 0 && subline[subread - 1] == '\n') {
                    subline[subread - 1] = '\0';
                    subread--;
                }
                if (subread > 0 && subline[subread - 1] == '\n') {
                    subline[subread - 1] = '\0';
                    subread--;
                }

                if (strcmp(subline, "--") == 0 || strcmp(subline, "END") == 0) {
                    break;
                }

                if (r->line_count >= r->line_capacity) {
                    r->line_capacity = r->line_capacity == 0 ? 4 : r->line_capacity * 2;
                    char **nl = realloc(r->body_lines, r->line_capacity * sizeof(char *));
                    if (!nl) exit(1);
                    r->body_lines = nl;
                }
                r->body_lines[r->line_count] = strdup(subline);
                if (!r->body_lines[r->line_count]) exit(1);
                r->line_count++;
            }
            free(subline);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Title: %s\n", items[i].title);
                for (size_t j = 0; j < items[i].line_count; j++) {
                    printf("%s\n", items[i].body_lines[j]);
                }
                if (i < count - 1) {
                    printf("--\n");
                }
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        for (size_t j = 0; j < items[i].line_count; j++) {
            free(items[i].body_lines[j]);
        }
        free(items[i].body_lines);
    }
    free(items);
    free(line);
    return 0;
}
