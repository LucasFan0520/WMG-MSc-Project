// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int bcount;
} CatalogRecord;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    CatalogRecord *cat = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    if (fp) {
        char *title = NULL;
        char **body = NULL;
        int bcount = 0;
        int state = 0;
        while ((read = getline(&line, &cap, fp)) != -1) {
            line[strcspn(line, "\r\n")] = 0;
            if (strcmp(line, "--") == 0) {
                if (title) {
                    CatalogRecord *tmp = realloc(cat, (count + 1) * sizeof(CatalogRecord));
                    if (tmp) {
                        cat = tmp;
                        cat[count].title = title;
                        cat[count].body = body;
                        cat[count].bcount = bcount;
                        count++;
                    }
                    title = NULL;
                    body = NULL;
                    bcount = 0;
                }
                state = 0;
            } else {
                if (state == 0) {
                    title = strdup(line);
                    state = 1;
                } else {
                    char **tmp = realloc(body, (bcount + 1) * sizeof(char *));
                    if (tmp) {
                        body = tmp;
                        body[bcount] = strdup(line);
                        bcount++;
                    }
                }
            }
        }
        if (title) {
            CatalogRecord *tmp = realloc(cat, (count + 1) * sizeof(CatalogRecord));
            if (tmp) {
                cat = tmp;
                cat[count].title = title;
                cat[count].body = body;
                cat[count].bcount = bcount;
                count++;
            }
        }
        fclose(fp);
    }
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') {
                title++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(cat[i].title, title) == 0) {
                    for (int k = 0; k < cat[i].bcount; k++) {
                        printf("%s\n", cat[i].body[k]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') {
                title++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(cat[i].title, title) == 0) {
                    free(cat[i].title);
                    for (int k = 0; k < cat[i].bcount; k++) {
                        free(cat[i].body[k]);
                    }
                    free(cat[i].body);
                    for (int j = i; j < count - 1; j++) {
                        cat[j] = cat[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            while (*title == ' ') {
                title++;
            }
            char *tcopy = strdup(title);
            char **body = NULL;
            int bcount = 0;
            char *bline = NULL;
            size_t bcap = 0;
            while (getline(&bline, &bcap, stdin) != -1) {
                bline[strcspn(bline, "\r\n")] = 0;
                if (strcmp(bline, "--") == 0) {
                    break;
                }
                char **tmp = realloc(body, (bcount + 1) * sizeof(char *));
                if (tmp) {
                    body = tmp;
                    body[bcount] = strdup(bline);
                    bcount++;
                }
            }
            free(bline);
            CatalogRecord *tmp = realloc(cat, (count + 1) * sizeof(CatalogRecord));
            if (tmp) {
                cat = tmp;
                cat[count].title = tcopy;
                cat[count].body = body;
                cat[count].bcount = bcount;
                count++;
            } else {
                free(tcopy);
                for (int k = 0; k < bcount; k++) {
                    free(body[k]);
                }
                free(body);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", cat[i].title);
                for (int k = 0; k < cat[i].bcount; k++) {
                    printf("%s\n", cat[i].body[k]);
                }
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(cat[i].title);
        for (int k = 0; k < cat[i].bcount; k++) {
            free(cat[i].body[k]);
        }
        free(cat[i].body);
    }
    free(cat);
    return 0;
}
