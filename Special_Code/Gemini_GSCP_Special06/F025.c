// F025.c
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
    char *student;
    char *note;
} Stud;

typedef struct {
    char *name;
    Stud *studs;
    int stud_count;
    int stud_cap;
} Class;

int main() {
    Class *arr = NULL;
    int count = 0;
    int cap = 0;
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
        if (strcmp(cmd, "CLASS") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Class *narr = realloc(arr, cap * sizeof(Class));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].name = mystrdup(name);
                    arr[count].studs = NULL;
                    arr[count].stud_count = 0;
                    arr[count].stud_cap = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *cls_name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, cls_name) == 0) {
                    if (arr[i].stud_count >= arr[i].stud_cap) {
                        arr[i].stud_cap = arr[i].stud_cap == 0 ? 4 : arr[i].stud_cap * 2;
                        Stud *ns = realloc(arr[i].studs, arr[i].stud_cap * sizeof(Stud));
                        if (ns) arr[i].studs = ns;
                    }
                    if (arr[i].stud_count < arr[i].stud_cap) {
                        arr[i].studs[arr[i].stud_count].student = mystrdup(student);
                        arr[i].studs[arr[i].stud_count].note = mystrdup(note);
                        arr[i].stud_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *fromClass = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *toClass = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int f_idx = -1, t_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, fromClass) == 0) f_idx = i;
                if (strcmp(arr[i].name, toClass) == 0) t_idx = i;
            }
            if (f_idx != -1 && t_idx != -1) {
                for (int j = 0; j < arr[f_idx].stud_count; j++) {
                    if (strcmp(arr[f_idx].studs[j].student, student) == 0) {
                        if (arr[t_idx].stud_count >= arr[t_idx].stud_cap) {
                            arr[t_idx].stud_cap = arr[t_idx].stud_cap == 0 ? 4 : arr[t_idx].stud_cap * 2;
                            Stud *ns = realloc(arr[t_idx].studs, arr[t_idx].stud_cap * sizeof(Stud));
                            if (ns) arr[t_idx].studs = ns;
                        }
                        if (arr[t_idx].stud_count < arr[t_idx].stud_cap) {
                            arr[t_idx].studs[arr[t_idx].stud_count] = arr[f_idx].studs[j];
                            arr[t_idx].stud_count++;
                            for (int k = j; k < arr[f_idx].stud_count - 1; k++) {
                                arr[f_idx].studs[k] = arr[f_idx].studs[k + 1];
                            }
                            arr[f_idx].stud_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *cls_name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, cls_name) == 0) {
                    for (int j = 0; j < arr[i].stud_count; j++) {
                        if (strcmp(arr[i].studs[j].student, student) == 0) {
                            free(arr[i].studs[j].student);
                            free(arr[i].studs[j].note);
                            for (int k = j; k < arr[i].stud_count - 1; k++) {
                                arr[i].studs[k] = arr[i].studs[k + 1];
                            }
                            arr[i].stud_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *cls_name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, cls_name) == 0) {
                    free(arr[i].name);
                    for (int j = 0; j < arr[i].stud_count; j++) {
                        free(arr[i].studs[j].student);
                        free(arr[i].studs[j].note);
                    }
                    free(arr[i].studs);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Class: %s\n", arr[i].name);
                for (int j = 0; j < arr[i].stud_count; j++) {
                    printf("  %s - %s\n", arr[i].studs[j].student, arr[i].studs[j].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].stud_count; j++) {
            free(arr[i].studs[j].student);
            free(arr[i].studs[j].note);
        }
        free(arr[i].studs);
    }
    free(arr);
    return 0;
}
