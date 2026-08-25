// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    char **body;
    int body_count;
} Record;

int main(int argc, char **argv) {
    Record *catalog = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            int reading_record = 0;
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                if (strcmp(line, "--") == 0) {
                    reading_record = 0;
                    free(line);
                    continue;
                }
                if (reading_record == 0) {
                    catalog = realloc(catalog, sizeof(Record) * (count + 1));
                    catalog[count].title = strdup(line);
                    catalog[count].body = NULL;
                    catalog[count].body_count = 0;
                    count++;
                    reading_record = 1;
                } else {
                    int rc = catalog[count - 1].body_count;
                    catalog[count - 1].body = realloc(catalog[count - 1].body, sizeof(char*) * (rc + 1));
                    catalog[count - 1].body[rc] = strdup(line);
                    catalog[count - 1].body_count++;
                }
                free(line);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body[j]);
                    }
                    if (i < count - 1) printf("--\n");
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *title = p1 + 1;
        if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) free(catalog[i].body[j]);
                    free(catalog[i].body);
                    for (int j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    if (count == 0) { free(catalog); catalog = NULL; }
                    else { catalog = realloc(catalog, sizeof(Record) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            catalog = realloc(catalog, sizeof(Record) * (count + 1));
            catalog[count].title = strdup(title);
            catalog[count].body = NULL;
            catalog[count].body_count = 0;
            count++;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) { free(bline); break; }
                int rc = catalog[count - 1].body_count;
                catalog[count - 1].body = realloc(catalog[count - 1].body, sizeof(char*) * (rc + 1));
                catalog[count - 1].body[rc] = strdup(bline);
                catalog[count - 1].body_count++;
                free(bline);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].body_count; j++) free(catalog[i].body[j]);
        free(catalog[i].body);
    }
    free(catalog);
    return 0;
}
