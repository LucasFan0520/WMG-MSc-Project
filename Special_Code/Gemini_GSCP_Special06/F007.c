// F007.c
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
    long long time;
    char *label;
    char *note;
} Activity;

int cmp_activity(const void *a, const void *b) {
    Activity *aa = (Activity *)a;
    Activity *bb = (Activity *)b;
    if (aa->time < bb->time) return -1;
    if (aa->time > bb->time) return 1;
    return strcmp(aa->label, bb->label);
}

int main() {
    Activity *arr = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *tm_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\n' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            long long tm = strtoll(tm_s, NULL, 10);
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Activity *narr = realloc(arr, cap * sizeof(Activity));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].time = tm;
                arr[count].label = mystrdup(label);
                arr[count].note = mystrdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *tm_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            long long tm = strtoll(tm_s, NULL, 10);
            for (int i = 0; i < count; ) {
                if (arr[i].time == tm && strcmp(arr[i].label, label) == 0) {
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
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int best_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].label, label) == 0) {
                    if (best_idx == -1 || arr[i].time < arr[best_idx].time) {
                        best_idx = i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%lld %s %s\n", arr[best_idx].time, arr[best_idx].label, arr[best_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(Activity), cmp_activity);
            }
            for (int i = 0; i < count; i++) {
                printf("%lld %s %s\n", arr[i].time, arr[i].label, arr[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(arr[i].label);
                free(arr[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].label);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
