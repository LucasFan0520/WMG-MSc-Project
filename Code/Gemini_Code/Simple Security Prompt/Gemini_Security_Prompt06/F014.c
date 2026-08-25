// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int l_count;
} Item;

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

int main() {
    Item *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
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
        if (strcmp(cmd, "ITEM") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *note = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Item));
                list[count].name = strdup(name);
                list[count].note = strdup(note);
                list[count].labels = NULL;
                list[count].l_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0 || strcmp(cmd, "UNLABEL") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *label = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    if (strcmp(cmd, "LABEL") == 0) {
                        int lc = list[found].l_count;
                        list[found].labels = realloc(list[found].labels, (lc + 1) * sizeof(char *));
                        list[found].labels[lc] = strdup(label);
                        list[found].l_count++;
                    } else {
                        int l_idx = -1;
                        for (int i = 0; i < list[found].l_count; i++) {
                            if (strcmp(list[found].labels[i], label) == 0) {
                                l_idx = i;
                                break;
                            }
                        }
                        if (l_idx != -1) {
                            free(list[found].labels[l_idx]);
                            for (int i = l_idx; i < list[found].l_count - 1; i++) {
                                list[found].labels[i] = list[found].labels[i + 1];
                            }
                            list[found].l_count--;
                            if (list[found].l_count == 0) {
                                free(list[found].labels);
                                list[found].labels = NULL;
                            } else {
                                list[found].labels = realloc(list[found].labels, list[found].l_count * sizeof(char *));
                            }
                        }
                    }
                }
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
                for (int i = 0; i < list[found].l_count; i++) {
                    free(list[found].labels[i]);
                }
                free(list[found].labels);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Item));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s %s labels:", list[found].name, list[found].note);
                for (int i = 0; i < list[found].l_count; i++) {
                    printf(" %s", list[found].labels[i]);
                }
                printf("\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
        for (int j = 0; j < list[i].l_count; j++) {
            free(list[i].labels[j]);
        }
        free(list[i].labels);
    }
    free(list);
    return 0;
}