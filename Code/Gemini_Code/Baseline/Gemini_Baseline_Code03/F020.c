// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int lines;
} CatalogRecord;

int main(int argc, char *argv[]) {
    CatalogRecord *cat = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[1024];
            int in_record = 0;
            while (fgets(fline, sizeof(fline), f)) {
                int flen = strlen(fline);
                while (flen > 0 && (fline[flen - 1] == '\n' || fline[flen - 1] == '\r')) {
                    fline[flen - 1] = '\0';
                    flen--;
                }
                if (strcmp(fline, "--") == 0) {
                    in_record = 0;
                    continue;
                }
                if (!in_record) {
                    cat = realloc(cat, (count + 1) * sizeof(CatalogRecord));
                    cat[count].title = strdup(fline);
                    cat[count].body = NULL;
                    cat[count].lines = 0;
                    in_record = 1;
                    count++;
                } else {
                    int c = count - 1;
                    cat[c].body = realloc(cat[c].body, (cat[c].lines + 1) * sizeof(char *));
                    cat[c].body[cat[c].lines] = strdup(fline);
                    cat[c].lines++;
                }
            }
            fclose(f);
        }
    }
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
        if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cat[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    for (int j = 0; j < cat[found].lines; j++) {
                        printf("%s\n", cat[found].body[j]);
                    }
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cat[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(cat[found].title);
                    for (int j = 0; j < cat[found].lines; j++) {
                        free(cat[found].body[j]);
                    }
                    free(cat[found].body);
                    for (int i = found; i < count - 1; i++) {
                        cat[i] = cat[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        cat = realloc(cat, count * sizeof(CatalogRecord));
                    } else {
                        free(cat);
                        cat = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                cat = realloc(cat, (count + 1) * sizeof(CatalogRecord));
                cat[count].title = strdup(title);
                cat[count].body = NULL;
                cat[count].lines = 0;
                count++;
                char bline[1024];
                while (fgets(bline, sizeof(bline), stdin)) {
                    int blen = strlen(bline);
                    while (blen > 0 && (bline[blen - 1] == '\n' || bline[blen - 1] == '\r')) {
                        bline[blen - 1] = '\0';
                        blen--;
                    }
                    if (strcmp(bline, "--") == 0) {
                        break;
                    }
                    int c = count - 1;
                    cat[c].body = realloc(cat[c].body, (cat[c].lines + 1) * sizeof(char *));
                    cat[c].body[cat[c].lines] = strdup(bline);
                    cat[c].lines++;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Title: %s\n", cat[i].title);
                for (int j = 0; j < cat[i].lines; j++) {
                    printf("  %s\n", cat[i].body[j]);
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(cat[i].title);
        for (int j = 0; j < cat[i].lines; j++) {
            free(cat[i].body[j]);
        }
        free(cat[i].body);
    }
    free(cat);
    return 0;
}
