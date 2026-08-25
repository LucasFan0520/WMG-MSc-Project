// F027.c
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
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    if (rB->priority != rA->priority) {
        return rB->priority - rA->priority;
    }
    return rA->id - rB->id;
}

int main() {
    Reminder *arr = NULL;
    int count = 0;
    int cap = 0;
    int id_counter = 0;
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
            char *pri_s = get_token(&p);
            char *title = get_token(&p);
            char *note = get_rest(&p);
            if (pri_s && title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Reminder *narr = realloc(arr, cap * sizeof(Reminder));
                    if (narr) arr = narr;
                }
                arr[count].priority = atoi(pri_s);
                arr[count].title = strdup(title);
                arr[count].note = strdup(note);
                arr[count].id = id_counter++;
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = get_token(&p);
            char *delta_s = get_token(&p);
            if (title && delta_s) {
                int delta = atoi(delta_s);
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        arr[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        free(arr[i].title);
                        free(arr[i].note);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_s = get_token(&p);
            if (n_s) {
                int n = atoi(n_s);
                if (n > count) n = count;
                if (count > 0) {
                    Reminder *temp = malloc(count * sizeof(Reminder));
                    if (temp) {
                        memcpy(temp, arr, count * sizeof(Reminder));
                        qsort(temp, count * sizeof(Reminder), sizeof(Reminder), compare_reminders);
                        for (int i = 0; i < n; i++) {
                            printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
                        }
                        free(temp);
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", arr[i].priority, arr[i].title, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
