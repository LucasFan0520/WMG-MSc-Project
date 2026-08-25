// F014.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int l_count;
    int l_capacity;
} Item;

int main(void) {
    Item *items = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
                while (*note == ' ') note++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Item *tmp = realloc(items, new_cap * sizeof(Item));
                if (!tmp) return 1;
                items = tmp;
                capacity = new_cap;
            }
            char *nn = strdup(name);
            char *nt = strdup(note);
            if (!nn || !nt) return 1;
            items[count].name = nn;
            items[count].note = nt;
            items[count].labels = NULL;
            items[count].l_count = 0;
            items[count].l_capacity = 0;
            count++;
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *lbl = space + 1;
            while (*lbl == ' ') lbl++;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                Item *it = &items[found];
                if (it->l_count >= it->l_capacity) {
                    int new_lcap = it->l_capacity == 0 ? 4 : it->l_capacity * 2;
                    char **tmp = realloc(it->labels, new_lcap * sizeof(char *));
                    if (!tmp) return 1;
                    it->labels = tmp;
                    it->l_capacity = new_lcap;
                }
                char *nl = strdup(lbl);
                if (!nl) return 1;
                it->labels[it->l_count] = nl;
                it->l_count++;
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *lbl = space + 1;
            while (*lbl == ' ') lbl++;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                Item *it = &items[found];
                int lf = -1;
                for (int j = 0; j < it->l_count; j++) {
                    if (strcmp(it->labels[j], lbl) == 0) {
                        lf = j;
                        break;
                    }
                }
                if (lf != -1) {
                    free(it->labels[lf]);
                    for (int j = lf; j < it->l_count - 1; j++) {
                        it->labels[j] = it->labels[j + 1];
                    }
                    it->l_count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
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
                for (int j = 0; j < items[found].l_count; j++) free(items[found].labels[j]);
                free(items[found].labels);
                for (int i = found; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s %s Labels:", items[found].name, items[found].note);
                for (int j = 0; j < items[found].l_count; j++) {
                    printf(" %s", items[found].labels[j]);
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].l_count; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}
