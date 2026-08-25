// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteItem;

int main(int argc, char **argv) {
    if (argc < 2) return 1;

    NoteItem *items = NULL;
    size_t count = 0;
    size_t capacity = 0;

    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "LOAD") == 0) {
            FILE *f = fopen(argv[1], "r");
            if (f) {
                NoteItem *temp_items = NULL;
                size_t temp_count = 0;
                size_t temp_capacity = 0;
                int valid_load = 1;

                char *fline = NULL;
                size_t flen = 0;
                long fread_bytes;

                while ((fread_bytes = getline(&fline, &flen, f)) != -1) {
                    if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                        fline[fread_bytes - 1] = '\0';
                        fread_bytes--;
                    }
                    if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                        fline[fread_bytes - 1] = '\0';
                        fread_bytes--;
                    }

                    char *fp = fline;
                    while (*fp == ' ' || *fp == '	') fp++;
                    char *name = fp;
                    while (*fp && *fp != ' ' && *fp != '	') fp++;
                    if (*fp) {
                        *fp = '\0';
                        fp++;
                    }
                    while (*fp == ' ' || *fp == '	') fp++;
                    char *note = fp;

                    if (*name != '\0' && *note != '\0') {
                        if (temp_count >= temp_capacity) {
                            temp_capacity = temp_capacity == 0 ? 4 : temp_capacity * 2;
                            NoteItem *new_temp = realloc(temp_items, temp_capacity * sizeof(NoteItem));
                            if (!new_temp) exit(1);
                            temp_items = new_temp;
                        }
                        temp_items[temp_count].name = strdup(name);
                        temp_items[temp_count].note = strdup(note);
                        if (!temp_items[temp_count].name || !temp_items[temp_count].note) exit(1);
                        temp_count++;
                    } else {
                        valid_load = 0;
                        break;
                    }
                }
                free(fline);
                fclose(f);

                if (valid_load) {
                    for (size_t i = 0; i < count; i++) {
                        free(items[i].name);
                        free(items[i].note);
                    }
                    free(items);
                    items = temp_items;
                    count = temp_count;
                    capacity = temp_capacity;
                } else {
                    for (size_t i = 0; i < temp_count; i++) {
                        free(temp_items[i].name);
                        free(temp_items[i].note);
                    }
                    free(temp_items);
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                NoteItem *new_items = realloc(items, capacity * sizeof(NoteItem));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].name = strdup(name);
            items[count].note = strdup(note);
            if (!items[count].name || !items[count].note) exit(1);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    free(line);
    return 0;
}
