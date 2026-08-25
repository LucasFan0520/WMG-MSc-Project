// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} TrackerItem;

typedef struct {
    char *name;
    char *label;
} ItemLabel;

int main(void) {
    TrackerItem *items = NULL;
    size_t icount = 0;
    size_t icap = 0;
    ItemLabel *labels = NULL;
    size_t lcount = 0;
    size_t lcap = 0;
    while (1) {
        char *line = read_line();
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
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            int found = -1;
            for (size_t i = 0; i < icount; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(items[found].note);
                items[found].note = mystrdup(note);
            } else {
                if (icount >= icap) {
                    icap = icap == 0 ? 8 : icap * 2;
                    TrackerItem *nitems = realloc(items, icap * sizeof(TrackerItem));
                    if (nitems) items = nitems;
                }
                items[icount].name = mystrdup(name);
                items[icount].note = mystrdup(note);
                icount++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *label = p;
            size_t lblen = strlen(label);
            while (lblen > 0 && (label[lblen-1] == '\n' || label[lblen-1] == '\n')) {
                label[lblen-1] = '\0';
                lblen--;
            }
            if (lcount >= lcap) {
                lcap = lcap == 0 ? 8 : lcap * 2;
                ItemLabel *nlabels = realloc(labels, lcap * sizeof(ItemLabel));
                if (nlabels) labels = nlabels;
            }
            labels[lcount].name = mystrdup(name);
            labels[lcount].label = mystrdup(label);
            lcount++;
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *label = p;
            size_t lblen = strlen(label);
            while (lblen > 0 && (label[lblen-1] == '\n' || label[lblen-1] == '\n')) {
                label[lblen-1] = '\0';
                lblen--;
            }
            for (size_t i = 0; i < lcount; i++) {
                if (strcmp(labels[i].name, name) == 0 && strcmp(labels[i].label, label) == 0) {
                    free(labels[i].name);
                    free(labels[i].label);
                    for (size_t j = i; j < lcount - 1; j++) {
                        labels[j] = labels[j+1];
                    }
                    lcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < icount) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < icount - 1; j++) {
                        items[j] = items[j+1];
                    }
                    icount--;
                } else {
                    i++;
                }
            }
            size_t li = 0;
            while (li < lcount) {
                if (strcmp(labels[li].name, name) == 0) {
                    free(labels[li].name);
                    free(labels[li].label);
                    for (size_t j = li; j < lcount - 1; j++) {
                        labels[j] = labels[j+1];
                    }
                    lcount--;
                } else {
                    li++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            for (size_t i = 0; i < icount; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s Labels:", items[i].name, items[i].note);
                    for (size_t j = 0; j < lcount; j++) {
                        if (strcmp(labels[j].name, name) == 0) {
                            printf(" %s", labels[j].label);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < icount; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < icount; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    for (size_t i = 0; i < lcount; i++) {
        free(labels[i].name);
        free(labels[i].label);
    }
    free(labels);
    return 0;
}
