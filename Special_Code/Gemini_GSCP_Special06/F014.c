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
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
} Item;

int main() {
    Item *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '	' || *p == '\n' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\n' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Item *narr = realloc(arr, cap * sizeof(Item));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].name = mystrdup(name);
                arr[count].note = mystrdup(note);
                arr[count].labels = NULL;
                arr[count].label_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    arr[i].labels = realloc(arr[i].labels, (arr[i].label_count + 1) * sizeof(char *));
                    if (arr[i].labels) {
                        arr[i].labels[arr[i].label_count] = mystrdup(label);
                        arr[i].label_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    for (int j = 0; j < arr[i].label_count; j++) {
                        if (strcmp(arr[i].labels[j], label) == 0) {
                            free(arr[i].labels[j]);
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
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (int j = 0; j < arr[i].label_count; j++) {
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
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s Labels:", arr[i].name, arr[i].note);
                    for (int j = 0; j < arr[i].label_count; j++) {
                        printf(" %s", arr[i].labels[j]);
                    }
                    printf("\n");
                    break;
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
        for (int j = 0; j < arr[i].label_count; j++) {
            free(arr[i].labels[j]);
        }
        free(arr[i].labels);
    }
    free(arr);
    return 0;
}
