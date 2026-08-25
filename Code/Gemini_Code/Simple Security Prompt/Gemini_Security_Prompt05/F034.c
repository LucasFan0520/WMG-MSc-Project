// F034.c
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
    char *name;
    char *note;
} Note;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Note *arr = NULL;
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
        if (strcmp(cmd, "LOAD") == 0) {
            FILE *f = fopen(argv[1], "r");
            if (f) {
                Note *t_arr = NULL;
                int t_count = 0;
                int t_cap = 0;
                int success = 1;
                while (1) {
                    char *fline = read_line(f);
                    if (!fline) break;
                    char *fp = fline;
                    char *name = get_token(&fp);
                    char *note = get_rest(&fp);
                    if (name && strlen(name) > 0) {
                        if (t_count >= t_cap) {
                            t_cap = t_cap == 0 ? 4 : t_cap * 2;
                            Note *nt_arr = realloc(t_arr, t_cap * sizeof(Note));
                            if (nt_arr) t_arr = nt_arr;
                        }
                        t_arr[t_count].name = strdup(name);
                        t_arr[t_count].note = strdup(note);
                        t_count++;
                    }
                    free(fline);
                }
                fclose(f);
                if (success) {
                    for (int i = 0; i < count; i++) {
                        free(arr[i].name);
                        free(arr[i].note);
                    }
                    free(arr);
                    arr = t_arr;
                    count = t_count;
                    cap = t_cap;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = get_token(&p);
            char *note = get_rest(&p);
            if (name) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Note *narr = realloc(arr, cap * sizeof(Note));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].note);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
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
    }
    free(arr);
    return 0;
}
