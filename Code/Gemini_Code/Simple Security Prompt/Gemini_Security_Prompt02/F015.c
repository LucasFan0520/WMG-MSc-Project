// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main() {
    Contact *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *n_start = line + 4;
            char *s1 = strchr(n_start, ' ');
            if (s1) {
                *s1 = '\0';
                char *e_start = s1 + 1;
                char *s2 = strchr(e_start, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *p_start = s2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(Contact));
                    }
                    arr[count].name = strdup(n_start);
                    arr[count].email = strdup(e_start);
                    arr[count].phone = strdup(p_start);
                    arr[count].note = strdup("");
                    count++;
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *n_start = line + 5;
            char *space = strchr(n_start, ' ');
            if (space) {
                *space = '\0';
                char *text = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, n_start) == 0) {
                        free(arr[i].note);
                        arr[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *keep_name = line + 6;
            char *space = strchr(keep_name, ' ');
            if (space) {
                *space = '\0';
                char *rem_name = space + 1;
                int idxK = -1, idxR = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, keep_name) == 0) idxK = (int)i;
                    if (strcmp(arr[i].name, rem_name) == 0) idxR = (int)i;
                }
                if (idxK != -1 && idxR != -1) {
                    size_t nlen = strlen(arr[idxK].note) + strlen(arr[idxR].note) + 2;
                    char *nnote = malloc(nlen);
                    strcpy(nnote, arr[idxK].note);
                    if (strlen(arr[idxK].note) > 0 && strlen(arr[idxR].note) > 0) {
                        strcat(nnote, " ");
                    }
                    strcat(nnote, arr[idxR].note);
                    free(arr[idxK].note);
                    arr[idxK].note = nnote;
                    free(arr[idxR].name);
                    free(arr[idxR].email);
                    free(arr[idxR].phone);
                    free(arr[idxR].note);
                    for (size_t j = idxR; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *n_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, n_start) == 0) {
                    free(arr[i].name);
                    free(arr[i].email);
                    free(arr[i].phone);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *n_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, n_start) == 0) {
                    printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
