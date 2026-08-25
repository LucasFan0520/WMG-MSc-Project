// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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
    size_t lbl_count;
    size_t lbl_cap;
} Item;

int main(void) {
    Item *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].name, arr[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *note = p2 + 1;
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Item *narr = realloc(arr, ncap * sizeof(Item));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].name = strdup(name);
                        arr[count].note = strdup(note);
                        arr[count].labels = NULL;
                        arr[count].lbl_count = 0;
                        arr[count].lbl_cap = 0;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *lbl = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        Item *item = &arr[i];
                        if (item->lbl_count >= item->lbl_cap) {
                            size_t ncap = item->lbl_cap == 0 ? 4 : item->lbl_cap * 2;
                            char **nlabels = realloc(item->labels, ncap * sizeof(char *));
                            if (nlabels) {
                                item->labels = nlabels;
                                item->lbl_cap = ncap;
                            }
                        }
                        if (item->lbl_count < item->lbl_cap) {
                            item->labels[item->lbl_count] = strdup(lbl);
                            item->lbl_count++;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *lbl = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        Item *item = &arr[i];
                        for (size_t j = 0; j < item->lbl_count; j++) {
                            if (strcmp(item->labels[j], lbl) == 0) {
                                free(item->labels[j]);
                                for (size_t k = j; k < item->lbl_count - 1; k++) {
                                    item->labels[k] = item->labels[k + 1];
                                }
                                item->lbl_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = 0; j < arr[i].lbl_count; j++) {
                        free(arr[i].labels[j]);
                    }
                    free(arr[i].labels);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s", arr[i].name, arr[i].note);
                    for (size_t j = 0; j < arr[i].lbl_count; j++) {
                        printf(" %s", arr[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
        for (size_t j = 0; j < arr[i].lbl_count; j++) {
            free(arr[i].labels[j]);
        }
        free(arr[i].labels);
    }
    free(arr);
    return 0;
}
