// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *label;
} Label;

typedef struct {
    char *name;
    char *note;
    Label *labels;
    int label_count;
} Item;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Item *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *p = line + 5;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(Item));
                arr[count].name = strdup(name);
                arr[count].note = strdup(note);
                arr[count].labels = NULL;
                arr[count].label_count = 0;
                count++;
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p = line + 6;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *lbl = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        int lc = arr[i].label_count;
                        arr[i].labels = realloc(arr[i].labels, (lc + 1) * sizeof(Label));
                        arr[i].labels[lc].label = strdup(lbl);
                        arr[i].label_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p = line + 8;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *lbl = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        for (int j = 0; j < arr[i].label_count; j++) {
                            if (strcmp(arr[i].labels[j].label, lbl) == 0) {
                                free(arr[i].labels[j].label);
                                for (int k = j; k < arr[i].label_count - 1; k++) {
                                    arr[i].labels[k] = arr[i].labels[k + 1];
                                }
                                arr[i].label_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (int j = 0; j < arr[i].label_count; j++) {
                        free(arr[i].labels[j].label);
                    }
                    free(arr[i].labels);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s", arr[i].name, arr[i].note);
                    for (int j = 0; j < arr[i].label_count; j++) {
                        printf(" %s", arr[i].labels[j].label);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
        for (int j = 0; j < arr[i].label_count; j++) {
            free(arr[i].labels[j].label);
        }
        free(arr[i].labels);
    }
    free(arr);
    return 0;
}
