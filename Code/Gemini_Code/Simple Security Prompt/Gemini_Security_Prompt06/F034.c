// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    Note *list = NULL;
    int count = 0;
    char *filepath = (argc > 1) ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LOAD") == 0 && filepath != NULL) {
                FILE *fp = fopen(filepath, "r");
                if (fp) {
                    Note *temp_list = NULL;
                    int temp_count = 0;
                    int valid = 1;
                    while (1) {
                        char *fline = read_line(fp);
                        if (!fline) break;
                        char *sp = strchr(fline, ' ');
                        if (!sp) {
                            valid = 0;
                            free(fline);
                            break;
                        }
                        *sp = '\0';
                        temp_list = realloc(temp_list, (temp_count + 1) * sizeof(Note));
                        temp_list[temp_count].name = strdup(fline);
                        temp_list[temp_count].note = strdup(sp + 1);
                        temp_count++;
                        free(fline);
                    }
                    fclose(fp);
                    if (valid) {
                        for (int i = 0; i < count; i++) {
                            free(list[i].name);
                            free(list[i].note);
                        }
                        free(list);
                        list = temp_list;
                        count = temp_count;
                    } else {
                        for (int i = 0; i < temp_count; i++) {
                            free(temp_list[i].name);
                            free(temp_list[i].note);
                        }
                        free(temp_list);
                    }
                }
            } else if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", list[i].name, list[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *note = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Note));
                list[count].name = strdup(name);
                list[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].note);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Note));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
    }
    free(list);
    return 0;
}