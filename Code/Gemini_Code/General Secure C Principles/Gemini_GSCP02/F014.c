// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int lcount;
} TrackedItem;

int main(void) {
    TrackedItem *items = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char name[256];
            char note[1024];
            if (sscanf(line + 5, "%255s %[^\n]", name, note) == 2) {
                TrackedItem *tmp = realloc(items, (count + 1) * sizeof(TrackedItem));
                if (tmp) {
                    items = tmp;
                    items[count].name = strdup(name);
                    items[count].note = strdup(note);
                    items[count].labels = NULL;
                    items[count].lcount = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char name[256], lbl[256];
            if (sscanf(line + 6, "%255s %255s", name, lbl) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        char **tmp = realloc(items[i].labels, (items[i].lcount + 1) * sizeof(char *));
                        if (tmp) {
                            items[i].labels = tmp;
                            items[i].labels[items[i].lcount] = strdup(lbl);
                            items[i].lcount++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char name[256], lbl[256];
            if (sscanf(line + 8, "%255s %255s", name, lbl) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        for (int k = 0; k < items[i].lcount; k++) {
                            if (strcmp(items[i].labels[k], lbl) == 0) {
                                free(items[i].labels[k]);
                                for (int j = k; j < items[i].lcount - 1; j++) {
                                    items[i].labels[j] = items[i].labels[j + 1];
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
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (int k = 0; k < items[i].lcount; k++) {
                        free(items[i].labels[k]);
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
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s Labels:", items[i].name, items[i].note);
                    for (int k = 0; k < items[i].lcount; k++) {
                        printf(" %s", items[i].labels[k]);
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
        for (int k = 0; k < items[i].lcount; k++) {
            free(items[i].labels[k]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
