// F020.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int b_count;
    int b_capacity;
} Chunk;

int main(int argc, char **argv) {
    Chunk *catalog = NULL;
    int count = 0;
    int capacity = 0;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            int in_body = 0;
            Chunk current_chunk = {NULL, NULL, 0, 0};
            while ((fread = getline(&fline, &flen, f)) != -1) {
                if (fread > 0 && fline[fread - 1] == '\n') fline[fread - 1] = '\0';
                if (strcmp(fline, "--") == 0) {
                    if (current_chunk.title) {
                        if (count >= capacity) {
                            int new_cap = capacity == 0 ? 4 : capacity * 2;
                            Chunk *tmp = realloc(catalog, new_cap * sizeof(Chunk));
                            if (!tmp) return 1;
                            catalog = tmp;
                            capacity = new_cap;
                        }
                        catalog[count++] = current_chunk;
                        current_chunk.title = NULL;
                        current_chunk.body = NULL;
                        current_chunk.b_count = 0;
                        current_chunk.b_capacity = 0;
                    }
                    in_body = 0;
                } else {
                    if (!in_body) {
                        current_chunk.title = strdup(fline);
                        in_body = 1;
                    } else {
                        if (current_chunk.b_count >= current_chunk.b_capacity) {
                            int new_bcap = current_chunk.b_capacity == 0 ? 4 : current_chunk.b_capacity * 2;
                            char **tmp = realloc(current_chunk.body, new_bcap * sizeof(char *));
                            if (!tmp) return 1;
                            current_chunk.body = tmp;
                            current_chunk.b_capacity = new_bcap;
                        }
                        current_chunk.body[current_chunk.b_count++] = strdup(fline);
                    }
                }
            }
            if (current_chunk.title) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Chunk *tmp = realloc(catalog, new_cap * sizeof(Chunk));
                    if (!tmp) return 1;
                    catalog = tmp;
                    capacity = new_cap;
                }
                catalog[count++] = current_chunk;
            }
            free(fline);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int j = 0; j < catalog[found].b_count; j++) {
                    printf("%s\n", catalog[found].body[j]);
                }
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(catalog[found].title);
                for (int j = 0; j < catalog[found].b_count; j++) free(catalog[found].body[j]);
                free(catalog[found].body);
                for (int i = found; i < count - 1; i++) {
                    catalog[i] = catalog[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            while (*title == ' ') title++;
            Chunk current_chunk;
            current_chunk.title = strdup(title);
            current_chunk.body = NULL;
            current_chunk.b_count = 0;
            current_chunk.b_capacity = 0;
            
            char *subline = NULL;
            size_t sublen = 0;
            ssize_t subread;
            while ((subread = getline(&subline, &sublen, stdin)) != -1) {
                if (subread > 0 && subline[subread - 1] == '\n') subline[subread - 1] = '\0';
                if (strcmp(subline, "--") == 0) break;
                if (current_chunk.b_count >= current_chunk.b_capacity) {
                    int new_bcap = current_chunk.b_capacity == 0 ? 4 : current_chunk.b_capacity * 2;
                    char **tmp = realloc(current_chunk.body, new_bcap * sizeof(char *));
                    if (!tmp) return 1;
                    current_chunk.body = tmp;
                    current_chunk.b_capacity = new_bcap;
                }
                current_chunk.body[current_chunk.b_count++] = strdup(subline);
            }
            free(subline);
            
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Chunk *tmp = realloc(catalog, new_cap * sizeof(Chunk));
                if (!tmp) return 1;
                catalog = tmp;
                capacity = new_cap;
            }
            catalog[count++] = current_chunk;
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Title: %s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].b_count; j++) {
                    printf("%s\n", catalog[i].body[j]);
                }
                if (i < count - 1) printf("--\n");
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].b_count; j++) free(catalog[i].body[j]);
        free(catalog[i].body);
    }
    free(catalog);
    return 0;
}
