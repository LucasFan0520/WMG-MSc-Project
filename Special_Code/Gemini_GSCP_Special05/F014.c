// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t lbl_count;
    size_t lbl_cap;
} Item;

int main(void) {
    Item *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *p = line + 5;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Item *nb = realloc(items, cap * sizeof(Item));
                if (!nb) {
                    free(line);
                    break;
                }
                items = nb;
            }
            items[count].name = safe_dup(name);
            items[count].note = safe_dup(note);
            items[count].labels = NULL;
            items[count].lbl_count = 0;
            items[count].lbl_cap = 0;
            count++;
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p = line + 6;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *label = "";
            if (*p == ' ') {
                *p = '\0';
                label = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Item *it = &items[i];
                    if (it->lbl_count >= it->lbl_cap) {
                        it->lbl_cap = it->lbl_cap == 0 ? 4 : it->lbl_cap * 2;
                        char **nb = realloc(it->labels, it->lbl_cap * sizeof(char *));
                        if (!nb) break;
                        it->labels = nb;
                    }
                    it->labels[it->lbl_count] = safe_dup(label);
                    it->lbl_count++;
                    break;
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p = line + 8;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *label = "";
            if (*p == ' ') {
                *p = '\0';
                label = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Item *it = &items[i];
                    for (size_t j = 0; j < it->lbl_count; j++) {
                        if (strcmp(it->labels[j], label) == 0) {
                            free(it->labels[j]);
                            for (size_t k = j; k < it->lbl_count - 1; k++) {
                                it->labels[k] = it->labels[k + 1];
                            }
                            it->lbl_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s:", items[i].name, items[i].note);
                    for (size_t j = 0; j < items[i].lbl_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
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
