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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    int lcount;
    int lcap;
} ItemTracker;

int main(void) {
    ItemTracker *items = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    items = realloc(items, cap * sizeof(ItemTracker));
                }
                items[count].name = strdup(name);
                items[count].note = strdup(note);
                items[count].labels = NULL;
                items[count].lcount = 0;
                items[count].lcap = 0;
                count++;
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *lbl = p + 1;
                while (*lbl == ' ') lbl++;
                char *lbl_end = lbl;
                while (*lbl_end && *lbl_end != ' ') lbl_end++;
                if (*lbl_end == ' ') *lbl_end = '\0';
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        ItemTracker *it = &items[i];
                        if (it->lcount >= it->lcap) {
                            it->lcap = it->lcap == 0 ? 16 : it->lcap * 2;
                            it->labels = realloc(it->labels, it->lcap * sizeof(char *));
                        }
                        it->labels[it->lcount] = strdup(lbl);
                        it->lcount++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *lbl = p + 1;
                while (*lbl == ' ') lbl++;
                char *lbl_end = lbl;
                while (*lbl_end && *lbl_end != ' ') lbl_end++;
                if (*lbl_end == ' ') *lbl_end = '\0';
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        for (int j = 0; j < items[i].lcount; j++) {
                            if (strcmp(items[i].labels[j], lbl) == 0) {
                                free(items[i].labels[j]);
                                for (int k = j; k < items[i].lcount - 1; k++) {
                                    items[i].labels[k] = items[i].labels[k + 1];
                                }
                                items[i].lcount--;
                                break;
                            }
                        }
                        break;
                    }
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
                for (int j = 0; j < items[found].lcount; j++) free(items[found].labels[j]);
                free(items[found].labels);
                for (int i = found; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s\n", items[i].name, items[i].note);
                    for (int j = 0; j < items[i].lcount; j++) {
                        printf("Label: %s\n", items[i].labels[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].lcount; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}
