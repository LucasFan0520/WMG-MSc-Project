// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} FileNote;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    FileNote *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                FileNote *new_list = NULL;
                size_t new_count = 0;
                char *f_line = NULL;
                size_t f_len = 0;
                int valid = 1;
                while (getline(&f_line, &f_len, fp) != -1) {
                    size_t fl = strlen(f_line);
                    if (fl > 0 && f_line[fl - 1] == '\n') f_line[fl - 1] = '\0';
                    if (fl > 1 && f_line[fl - 2] == '\r') f_line[fl - 2] = '\0';
                    char *fp_ptr = f_line;
                    while (*fp_ptr == ' ' || *fp_ptr == '\t') fp_ptr++;
                    char *fname = fp_ptr;
                    while (*fp_ptr && *fp_ptr != ' ' && *fp_ptr != '\t') fp_ptr++;
                    if (*fp_ptr) {
                        *fp_ptr = '\0';
                        fp_ptr++;
                    }
                    while (*fp_ptr == ' ' || *fp_ptr == '\t') fp_ptr++;
                    char *fnote = fp_ptr;
                    if (strlen(fname) > 0 && strlen(fnote) > 0) {
                        FileNote *temp = realloc(new_list, (new_count + 1) * sizeof(FileNote));
                        if (temp) {
                            new_list = temp;
                            new_list[new_count].name = strdup(fname);
                            new_list[new_count].note = strdup(fnote);
                            new_count++;
                        }
                    }
                }
                free(f_line);
                fclose(fp);
                if (valid) {
                    for (size_t i = 0; i < count; i++) {
                        free(list[i].name);
                        free(list[i].note);
                    }
                    free(list);
                    list = new_list;
                    count = new_count;
                } else {
                    for (size_t i = 0; i < new_count; i++) {
                        free(new_list[i].name);
                        free(new_list[i].note);
                    }
                    free(new_list);
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].note);
                    list[i].note = strdup(note);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                FileNote *temp = realloc(list, (count + 1) * sizeof(FileNote));
                if (temp) {
                    list = temp;
                    list[count].name = strdup(name);
                    list[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].name, list[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
    }
    free(list);
    free(line);
    return 0;
}
