// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int body_count;
    int body_capacity;
} CatalogRecord;

CatalogRecord *catalog = NULL;
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

void add_record_empty(const char *title) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        catalog = realloc(catalog, capacity * sizeof(CatalogRecord));
    }
    catalog[count].title = strdup(title);
    catalog[count].body = NULL;
    catalog[count].body_count = 0;
    catalog[count].body_capacity = 0;
    count++;
}

void add_body_line(int idx, const char *line) {
    CatalogRecord *r = &catalog[idx];
    if (r->body_count >= r->body_capacity) {
        r->body_capacity = r->body_capacity == 0 ? 4 : r->body_capacity * 2;
        r->body = realloc(r->body, r->body_capacity * sizeof(char *));
    }
    r->body[r->body_count] = strdup(line);
    r->body_count++;
}

void delete_record(const char *title) {
    int idx = find_record(title);
    if (idx != -1) {
        free(catalog[idx].title);
        for (int i = 0; i < catalog[idx].body_count; i++) {
            free(catalog[idx].body[i]);
        }
        free(catalog[idx].body);
        for (int i = idx; i < count - 1; i++) {
            catalog[i] = catalog[i + 1];
        }
        count--;
    }
}

void print_record(int idx) {
    printf("%s\n", catalog[idx].title);
    for (int i = 0; i < catalog[idx].body_count; i++) {
        printf("%s\n", catalog[idx].body[i]);
    }
}

void find_record_p(const char *title) {
    int idx = find_record(title);
    if (idx != -1) {
        print_record(idx);
    }
}

void print_catalog(void) {
    for (int i = 0; i < count; i++) {
        print_record(i);
        if (i < count - 1) {
            printf("--\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *f_line = NULL;
            size_t f_len = 0;
            ssize_t f_read;
            int state = 0;
            int curr_idx = -1;
            while ((f_read = getline(&f_line, &f_len, fp)) != -1) {
                if (f_read > 0 && f_line[f_read - 1] == '\n') {
                    f_line[f_read - 1] = '\0';
                }
                if (strcmp(f_line, "--") == 0) {
                    state = 0;
                } else {
                    if (state == 0) {
                        add_record_empty(f_line);
                        curr_idx = count - 1;
                        state = 1;
                    } else {
                        add_body_line(curr_idx, f_line);
                    }
                }
            }
            free(f_line);
            fclose(fp);
        }
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, "");
            if (title) find_record_p(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, "");
            if (title) delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = strtok(NULL, "");
            if (title) {
                add_record_empty(title);
                int curr_idx = count - 1;
                char *b_line = NULL;
                size_t b_len = 0;
                ssize_t b_read;
                while ((b_read = getline(&b_line, &b_len, stdin)) != -1) {
                    if (b_read > 0 && b_line[b_read - 1] == '\n') {
                        b_line[b_read - 1] = '\0';
                    }
                    if (strcmp(b_line, "--") == 0) {
                        break;
                    }
                    add_body_line(curr_idx, b_line);
                }
                free(b_line);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_catalog();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].body_count; j++) {
            free(catalog[i].body[j]);
        }
        free(catalog[i].body);
    }
    free(catalog);
    return 0;
}
