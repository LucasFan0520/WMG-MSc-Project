// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Rem;

int compare_rem(const void *a, const void *b) {
    Rem *ra = (Rem *)a;
    Rem *rb = (Rem *)b;
    if (rb->priority != ra->priority) {
        return rb->priority - ra->priority;
    }
    return ra->id - rb->id;
}

int main() {
    Rem *rems = NULL;
    int count = 0;
    int next_id = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int prio = atoi(args);
                char *title = p2 + 1;
                char *p3 = strchr(title, ' ');
                if (p3) {
                    *p3 = '\0';
                    rems = realloc(rems, sizeof(Rem) * (count + 1));
                    rems[count].priority = prio;
                    rems[count].title = strdup(title);
                    rems[count].note = strdup(p3 + 1);
                    rems[count].id = next_id++;
                    count++;
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int delta = atoi(p2 + 1);
                for (int i = 0; i < count; i++) {
                    if (strcmp(rems[i].title, args) == 0) {
                        rems[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(rems[i].title, args) == 0) {
                    free(rems[i].title);
                    free(rems[i].note);
                    for (int j = i; j < count - 1; j++) {
                        rems[j] = rems[j + 1];
                    }
                    count--;
                    if (count == 0) { free(rems); rems = NULL; }
                    else { rems = realloc(rems, sizeof(Rem) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = atoi(args);
            if (count > 0) {
                Rem *sorted = malloc(sizeof(Rem) * count);
                memcpy(sorted, rems, sizeof(Rem) * count);
                qsort(sorted, count, sizeof(Rem), compare_rem);
                int limit = (n < count) ? n : count;
                for (int i = 0; i < limit; i++) {
                    printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
                }
                free(sorted);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(rems[i].title);
        free(rems[i].note);
    }
    free(rems);
    return 0;
}
