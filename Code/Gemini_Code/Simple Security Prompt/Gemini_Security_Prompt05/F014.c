// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    int lcount;
    int lcap;
} Item;

int main() {
    Item *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = get_token(&p);
            char *note = get_rest(&p);
            if (name) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Item *narr = realloc(arr, cap * sizeof(Item));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].note = strdup(note);
                arr[count].labels = NULL;
                arr[count].lcount = 0;
                arr[count].lcap = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = get_token(&p);
            char *label = get_token(&p);
            if (name && label) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        Item *it = &arr[i];
                        if (it->lcount >= it->lcap) {
                            it->lcap = it->lcap == 0 ? 4 : it->lcap * 2;
                            char **nlabels = realloc(it->labels, it->lcap * sizeof(char *));
                            if (nlabels) it->labels = nlabels;
                        }
                        it->labels[it->lcount] = strdup(label);
                        it->lcount++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = get_token(&p);
            char *label = get_token(&p);
            if (name && label) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        for (int j = 0; j < arr[i].lcount; j++) {
                            if (strcmp(arr[i].labels[j], label) == 0) {
                                free(arr[i].labels[j]);
                                for (int k = j; k < arr[i].lcount - 1; k++) {
                                    arr[i].labels[k] = arr[i].labels[k + 1];
                                }
                                arr[i].lcount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].note);
                        for (int j = 0; j < arr[i].lcount; j++) {
                            free(arr[i].labels[j]);
                        }
                        free(arr[i].labels);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        printf("%s %s", arr[i].name, arr[i].note);
                        for (int j = 0; j < arr[i].lcount; j++) {
                            printf(" %s", arr[i].labels[j]);
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
        for (int j = 0; j < arr[i].lcount; j++) {
            free(arr[i].labels[j]);
        }
        free(arr[i].labels);
    }
    free(arr);
    return 0;
}
