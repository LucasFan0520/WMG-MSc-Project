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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
    int label_cap;
} TrackerItem;

int main() {
    TrackerItem *items = NULL;
    int count = 0;
    int cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                TrackerItem *nb = realloc(items, cap * sizeof(TrackerItem));
                if (nb) items = nb;
            }
            items[count].name = strdup(name);
            items[count].note = strdup(note);
            items[count].labels = NULL;
            items[count].label_count = 0;
            items[count].label_cap = 0;
            count++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    TrackerItem *ti = &items[i];
                    if (ti->label_count >= ti->label_cap) {
                        ti->label_cap = ti->label_cap == 0 ? 16 : ti->label_cap * 2;
                        char **nb = realloc(ti->labels, ti->label_cap * sizeof(char *));
                        if (nb) ti->labels = nb;
                    }
                    ti->labels[ti->label_count] = strdup(label);
                    ti->label_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    for (int j = 0; j < items[i].label_count; j++) {
                        if (strcmp(items[i].labels[j], label) == 0) {
                            free(items[i].labels[j]);
                            for (int k = j; k < items[i].label_count - 1; k++) {
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
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (int j = 0; j < items[i].label_count; j++) {
                        free(items[i].labels[j]);
                    }
                    free(items[i].labels);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (int j = 0; j < items[i].label_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].label_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
