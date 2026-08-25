// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    size_t body_count;
} CatalogRecord;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    CatalogRecord *list = NULL;
    size_t count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    if (fp) {
        int new_record = 1;
        while ((read = getline(&line, &line_len, fp)) != -1) {
            if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
            if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
            if (strcmp(line, "--") == 0) {
                new_record = 1;
                continue;
            }
            if (new_record) {
                CatalogRecord *temp = realloc(list, (count + 1) * sizeof(CatalogRecord));
                if (temp) {
                    list = temp;
                    list[count].title = strdup(line);
                    list[count].body = NULL;
                    list[count].body_count = 0;
                    count++;
                }
                new_record = 0;
            } else {
                if (count > 0) {
                    char **temp = realloc(list[count - 1].body, (list[count - 1].body_count + 1) * sizeof(char *));
                    if (temp) {
                        list[count - 1].body = temp;
                        list[count - 1].body[list[count - 1].body_count] = strdup(line);
                        list[count - 1].body_count++;
                    }
                }
            }
        }
        fclose(fp);
    }
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    printf("Title: %s\n", list[i].title);
                    for (size_t k = 0; k < list[i].body_count; k++) {
                        printf("%s\n", list[i].body[k]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(list[i].title);
                    for (size_t k = 0; k < list[i].body_count; k++) {
                        free(list[i].body[k]);
                    }
                    free(list[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            CatalogRecord *temp = realloc(list, (count + 1) * sizeof(CatalogRecord));
            if (temp) {
                list = temp;
                list[count].title = strdup(title);
                list[count].body = NULL;
                list[count].body_count = 0;
                count++;
                while ((read = getline(&line, &line_len, stdin)) != -1) {
                    if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
                    if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
                    if (strcmp(line, "--") == 0) {
                        break;
                    }
                    char **temp_b = realloc(list[count - 1].body, (list[count - 1].body_count + 1) * sizeof(char *));
                    if (temp_b) {
                        list[count - 1].body = temp_b;
                        list[count - 1].body[list[count - 1].body_count] = strdup(line);
                        list[count - 1].body_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (i > 0) printf("--\n");
                printf("%s\n", list[i].title);
                for (size_t k = 0; k < list[i].body_count; k++) {
                    printf("%s\n", list[i].body[k]);
                }
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        for (size_t k = 0; k < list[i].body_count; k++) {
            free(list[i].body[k]);
        }
        free(list[i].body);
    }
    free(list);
    free(line);
    return 0;
}
