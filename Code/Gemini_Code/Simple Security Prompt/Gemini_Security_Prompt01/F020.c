// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body_lines;
    int line_count;
} CatalogRecord;

CatalogRecord *catalog = NULL;
int record_count = 0;

void free_record_contents(CatalogRecord *r) {
    free(r->title);
    for (int i = 0; i < r->line_count; i++) {
        free(r->body_lines[i]);
    }
    free(r->body_lines);
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            int state = 0; // 0=expect title, 1=expect body
            while ((fread = getline(&fline, &flen, fp)) != -1) {
                fline[strcspn(fline, "\r\n")] = '\0';
                if (strcmp(fline, "--") == 0) {
                    state = 0;
                    continue;
                }
                if (state == 0) {
                    catalog = realloc(catalog, (record_count + 1) * sizeof(CatalogRecord));
                    if (!catalog) return 1;
                    catalog[record_count].title = strdup(fline);
                    catalog[record_count].body_lines = NULL;
                    catalog[record_count].line_count = 0;
                    record_count++;
                    state = 1;
                } else {
                    CatalogRecord *cur = &catalog[record_count - 1];
                    cur->body_lines = realloc(cur->body_lines, (cur->line_count + 1) * sizeof(char *));
                    if (!cur->body_lines) return 1;
                    cur->body_lines[cur->line_count] = strdup(fline);
                    cur->line_count++;
                }
            }
            free(fline);
            fclose(fp);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < record_count; i++) {
                if (strcmp(catalog[i].title, args) == 0) {
                    printf("%s\n", catalog[i].title);
                    for (int j = 0; j < catalog[i].line_count; j++) {
                        printf("%s\n", catalog[i].body_lines[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < record_count; i++) {
                if (strcmp(catalog[i].title, args) == 0) {
                    free_record_contents(&catalog[i]);
                    for (int j = i; j < record_count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    record_count--;
                    catalog = realloc(catalog, record_count * sizeof(CatalogRecord));
                    if (record_count > 0 && !catalog) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0 && args) {
            catalog = realloc(catalog, (record_count + 1) * sizeof(CatalogRecord));
            if (!catalog) return 1;
            catalog[record_count].title = strdup(args);
            catalog[record_count].body_lines = NULL;
            catalog[record_count].line_count = 0;
            record_count++;

            char *bline = NULL;
            size_t blen = 0;
            ssize_t bread;
            while ((bread = getline(&bline, &blen, stdin)) != -1) {
                bline[strcspn(bline, "\r\n")] = '\0';
                if (strcmp(bline, "--") == 0) {
                    break;
                }
                CatalogRecord *cur = &catalog[record_count - 1];
                cur->body_lines = realloc(cur->body_lines, (cur->line_count + 1) * sizeof(char *));
                if (!cur->body_lines) return 1;
                cur->body_lines[cur->line_count] = strdup(bline);
                cur->line_count++;
            }
            free(bline);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < record_count; i++) {
                printf("%s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].line_count; j++) {
                    printf("%s\n", catalog[i].body_lines[j]);
                }
                if (i < record_count - 1) {
                    printf("--\n");
                }
            }
        }
    }

    free(line);
    for (int i = 0; i < record_count; i++) {
        free_record_contents(&catalog[i]);
    }
    free(catalog);
    return 0;
}
