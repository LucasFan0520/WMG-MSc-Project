// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int b_count;
} Record;

int main(int argc, char **argv) {
    Record *catalog = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            int new_record = 1;
            while ((read = getline(&line, &len, f)) != -1) {
                if (read > 0 && line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                if (strcmp(line, "--") == 0) {
                    new_record = 1;
                    continue;
                }
                if (new_record) {
                    Record *tmp = realloc(catalog, sizeof(Record) * (count + 1));
                    if (tmp) {
                        catalog = tmp;
                        catalog[count].title = strdup(line);
                        catalog[count].body = NULL;
                        catalog[count].b_count = 0;
                        if (catalog[count].title) {
                            count++;
                        }
                    }
                    new_record = 0;
                } else {
                    if (count > 0) {
                        char **b_tmp = realloc(catalog[count - 1].body, sizeof(char *) * (catalog[count - 1].b_count + 1));
                        if (b_tmp) {
                            catalog[count - 1].body = b_tmp;
                            catalog[count - 1].body[catalog[count - 1].b_count] = strdup(line);
                            if (catalog[count - 1].body[catalog[count - 1].b_count]) {
                                catalog[count - 1].b_count++;
                            }
                        }
                    }
                }
            }
            fclose(f);
        }
    }

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    printf("%s\n", catalog[i].title);
                    for (int j = 0; j < catalog[i].b_count; j++) {
                        printf("%s\n", catalog[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(catalog[found].title);
                for (int j = 0; j < catalog[found].b_count; j++) {
                    free(catalog[found].body[j]);
                }
                free(catalog[found].body);
                for (int i = found; i < count - 1; i++) {
                    catalog[i] = catalog[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            Record *tmp = realloc(catalog, sizeof(Record) * (count + 1));
            if (tmp) {
                catalog = tmp;
                catalog[count].title = strdup(title);
                catalog[count].body = NULL;
                catalog[count].b_count = 0;
                if (catalog[count].title) {
                    int idx = count;
                    count++;
                    char *b_line = NULL;
                    size_t b_len = 0;
                    ssize_t b_read;
                    while ((b_read = getline(&b_line, &b_len, stdin)) != -1) {
                        if (b_read > 0 && b_line[b_read - 1] == '\n') {
                            b_line[b_read - 1] = '\0';
                        }
                        if (strcmp(b_line, "--") == 0 || strcmp(b_line, "END") == 0) {
                            break;
                        }
                        char **b_tmp = realloc(catalog[idx].body, sizeof(char *) * (catalog[idx].b_count + 1));
                        if (b_tmp) {
                            catalog[idx].body = b_tmp;
                            catalog[idx].body[catalog[idx].b_count] = strdup(b_line);
                            if (catalog[idx].body[catalog[idx].b_count]) {
                                catalog[idx].b_count++;
                            }
                        }
                    }
                    free(b_line);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].b_count; j++) {
                    printf("%s\n", catalog[i].body[j]);
                }
                if (i < count - 1) {
                    printf("--\n");
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].b_count; j++) {
            free(catalog[i].body[j]);
        }
        free(catalog[i].body);
    }
    free(catalog);
    free(line);
    return 0;
}
