// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    char *disc_title = NULL;
    char *disc_body = NULL;
    int disc_avail = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].title, arr[i].body);
                }
            } else if (strcmp(cmd, "RESTORE") == 0) {
                if (disc_avail) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Draft *narr = realloc(arr, ncap * sizeof(Draft));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].title = strdup(disc_title);
                        arr[count].body = strdup(disc_body);
                        count++;
                        free(disc_title);
                        free(disc_body);
                        disc_title = NULL;
                        disc_body = NULL;
                        disc_avail = 0;
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                char *body = p2 + 1;
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Draft *narr = realloc(arr, ncap * sizeof(Draft));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].title = strdup(title);
                        arr[count].body = strdup(body);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                char *body = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        char *nbody = strdup(body);
                        if (nbody) {
                            free(arr[i].body);
                            arr[i].body = nbody;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    if (disc_avail) {
                        free(disc_title);
                        free(disc_body);
                    }
                    disc_title = strdup(arr[i].title);
                    disc_body = strdup(arr[i].body);
                    disc_avail = 1;
                    free(arr[i].title);
                    free(arr[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("%s\n", arr[i].body);
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].body);
    }
    free(arr);
    if (disc_avail) {
        free(disc_title);
        free(disc_body);
    }
    return 0;
}
