/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} CatalogRecord;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    CatalogRecord *catalog = NULL;
    int count = 0;
    int capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread_bytes;
        char *current_title = NULL;
        char *current_body = NULL;
        size_t body_len = 0;

        while ((fread_bytes = getline(&fline, &flen, fp)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            if (strcmp(fline, "--") == 0) {
                if (current_title) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        CatalogRecord *tmp = realloc(catalog, new_cap * sizeof(CatalogRecord));
                        if (tmp) {
                            catalog = tmp;
                            capacity = new_cap;
                        } else {
                            free(current_title);
                            free(current_body);
                            current_title = NULL;
                            current_body = NULL;
                            body_len = 0;
                            continue;
                        }
                    }
                    catalog[count].title = current_title;
                    catalog[count].body = current_body ? current_body : strdup("");
                    count++;
                    current_title = NULL;
                    current_body = NULL;
                    body_len = 0;
                }
            } else {
                if (!current_title) {
                    current_title = strdup(fline);
                } else {
                    size_t line_l = strlen(fline);
                    if (!current_body) {
                        current_body = malloc(line_l + 2);
                        if (current_body) {
                            strcpy(current_body, fline);
                            strcat(current_body, "\n");
                            body_len = line_l + 1;
                        }
                    } else {
                        char *tmp = realloc(current_body, body_len + line_l + 2);
                        if (tmp) {
                            current_body = tmp;
                            strcpy(current_body + body_len, fline);
                            strcat(current_body + body_len, "\n");
                            body_len += line_l + 1;
                        }
                    }
                }
            }
        }
        if (current_title) {
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                CatalogRecord *tmp = realloc(catalog, new_cap * sizeof(CatalogRecord));
                if (tmp) {
                    catalog = tmp;
                    catalog[count].title = current_title;
                    catalog[count].body = current_body ? current_body : strdup("");
                    count++;
                } else {
                    free(current_title);
                    free(current_body);
                }
            } else {
                catalog[count].title = current_title;
                catalog[count].body = current_body ? current_body : strdup("");
                count++;
            }
        }
        free(fline);
        fclose(fp);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *t = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, t) == 0) {
                    printf("%s", catalog[i].body);
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *t = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, t) == 0) {
                    free(catalog[i].title);
                    free(catalog[i].body);
                    for (int j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *t = line + 4;
            char *n_title = strdup(t);
            char *n_body = NULL;
            size_t blen = 0;
            char *subline = NULL;
            size_t sublen = 0;
            ssize_t subread;
            while ((subread = getline(&subline, &sublen, stdin)) != -1) {
                subline[strcspn(subline, "\r\n")] = '\0';
                if (strcmp(subline, "--") == 0) {
                    break;
                }
                size_t slen = strlen(subline);
                if (!n_body) {
                    n_body = malloc(slen + 2);
                    if (n_body) {
                        strcpy(n_body, subline);
                        strcat(n_body, "\n");
                        blen = slen + 1;
                    }
                } else {
                    char *tmp = realloc(n_body, blen + slen + 2);
                    if (tmp) {
                        n_body = tmp;
                        strcpy(n_body + blen, subline);
                        strcat(n_body + blen, "\n");
                        blen += slen + 1;
                    }
                }
            }
            free(subline);
            if (n_title) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    CatalogRecord *tmp = realloc(catalog, new_cap * sizeof(CatalogRecord));
                    if (tmp) {
                        catalog = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_title);
                        free(n_body);
                        continue;
                    }
                }
                catalog[count].title = n_title;
                catalog[count].body = n_body ? n_body : strdup("");
                count++;
            } else {
                free(n_body);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n%s--\n", catalog[i].title, catalog[i].body);
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
