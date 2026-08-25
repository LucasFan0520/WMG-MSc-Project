// F034.c
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
    char *name;
    char *note;
} NoteRec;

int main(int argc, char **argv) {
    NoteRec *arr = NULL;
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
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                FILE *f = fopen(argv[1], "r");
                if (f) {
                    NoteRec *temp_arr = NULL;
                    int temp_cnt = 0, temp_cap = 0;
                    char *fline;
                    int success = 1;
                    while ((fline = read_line(f)) != NULL) {
                        char *fp_line = fline;
                        while (*fp_line == ' ' || *fp_line == '\t') fp_line++;
                        if (*fp_line == '\0') { free(fline); continue; }
                        char *name = fp_line;
                        while (*fp_line && *fp_line != ' ' && *fp_line != '\t' && *fp_line != '\r' && *fp_line != '\n') fp_line++;
                        if (*fp_line) { *fp_line = '\0'; fp_line++; }
                        while (*fp_line == ' ' || *fp_line == '\t') fp_line++;
                        char *note = fp_line;
                        size_t nl = strlen(note);
                        while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                            note[nl-1] = '\0';
                            nl--;
                        }
                        if (strlen(name) == 0) { success = 0; }
                        if (success) {
                            if (temp_cnt >= temp_cap) {
                                temp_cap = temp_cap == 0 ? 8 : temp_cap * 2;
                                NoteRec *nt = realloc(temp_arr, temp_cap * sizeof(NoteRec));
                                if (nt) temp_arr = nt;
                            }
                            if (temp_cnt < temp_cap) {
                                temp_arr[temp_cnt].name = mystrdup(name);
                                temp_arr[temp_cnt].note = mystrdup(note);
                                temp_cnt++;
                            }
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
                        arr = temp_arr;
                        count = temp_cnt;
                        cap = temp_cap;
                    } else {
                        for (int i = 0; i < temp_cnt; i++) {
                            free(temp_arr[i].name);
                            free(temp_arr[i].note);
                        }
                        free(temp_arr);
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].note);
                    arr[i].note = mystrdup(note);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    NoteRec *narr = realloc(arr, cap * sizeof(NoteRec));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].name = mystrdup(name);
                    arr[count].note = mystrdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
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
