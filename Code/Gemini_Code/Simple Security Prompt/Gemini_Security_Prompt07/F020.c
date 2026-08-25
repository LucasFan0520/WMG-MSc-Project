// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            char fline[4096];
            int is_title = 1;
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\r\n")] = 0;
                if (strcmp(fline, "--") == 0) {
                    is_title = 1;
                    continue;
                }
                if (is_title) {
                    catalog = realloc(catalog, (count + 1) * sizeof(Record));
                    catalog[count].title = strdup(fline);
                    catalog[count].body = NULL;
                    catalog[count].body_count = 0;
                    count++;
                    is_title = 0;
                } else {
                    Record *r = &catalog[count - 1];
                    r->body = realloc(r->body, (r->body_count + 1) * sizeof(char *));
                    r->body[r->body_count] = strdup(fline);
                    r->body_count++;
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, "");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        printf("Title: %s\n", catalog[i].title);
                        for (int j = 0; j < catalog[i].body_count; j++) {
                            printf("%s\n", catalog[i].body[j]);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, "");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        free(catalog[i].title);
                        for (int j = 0; j < catalog[i].body_count; j++) {
                            free(catalog[i].body[j]);
                        }
                        free(catalog[i].body);
                        for (int j = i; j < count - 1; j++) {
                            catalog[j] = catalog[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(catalog);
                    catalog = NULL;
                } else {
                    catalog = realloc(catalog, count * sizeof(Record));
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = strtok(NULL, "");
            if (title) {
                catalog = realloc(catalog, (count + 1) * sizeof(Record));
                catalog[count].title = strdup(title);
                catalog[count].body = NULL;
                catalog[count].body_count = 0;
                count++;
                char bline[4096];
                while (fgets(bline, sizeof(bline), stdin)) {
                    bline[strcspn(bline, "\r\n")] = 0;
                    if (strcmp(bline, "--") == 0) {
                        break;
                    }
                    Record *r = &catalog[count - 1];
                    r->body = realloc(r->body, (r->body_count + 1) * sizeof(char *));
                    r->body[r->body_count] = strdup(bline);
                    r->body_count++;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].body_count; j++) {
                    printf("%s\n", catalog[i].body[j]);
                }
                printf("--\n");
            }
        }
    }
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
