/* F014.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int lbl_count;
    int lbl_cap;
} TrackerItem;

int main(void) {
    TrackerItem *items = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            char *name = NULL;
            char *note = NULL;
            if (space) {
                *space = '\0';
                name = strdup(args);
                note = strdup(space + 1);
                *space = ' ';
            } else {
                name = strdup(args);
                note = strdup("");
            }
            if (name && note) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    TrackerItem *tmp = realloc(items, new_cap * sizeof(TrackerItem));
                    if (tmp) {
                        items = tmp;
                        capacity = new_cap;
                    } else {
                        free(name);
                        free(note);
                        continue;
                    }
                }
                items[count].name = name;
                items[count].note = note;
                items[count].labels = NULL;
                items[count].lbl_count = 0;
                items[count].lbl_cap = 0;
                count++;
            } else {
                free(name);
                free(note);
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *lbl = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        TrackerItem *ti = &items[i];
                        char *n_lbl = strdup(lbl);
                        if (n_lbl) {
                            if (ti->lbl_count >= ti->lbl_cap) {
                                int new_lcap = ti->lbl_cap == 0 ? 4 : ti->lbl_cap * 2;
                                char **tmp = realloc(ti->labels, new_lcap * sizeof(char *));
                                if (tmp) {
                                    ti->labels = tmp;
                                    ti->lbl_cap = new_lcap;
                                } else {
                                    free(n_lbl);
                                    break;
                                }
                            }
                            ti->labels[ti->lbl_count] = n_lbl;
                            ti->lbl_count++;
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *args = line + 8;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *lbl = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        TrackerItem *ti = &items[i];
                        for (int j = 0; j < ti->lbl_count; j++) {
                            if (strcmp(ti->labels[j], lbl) == 0) {
                                free(ti->labels[j]);
                                for (int m = j; m < ti->lbl_count - 1; m++) {
                                    ti->labels[m] = ti->labels[m + 1];
                                }
                                ti->lbl_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (int j = 0; j < items[i].lbl_count; j++) {
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (int j = 0; j < items[i].lbl_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
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
        for (int j = 0; j < items[i].lbl_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
