// F027.c
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

int main() {
    Reminder *items = NULL;
    int count = 0;
    int cap = 0;
    int next_id = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", items[i].priority, items[i].title, items[i].note);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *pri_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Reminder *nb = realloc(items, cap * sizeof(Reminder));
                if (nb) items = nb;
            }
            items[count].priority = atoi(pri_str);
            items[count].title = strdup(title);
            items[count].note = strdup(note);
            items[count].id = next_id++;
            count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *delta_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int delta = atoi(delta_str);
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    items[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    free(items[i].note);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int n = atoi(n_str);
            if (n > count) n = count;
            Reminder *copy = malloc(count * sizeof(Reminder));
            if (copy) {
                for (int i = 0; i < count; i++) copy[i] = items[i];
                for (int i = 0; i < count; i++) {
                    for (int j = i + 1; j < count; j++) {
                        int swap = 0;
                        if (copy[i].priority < copy[j].priority) {
                            swap = 1;
                        } else if (copy[i].priority == copy[j].priority) {
                            if (copy[i].id > copy[j].id) swap = 1;
                        }
                        if (swap) {
                            Reminder tmp = copy[i];
                            copy[i] = copy[j];
                            copy[j] = tmp;
                        }
                    }
                }
                for (int i = 0; i < n; i++) {
                    printf("%d %s %s\n", copy[i].priority, copy[i].title, copy[i].note);
                }
                free(copy);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].note);
    }
    free(items);
    return 0;
}
