// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} CatalogRecord;

CatalogRecord *catalog = NULL;
int count = 0;
int capacity = 0;

void add_record(const char *title, const char *body) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        CatalogRecord *tmp = realloc(catalog, capacity * sizeof(CatalogRecord));
        if (!tmp) return;
        catalog = tmp;
    }
    catalog[count].title = strdup(title);
    catalog[count].body = strdup(body);
    if (catalog[count].title && catalog[count].body) {
        count++;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t nread;
        char *cur_title = NULL;
        char *cur_body = NULL;
        size_t b_cap = 0;
        size_t b_len = 0;
        while ((nread = getline(&fline, &flen, f)) != -1) {
            size_t l = strlen(fline);
            if (l > 0 && fline[l - 1] == '\n') fline[l - 1] = '\0';
            if (strcmp(fline, "--") == 0) {
                if (cur_title) {
                    add_record(cur_title, cur_body ? cur_body : "");
                    free(cur_title);
                    free(cur_body);
                    cur_title = NULL;
                    cur_body = NULL;
                    b_cap = 0;
                    b_len = 0;
                }
            } else {
                if (!cur_title) {
                    cur_title = strdup(fline);
                } else {
                    size_t fl = strlen(fline);
                    if (b_len + fl + 2 > b_cap) {
                        b_cap = b_cap == 0 ? 128 : b_cap * 2 + fl + 2;
                        char *nb = realloc(cur_body, b_cap);
                        if (nb) cur_body = nb;
                    }
                    if (b_len == 0) {
                        if (cur_body) strcpy(cur_body, fline);
                        b_len = fl;
                    } else {
                        if (cur_body) {
                            strcat(cur_body, "\n");
                            strcat(cur_body, fline);
                        }
                        b_len += 1 + fl;
                    }
                }
            }
        }
        if (cur_title) {
            add_record(cur_title, cur_body ? cur_body : "");
            free(cur_title);
            free(cur_body);
        }
        free(fline);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') line[l - 1] = '\0';
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "FIND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, arg1) == 0) {
                        printf("%s\n", catalog[i].body);
                        break;
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, arg1) == 0) {
                        free(catalog[i].title);
                        free(catalog[i].body);
                        for (int j = i; j < count - 1; j++) {
                            catalog[j] = catalog[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            } else if (strcmp(cmd, "ADD") == 0) {
                char *cur_body = NULL;
                size_t b_cap = 0;
                size_t b_len = 0;
                char *stdin_line = NULL;
                size_t stdin_len = 0;
                ssize_t sread;
                while ((sread = getline(&stdin_line, &stdin_len, stdin)) != -1) {
                    size_t sl = strlen(stdin_line);
                    if (sl > 0 && stdin_line[sl - 1] == '\n') stdin_line[sl - 1] = '\0';
                    if (strcmp(stdin_line, "--") == 0) {
                        break;
                    }
                    size_t fl = strlen(stdin_line);
                    if (b_len + fl + 2 > b_cap) {
                        b_cap = b_cap == 0 ? 128 : b_cap * 2 + fl + 2;
                        char *nb = realloc(cur_body, b_cap);
                        if (nb) cur_body = nb;
                    }
                    if (b_len == 0) {
                        if (cur_body) strcpy(cur_body, stdin_line);
                        b_len = fl;
                    } else {
                        if (cur_body) {
                            strcat(cur_body, "\n");
                            strcat(cur_body, stdin_line);
                        }
                        b_len += 1 + fl;
                    }
                }
                add_record(arg1, cur_body ? cur_body : "");
                free(cur_body);
                free(stdin_line);
            }
        } else {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n%s\n--\n", catalog[i].title, catalog[i].body);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        free(catalog[i].body);
    }
    free(catalog);
    return 0;
}