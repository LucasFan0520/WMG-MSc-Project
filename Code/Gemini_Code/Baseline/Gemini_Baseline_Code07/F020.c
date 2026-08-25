// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char **body;
    int body_count;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    Record *catalog = NULL;
    int count = 0;
    int capacity = 0;
    if (fp) {
        int reading_record = 0;
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            if (strcmp(line, "--") == 0) {
                reading_record = 0;
                free(line);
                continue;
            }
            if (reading_record == 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    catalog = realloc(catalog, capacity * sizeof(Record));
                }
                catalog[count].title = strdup(line);
                catalog[count].body = NULL;
                catalog[count].body_count = 0;
                reading_record = 1;
                count++;
            } else {
                int idx = count - 1;
                catalog[idx].body = realloc(catalog[idx].body, (catalog[idx].body_count + 1) * sizeof(char *));
                catalog[idx].body[catalog[idx].body_count++] = strdup(line);
            }
            free(line);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].body_count; j++) {
                    printf("%s\n", catalog[i].body[j]);
                }
                if (i < count - 1) printf("--\n");
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                catalog = realloc(catalog, capacity * sizeof(Record));
            }
            catalog[count].title = strdup(p);
            catalog[count].body = NULL;
            catalog[count].body_count = 0;
            count++;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline || strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                int idx = count - 1;
                catalog[idx].body = realloc(catalog[idx].body, (catalog[idx].body_count + 1) * sizeof(char *));
                catalog[idx].body[catalog[idx].body_count++] = strdup(bline);
                free(bline);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, p) == 0) {
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, p) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(catalog[found].title);
                for (int j = 0; j < catalog[found].body_count; j++) {
                    free(catalog[found].body[j]);
                }
                free(catalog[found].body);
                for (int i = found; i < count - 1; i++) {
                    catalog[i] = catalog[i + 1];
                }
                count--;
            }
        }
        free(line);
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
