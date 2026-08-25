// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t lbl_count;
    size_t lbl_cap;
} TrackerItem;

int main() {
    TrackerItem *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *label = p;
        while (*p && *p != ' ') p++;
        if (*p) *p = '\0';
        if (strcmp(cmd, "ITEM") == 0) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(TrackerItem));
            }
            items[count].name = mystrdup(name);
            items[count].note = mystrdup(label); 
            items[count].lbl_count = 0;
            items[count].lbl_cap = 0;
            items[count].labels = NULL;
            count++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (items[i].lbl_count >= items[i].lbl_cap) {
                        items[i].lbl_cap = items[i].lbl_cap == 0 ? 4 : items[i].lbl_cap * 2;
                        items[i].labels = realloc(items[i].labels, items[i].lbl_cap * sizeof(char *));
                    }
                    items[i].labels[items[i].lbl_count++] = mystrdup(label);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    for (size_t j = 0; j < items[i].lbl_count; j++) {
                        if (strcmp(items[i].labels[j], label) == 0) {
                            free(items[i].labels[j]);
                            for (size_t k = j; k < items[i].lbl_count - 1; k++) {
                                items[i].labels[k] = items[i].labels[k + 1];
                            }
                            items[i].lbl_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = 0; j < items[i].lbl_count; j++) {
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s Labels:", items[i].name, items[i].note);
                    for (size_t j = 0; j < items[i].lbl_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s Labels:", items[i].name, items[i].note);
                for (size_t j = 0; j < items[i].lbl_count; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                printf("\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (size_t j = 0; j < items[i].lbl_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
