// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
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

typedef struct {
    int priority;
    char *title;
    char *note;
    size_t ins_order;
} Reminder;

int compare_rem(const void *a, const void *b) {
    Reminder *ra = (Reminder *)a;
    Reminder *rb = (Reminder *)b;
    if (rb->priority != ra->priority) {
        return rb->priority - ra->priority;
    }
    return (ra->ins_order > rb->ins_order) - (ra->ins_order < rb->ins_order);
}

int main(void) {
    Reminder *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t order_counter = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *pri_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Reminder *nb = realloc(list, cap * sizeof(Reminder));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].priority = atoi(pri_str);
                list[count].title = strdup(title);
                list[count].note = strdup(note);
                list[count].ins_order = order_counter++;
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int delta = atoi(p);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    list[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    free(list[i].title);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int n = atoi(arg1);
            Reminder *temp = malloc(count * sizeof(Reminder));
            if (temp) {
                memcpy(temp, list, count * sizeof(Reminder));
                qsort(temp, count, sizeof(Reminder), compare_rem);
                int limit = n < (int)count ? n : (int)count;
                for (int i = 0; i < limit; i++) {
                    printf("(%d) %s: %s\n", temp[i].priority, temp[i].title, temp[i].note);
                }
                free(temp);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("(%d) %s: %s\n", list[i].priority, list[i].title, list[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].note);
    }
    free(list);
    return 0;
}
