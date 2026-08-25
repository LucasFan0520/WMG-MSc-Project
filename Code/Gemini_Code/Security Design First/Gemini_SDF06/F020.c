// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body_lines;
    size_t body_count;
    size_t body_capacity;
} CatalogRecord;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    CatalogRecord *catalog = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    int reading_title = 1;
    CatalogRecord cur;
    cur.title = NULL;
    cur.body_lines = NULL;
    cur.body_count = 0;
    cur.body_capacity = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "--") == 0) {
            if (cur.title) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    CatalogRecord *new_cat = realloc(catalog, new_cap * sizeof(CatalogRecord));
                    if (new_cat) {
                        catalog = new_cat;
                        capacity = new_cap;
                    } else {
                        break;
                    }
                }
                catalog[count] = cur;
                count++;
                cur.title = NULL;
                cur.body_lines = NULL;
                cur.body_count = 0;
                cur.body_capacity = 0;
            }
            reading_title = 1;
        } else {
            if (reading_title) {
                cur.title = strdup(line);
                reading_title = 0;
            } else {
                if (cur.body_count >= cur.body_capacity) {
                    size_t new_cap = cur.body_capacity == 0 ? 4 : cur.body_capacity * 2;
                    char **new_body = realloc(cur.body_lines, new_cap * sizeof(char *));
                    if (new_body) {
                        cur.body_lines = new_body;
                        cur.body_capacity = new_cap;
                    } else {
                        continue;
                    }
                }
                char *bl = strdup(line);
                if (bl) {
                    cur.body_lines[cur.body_count] = bl;
                    cur.body_count++;
                }
            }
        }
    }
    if (cur.title) {
        if (count >= capacity) {
            size_t new_cap = capacity + 1;
            CatalogRecord *new_cat = realloc(catalog, new_cap * sizeof(CatalogRecord));
            if (new_cat) {
                catalog = new_cat;
                capacity = new_cap;
            }
        }
        catalog[count] = cur;
        count++;
    }
    fclose(fp);

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            if (*title == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    printf("%s\n", catalog[i].title);
                    for (size_t j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body_lines[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            if (*title == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (size_t j = 0; j < catalog[i].body_count; j++) {
                        free(catalog[i].body_lines[j]);
                    }
                    free(catalog[i].body_lines);
                    for (size_t j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            if (*title == '\0') continue;

            CatalogRecord new_rec;
            new_rec.title = strdup(title);
            new_rec.body_lines = NULL;
            new_rec.body_count = 0;
            new_rec.body_capacity = 0;

            while ((read = getline(&line, &len, stdin)) != -1) {
                line[strcspn(line, "\r\n")] = '\0';
                if (strcmp(line, "--") == 0) {
                    break;
                }
                if (new_rec.body_count >= new_rec.body_capacity) {
                    size_t new_cap = new_rec.body_capacity == 0 ? 4 : new_rec.body_capacity * 2;
                    char **new_body = realloc(new_rec.body_lines, new_cap * sizeof(char *));
                    if (new_body) {
                        new_rec.body_lines = new_body;
                        new_rec.body_capacity = new_cap;
                    } else {
                        continue;
                    }
                }
                char *bl = strdup(line);
                if (bl) {
                    new_rec.body_lines[new_rec.body_count] = bl;
                    new_rec.body_count++;
                }
            }

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                CatalogRecord *new_cat = realloc(catalog, new_cap * sizeof(CatalogRecord));
                if (new_cat) {
                    catalog = new_cat;
                    capacity = new_cap;
                } else {
                    free(new_rec.title);
                    for (size_t j = 0; j < new_rec.body_count; j++) free(new_rec.body_lines[j]);
                    free(new_rec.body_lines);
                    continue;
                }
            }
            catalog[count] = new_rec;
            count++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (size_t j = 0; j < catalog[i].body_count; j++) {
                    printf("%s\n", catalog[i].body_lines[j]);
                }
                if (i < count - 1) printf("--\n");
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(catalog[i].title);
        for (size_t j = 0; j < catalog[i].body_count; j++) {
            free(catalog[i].body_lines[j]);
        }
        free(catalog[i].body_lines);
    }
    free(catalog);
    free(line);
    return 0;
}
