// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int body_count;
} Record;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Record *catalog = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *title = read_line(fp);
            if (!title) break;
            if (strcmp(title, "--") == 0 || strlen(title) == 0) {
                free(title);
                continue;
            }
            catalog = realloc(catalog, (count + 1) * sizeof(Record));
            catalog[count].title = title;
            catalog[count].body = NULL;
            catalog[count].body_count = 0;
            while (1) {
                char *bline = read_line(fp);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                int bc = catalog[count].body_count;
                catalog[count].body = realloc(catalog[count].body, (bc + 1) * sizeof(char *));
                catalog[count].body[bc] = bline;
                catalog[count].body_count++;
            }
            count++;
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (catalog[i].title && strcmp(catalog[i].title, title) == 0) {
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (catalog[i].title && strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        free(catalog[i].body[j]);
                    }
                    free(catalog[i].body);
                    catalog[i].title = NULL;
                    catalog[i].body = NULL;
                    catalog[i].body_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            catalog = realloc(catalog, (count + 1) * sizeof(Record));
            catalog[count].title = strdup(title);
            catalog[count].body = NULL;
            catalog[count].body_count = 0;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                int bc = catalog[count].body_count;
                catalog[count].body = realloc(catalog[count].body, (bc + 1) * sizeof(char *));
                catalog[count].body[bc] = bline;
                catalog[count].body_count++;
            }
            count++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                if (catalog[i].title) {
                    printf("%s\n", catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("  %s\n", catalog[i].body[j]);
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (catalog[i].title) {
            free(catalog[i].title);
            for (int j = 0; j < catalog[i].body_count; j++) {
                free(catalog[i].body[j]);
            }
            free(catalog[i].body);
        }
    }
    free(catalog);
    return 0;
}
