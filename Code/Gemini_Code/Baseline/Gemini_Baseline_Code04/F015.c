// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Contact *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *email = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *phone = p + 1;
                    arr = realloc(arr, (count + 1) * sizeof(Contact));
                    arr[count].name = strdup(name);
                    arr[count].email = strdup(email);
                    arr[count].phone = strdup(phone);
                    arr[count].note = strdup("");
                    count++;
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].note);
                        arr[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *remove = p + 1;
                int idxKeep = -1, idxRem = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, keep) == 0) idxKeep = i;
                    if (strcmp(arr[i].name, remove) == 0) idxRem = i;
                }
                if (idxKeep != -1 && idxRem != -1) {
                    size_t len1 = strlen(arr[idxKeep].note);
                    size_t len2 = strlen(arr[idxRem].note);
                    char *new_note = malloc(len1 + len2 + 2);
                    if (len1 > 0 && len2 > 0) {
                        strcpy(new_note, arr[idxKeep].note);
                        strcat(new_note, " ");
                        strcat(new_note, arr[idxRem].note);
                    } else if (len1 > 0) {
                        strcpy(new_note, arr[idxKeep].note);
                    } else {
                        strcpy(new_note, arr[idxRem].note);
                    }
                    free(arr[idxKeep].note);
                    arr[idxKeep].note = new_note;
                    free(arr[idxRem].name);
                    free(arr[idxRem].email);
                    free(arr[idxRem].phone);
                    free(arr[idxRem].note);
                    for (int j = idxRem; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].email);
                    free(arr[i].phone);
                    free(arr[i].note);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
