// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
} Item;

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

int main(void) {
    Item *list = NULL;
    int count = 0;
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
        if (strcmp(cmd, "ITEM") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            list = realloc(list, (count + 1) * sizeof(Item));
            list[count].name = strdup(name);
            list[count].note = strdup(note);
            list[count].labels = NULL;
            list[count].label_count = 0;
            count++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    int lc = list[i].label_count;
                    list[i].labels = realloc(list[i].labels, (lc + 1) * sizeof(char *));
                    list[i].labels[lc] = strdup(label);
                    list[i].label_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    for (int j = 0; j < list[i].label_count; j++) {
                        if (strcmp(list[i].labels[j], label) == 0) {
                            free(list[i].labels[j]);
                            for (int k = j; k < list[i].label_count - 1; k++) {
                                list[i].labels[k] = list[i].labels[k + 1];
                            }
                            list[i].label_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    for (int j = 0; j < list[i].label_count; j++) {
                        free(list[i].labels[j]);
                    }
                    free(list[i].labels);
                    list[i].name = NULL;
                    list[i].note = NULL;
                    list[i].labels = NULL;
                    list[i].label_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    printf("%s: %s [", list[i].name, list[i].note);
                    for (int j = 0; j < list[i].label_count; j++) {
                        printf("%s%s", list[i].labels[j], (j == list[i].label_count - 1) ? "" : ", ");
                    }
                    printf("]\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].name) {
                    printf("%s\n", list[i].name);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].name) {
            free(list[i].name);
            free(list[i].note);
            for (int j = 0; j < list[i].label_count; j++) {
                free(list[i].labels[j]);
            }
            free(list[i].labels);
        }
    }
    free(list);
    return 0;
}
