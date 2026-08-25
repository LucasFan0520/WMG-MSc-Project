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
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

int cmp_rem(const void *a, const void *b) {
    Reminder *aa = (Reminder *)a;
    Reminder *bb = (Reminder *)b;
    if (aa->priority > bb->priority) return -1;
    if (aa->priority < bb->priority) return 1;
    return (aa->id < bb->id) ? -1 : 1;
}

int main() {
    Reminder *arr = NULL;
    int count = 0;
    int cap = 0;
    int next_id = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *pri_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            int pri = atoi(pri_s);
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Reminder *narr = realloc(arr, cap * sizeof(Reminder));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].priority = pri;
                arr[count].title = mystrdup(title);
                arr[count].note = mystrdup(note);
                arr[count].id = next_id++;
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *delta_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int delta = atoi(delta_s);
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    arr[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
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
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *n_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int n = atoi(n_s);
            if (n > count) n = count;
            if (count > 0) {
                Reminder *cp = malloc(count * sizeof(Reminder));
                if (cp) {
                    memcpy(cp, arr, count * sizeof(Reminder));
                    qsort(cp, count, sizeof(Reminder), cmp_rem);
                    for (int i = 0; i < n; i++) {
                        printf("%d %s %s\n", cp[i].priority, cp[i].title, cp[i].note);
                    }
                    free(cp);
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
