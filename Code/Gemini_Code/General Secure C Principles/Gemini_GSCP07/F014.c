// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} TrackerItem;

typedef struct {
    char *item_name;
    char *label;
} LabelRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    TrackerItem *ilist = NULL;
    size_t icount = 0;
    size_t icapacity = 0;
    LabelRec *llist = NULL;
    size_t lcount = 0;
    size_t lcapacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (icount >= icapacity) {
                icapacity = icapacity == 0 ? 4 : icapacity * 2;
                TrackerItem *nl = realloc(ilist, icapacity * sizeof(TrackerItem));
                if (!nl) break;
                ilist = nl;
            }
            ilist[icount].name = strdup(name);
            ilist[icount].note = strdup(note);
            icount++;
        } else if (strcmp(cmd, "LABEL") == 0) {
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
            if (lcount >= lcapacity) {
                lcapacity = lcapacity == 0 ? 4 : lcapacity * 2;
                LabelRec *nl = realloc(llist, lcapacity * sizeof(LabelRec));
                if (!nl) break;
                llist = nl;
            }
            llist[lcount].item_name = strdup(name);
            llist[lcount].label = strdup(label);
            lcount++;
        } else if (strcmp(cmd, "UNLABEL") == 0) {
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
            for (size_t i = 0; i < lcount; i++) {
                if (strcmp(llist[i].item_name, name) == 0 && strcmp(llist[i].label, label) == 0) {
                    free(llist[i].item_name);
                    free(llist[i].label);
                    for (size_t j = i; j < lcount - 1; j++) {
                        llist[j] = llist[j + 1];
                    }
                    lcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < icount; i++) {
                if (strcmp(ilist[i].name, name) == 0) {
                    free(ilist[i].name);
                    free(ilist[i].note);
                    for (size_t j = i; j < icount - 1; j++) {
                        ilist[j] = ilist[j + 1];
                    }
                    icount--;
                    break;
                }
            }
            size_t k = 0;
            while (k < lcount) {
                if (strcmp(llist[k].item_name, name) == 0) {
                    free(llist[k].item_name);
                    free(llist[k].label);
                    for (size_t j = k; j < lcount - 1; j++) {
                        llist[j] = llist[j + 1];
                    }
                    lcount--;
                } else {
                    k++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < icount; i++) {
                if (strcmp(ilist[i].name, name) == 0) {
                    printf("%s:", ilist[i].note);
                    for (size_t j = 0; j < lcount; j++) {
                        if (strcmp(llist[j].item_name, name) == 0) {
                            printf(" %s", llist[j].label);
                        }
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < icount; i++) {
                printf("%s %s\n", ilist[i].name, ilist[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < icount; i++) {
        free(ilist[i].name);
        free(ilist[i].note);
    }
    free(ilist);
    for (size_t i = 0; i < lcount; i++) {
        free(llist[i].item_name);
        free(llist[i].label);
    }
    free(llist);
    return 0;
}
