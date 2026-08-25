// F031.c
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
} ImportRec;

int main() {
    ImportRec *arr = NULL;
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
        if (strcmp(cmd, "BATCH") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *n_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int n = atoi(n_s);
            int valid_batch = 1;
            ImportRec *temp_arr = NULL;
            int temp_count = 0;
            if (n > 0) {
                temp_arr = malloc(n * sizeof(ImportRec));
                for (int i = 0; i < n; i++) {
                    char *bline = read_line(stdin);
                    if (!bline) {
                        valid_batch = 0;
                        continue;
                    }
                    char *bp = bline;
                    while (*bp == ' ' || *bp == '\t') bp++;
                    if (*bp == '\0') {
                        valid_batch = 0;
                        free(bline);
                        continue;
                    }
                    char *name = bp;
                    while (*bp && *bp != ' ' && *bp != '\t' && *bp != '\r' && *bp != '\n') bp++;
                    if (*bp) { *bp = '\0'; bp++; }
                    while (*bp == ' ' || *bp == '\t') bp++;
                    char *note = bp;
                    size_t nl = strlen(note);
                    while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                        note[nl-1] = '\0';
                        nl--;
                    }
                    if (strlen(name) == 0) {
                        valid_batch = 0;
                    }
                    if (valid_batch) {
                        temp_arr[temp_count].name = mystrdup(name);
                        temp_arr[temp_count].note = mystrdup(note);
                        temp_count++;
                    }
                    free(bline);
                }
            }
            if (valid_batch && temp_count == n) {
                for (int i = 0; i < temp_count; i++) {
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        ImportRec *narr = realloc(arr, cap * sizeof(ImportRec));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count] = temp_arr[i];
                        count++;
                    } else {
                        free(temp_arr[i].name);
                        free(temp_arr[i].note);
                    }
                }
            } else {
                for (int i = 0; i < temp_count; i++) {
                    free(temp_arr[i].name);
                    free(temp_arr[i].note);
                }
            }
            free(temp_arr);
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; ) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
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
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s\n", arr[i].note);
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
