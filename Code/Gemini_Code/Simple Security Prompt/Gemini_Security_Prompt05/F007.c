// F007.c
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
    char *time;
    char *label;
    char *note;
} Activity;

int compare_activity(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    int cmp = strcmp(actA->time, actB->time);
    if (cmp != 0) return cmp;
    return strcmp(actA->label, actB->label);
}

int main() {
    Activity *arr = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            char *time = get_token(&p);
            char *label = get_token(&p);
            char *note = get_rest(&p);
            if (time && label) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Activity *narr = realloc(arr, cap * sizeof(Activity));
                    if (narr) arr = narr;
                }
                arr[count].time = strdup(time);
                arr[count].label = strdup(label);
                arr[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = get_token(&p);
            char *label = get_token(&p);
            if (time && label) {
                for (int i = 0; i < count; ) {
                    if (strcmp(arr[i].time, time) == 0 && strcmp(arr[i].label, label) == 0) {
                        free(arr[i].time);
                        free(arr[i].label);
                        free(arr[i].note);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = get_token(&p);
            if (label) {
                int best_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].label, label) == 0) {
                        if (best_idx == -1 || strcmp(arr[i].time, arr[best_idx].time) < 0) {
                            best_idx = i;
                        }
                    }
                }
                if (best_idx != -1) {
                    printf("%s %s %s\n", arr[best_idx].time, arr[best_idx].label, arr[best_idx].note);
                }
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(Activity), compare_activity);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", arr[i].time, arr[i].label, arr[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(arr[i].time);
                free(arr[i].label);
                free(arr[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].time);
        free(arr[i].label);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
