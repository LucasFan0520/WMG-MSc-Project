// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t label_count;
    size_t label_cap;
} Item;

int main(void) {
    Item *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Item *nb = realloc(list, cap * sizeof(Item));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].name = strdup(name);
                list[count].note = strdup(note);
                list[count].labels = NULL;
                list[count].label_count = 0;
                list[count].label_cap = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0 || strcmp(cmd, "UNLABEL") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    if (strcmp(cmd, "LABEL") == 0) {
                        if (list[i].label_count >= list[i].label_cap) {
                            list[i].label_cap = list[i].label_cap == 0 ? 4 : list[i].label_cap * 2;
                            char **nl = realloc(list[i].labels, list[i].label_cap * sizeof(char *));
                            if (nl) list[i].labels = nl;
                        }
                        if (list[i].label_count < list[i].label_cap) {
                            list[i].labels[list[i].label_count++] = strdup(label);
                        }
                    } else {
                        for (size_t j = 0; j < list[i].label_count; j++) {
                            if (strcmp(list[i].labels[j], label) == 0) {
                                free(list[i].labels[j]);
                                for (size_t k = j; k < list[i].label_count - 1; k++) {
                                    list[i].labels[k] = list[i].labels[k + 1];
                                }
                                list[i].label_count--;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    for (size_t j = 0; j < list[i].label_count; j++) {
                        free(list[i].labels[j]);
                    }
                    free(list[i].labels);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    printf("%s %s", list[i].name, list[i].note);
                    for (size_t j = 0; j < list[i].label_count; j++) {
                        printf(" %s", list[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s", list[i].name, list[i].note);
                for (size_t j = 0; j < list[i].label_count; j++) {
                    printf(" %s", list[i].labels[j]);
                }
                printf("\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
        for (size_t j = 0; j < list[i].label_count; j++) {
            free(list[i].labels[j]);
        }
        free(list[i].labels);
    }
    free(list);
    return 0;
}
