// F025.c
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
} Student;

typedef struct {
    char *name;
    Student *students;
    int scount;
    int scap;
} Class;

int main() {
    Class *arr = NULL;
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
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = get_token(&p);
            if (name) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Class *narr = realloc(arr, cap * sizeof(Class));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].students = NULL;
                arr[count].scount = 0;
                arr[count].scap = 0;
                count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cname = get_token(&p);
            char *sname = get_token(&p);
            char *note = get_rest(&p);
            if (cname && sname) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, cname) == 0) {
                        Class *c = &arr[i];
                        if (c->scount >= c->scap) {
                            c->scap = c->scap == 0 ? 4 : c->scap * 2;
                            Student *nstuds = realloc(c->students, c->scap * sizeof(Student));
                            if (nstuds) c->students = nstuds;
                        }
                        c->students[c->scount].name = strdup(sname);
                        c->students[c->scount].note = strdup(note);
                        c->scount++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromC = get_token(&p);
            char *toC = get_token(&p);
            char *sname = get_token(&p);
            if (fromC && toC && sname) {
                int fidx = -1, tidx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, fromC) == 0) fidx = i;
                    if (strcmp(arr[i].name, toC) == 0) tidx = i;
                }
                if (fidx != -1 && tidx != -1) {
                    for (int j = 0; j < arr[fidx].scount; j++) {
                        if (strcmp(arr[fidx].students[j].name, sname) == 0) {
                            Class *dest = &arr[tidx];
                            if (dest->scount >= dest->scap) {
                                dest->scap = dest->scap == 0 ? 4 : dest->scap * 2;
                                Student *nstuds = realloc(dest->students, dest->scap * sizeof(Student));
                                if (nstuds) dest->students = nstuds;
                            }
                            dest->students[dest->scount] = arr[fidx].students[j];
                            dest->scount++;
                            for (int k = j; k < arr[fidx].scount - 1; k++) {
                                arr[fidx].students[k] = arr[fidx].students[k + 1];
                            }
                            arr[fidx].scount--;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cname = get_token(&p);
            char *sname = get_token(&p);
            if (cname && sname) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, cname) == 0) {
                        for (int j = 0; j < arr[i].scount; j++) {
                            if (strcmp(arr[i].students[j].name, sname) == 0) {
                                free(arr[i].students[j].name);
                                free(arr[i].students[j].note);
                                for (int k = j; k < arr[i].scount - 1; k++) {
                                    arr[i].students[k] = arr[i].students[k + 1];
                                }
                                arr[i].scount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        for (int j = 0; j < arr[i].scount; j++) {
                            free(arr[i].students[j].name);
                            free(arr[i].students[j].note);
                        }
                        free(arr[i].students);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
                for (int j = 0; j < arr[i].scount; j++) {
                    printf("%s %s\n", arr[i].students[j].name, arr[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].scount; j++) {
            free(arr[i].students[j].name);
            free(arr[i].students[j].note);
        }
        free(arr[i].students);
    }
    free(arr);
    return 0;
}
