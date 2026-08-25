/* F014.c */
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
        len--;
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
    char **labels;
    size_t l_count;
    size_t l_cap;
} Item;

int main(void) {
    Item *items = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        if (*p == ' ') {
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
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Item *ni = realloc(items, cap * sizeof(Item));
                    if (ni) items = ni;
                }
                char *nname = mystrdup(name);
                char *nnote = mystrdup(note);
                if (nname && nnote) {
                    items[count].name = nname;
                    items[count].note = nnote;
                    items[count].labels = NULL;
                    items[count].l_count = 0;
                    items[count].l_cap = 0;
                    count++;
                } else {
                    free(nname);
                    free(nnote);
                }
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Item *it = &items[found];
                if (it->l_count >= it->l_cap) {
                    it->l_cap = it->l_cap == 0 ? 4 : it->l_cap * 2;
                    char **nl = realloc(it->labels, it->l_cap * sizeof(char *));
                    if (nl) it->labels = nl;
                }
                char *nlab = mystrdup(label);
                if (nlab) {
                    it->labels[it->l_count] = nlab;
                    it->l_count++;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Item *it = &items[found];
                int l_found = -1;
                for (size_t j = 0; j < it->l_count; j++) {
                    if (strcmp(it->labels[j], label) == 0) {
                        l_found = (int)j;
                        break;
                    }
                }
                if (l_found != -1) {
                    free(it->labels[l_found]);
                    for (size_t j = (size_t)l_found; j < it->l_count - 1; j++) {
                        it->labels[j] = it->labels[j + 1];
                    }
                    it->l_count--;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(items[found].name);
                free(items[found].note);
                for (size_t j = 0; j < items[found].l_count; j++) {
                    free(items[found].labels[j]);
                }
                free(items[found].labels);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s %s", items[found].name, items[found].note);
                for (size_t j = 0; j < items[found].l_count; j++) {
                    printf(" %s", items[found].labels[j]);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (size_t j = 0; j < items[i].l_count; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                putchar('\n');
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (size_t j = 0; j < items[i].l_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
