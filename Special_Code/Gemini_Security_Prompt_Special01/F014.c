// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t label_count;
    size_t label_capacity;
} Item;

int main(void) {
    Item *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *note = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Item *new_items = realloc(items, capacity * sizeof(Item));
                if (new_items) items = new_items;
            }
            items[count].name = mystrdup(name);
            items[count].note = mystrdup(note);
            items[count].labels = NULL;
            items[count].label_count = 0;
            items[count].label_capacity = 0;
            count++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *lbl = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                lbl++;
            }
            while (*lbl == ' ') lbl++;
            char *lbl_end = lbl;
            while (*lbl_end && *lbl_end != ' ') lbl_end++;
            *lbl_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (items[i].label_count >= items[i].label_capacity) {
                        items[i].label_capacity = items[i].label_capacity == 0 ? 2 : items[i].label_capacity * 2;
                        char **new_lbls = realloc(items[i].labels, items[i].label_capacity * sizeof(char *));
                        if (new_lbls) items[i].labels = new_lbls;
                    }
                    items[i].labels[items[i].label_count++] = mystrdup(lbl);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *lbl = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                lbl++;
            }
            while (*lbl == ' ') lbl++;
            char *lbl_end = lbl;
            while (*lbl_end && *lbl_end != ' ') lbl_end++;
            *lbl_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    for (size_t j = 0; j < items[i].label_count; j++) {
                        if (strcmp(items[i].labels[j], lbl) == 0) {
                            free(items[i].labels[j]);
                            for (size_t k = j; k < items[i].label_count - 1; k++) {
                                items[i].labels[k] = items[i].labels[k + 1];
                            }
                            items[i].label_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = 0; j < items[i].label_count; j++) {
                        free(items[i].labels[j]);
                    }
                    free(items[i].labels);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (size_t j = 0; j < items[i].label_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (size_t j = 0; j < items[i].label_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
