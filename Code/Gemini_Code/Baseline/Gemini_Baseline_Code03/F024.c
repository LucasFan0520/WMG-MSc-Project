// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    int auth_count;
} Book;

int main() {
    Book *catalog = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year = 0;
            if (sscanf(ptr, "%255s %d", title, &year) == 2) {
                catalog = realloc(catalog, (count + 1) * sizeof(Book));
                catalog[count].title = strdup(title);
                catalog[count].year = year;
                catalog[count].authors = NULL;
                catalog[count].auth_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(ptr, "%255s %255s", title, name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        int ac = catalog[i].auth_count;
                        catalog[i].authors = realloc(catalog[i].authors, (ac + 1) * sizeof(char *));
                        catalog[i].authors[ac] = strdup(name);
                        catalog[i].auth_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(ptr, "%255s %255s", title, name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        int f_idx = -1;
                        for (int j = 0; j < catalog[i].auth_count; j++) {
                            if (strcmp(catalog[i].authors[j], name) == 0) {
                                f_idx = j;
                                break;
                            }
                        }
                        if (f_idx != -1) {
                            free(catalog[i].authors[f_idx]);
                            for (int j = f_idx; j < catalog[i].auth_count - 1; j++) {
                                catalog[i].authors[j] = catalog[i].authors[j + 1];
                            }
                            catalog[i].auth_count--;
                            if (catalog[i].auth_count > 0) {
                                catalog[i].authors = realloc(catalog[i].authors, catalog[i].auth_count * sizeof(char *));
                            } else {
                                free(catalog[i].authors);
                                catalog[i].authors = NULL;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(catalog[found].title);
                    for (int j = 0; j < catalog[found].auth_count; j++) {
                        free(catalog[found].authors[j]);
                    }
                    free(catalog[found].authors);
                    for (int i = found; i < count - 1; i++) {
                        catalog[i] = catalog[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        catalog = realloc(catalog, count * sizeof(Book));
                    } else {
                        free(catalog);
                        catalog = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("Book: %s (%d) Authors:", catalog[found].title, catalog[found].year);
                    for (int j = 0; j < catalog[found].auth_count; j++) {
                        printf(" %s", catalog[found].authors[j]);
                    }
                    printf("\n");
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s (%d)\n", catalog[i].title, catalog[i].year);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].auth_count; j++) {
            free(catalog[i].authors[j]);
        }
        free(catalog[i].authors);
    }
    free(catalog);
    return 0;
}
