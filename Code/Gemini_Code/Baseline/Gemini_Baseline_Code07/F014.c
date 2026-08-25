// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
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
    int lbl_count;
    int lbl_capacity;
} Item;

int main() {
    Item *items = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
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
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", name, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    items = realloc(items, capacity * sizeof(Item));
                }
                items[count].name = strdup(name);
                items[count].note = strdup(p);
                items[count].labels = NULL;
                items[count].lbl_count = 0;
                items[count].lbl_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], lbl[256];
            if (sscanf(p, "%255s %255s", name, lbl) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        if (items[i].lbl_count >= items[i].lbl_capacity) {
                            items[i].lbl_capacity = items[i].lbl_capacity == 0 ? 4 : items[i].lbl_capacity * 2;
                            items[i].labels = realloc(items[i].labels, items[i].lbl_capacity * sizeof(char *));
                        }
                        items[i].labels[items[i].lbl_count++] = strdup(lbl);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], lbl[256];
            if (sscanf(p, "%255s %255s", name, lbl) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        int found_lbl = -1;
                        for (int j = 0; j < items[i].lbl_count; j++) {
                            if (strcmp(items[i].labels[j], lbl) == 0) {
                                found_lbl = j;
                                break;
                            }
                        }
                        if (found_lbl != -1) {
                            free(items[i].labels[found_lbl]);
                            for (int j = found_lbl; j < items[i].lbl_count - 1; j++) {
                                items[i].labels[j] = items[i].labels[j + 1];
                            }
                            items[i].lbl_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(items[found].name);
                    free(items[found].note);
                    for (int j = 0; j < items[found].lbl_count; j++) {
                        free(items[found].labels[j]);
                    }
                    free(items[found].labels);
                    for (int i = found; i < count - 1; i++) {
                        items[i] = items[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        printf("%s %s:", items[i].name, items[i].note);
                        for (int j = 0; j < items[i].lbl_count; j++) {
                            printf(" %s", items[i].labels[j]);
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].lbl_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
