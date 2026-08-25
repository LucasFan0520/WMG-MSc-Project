// F015.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *email = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                email = p;
            }
            while (*p && *p != ' ') p++;
            char *phone = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                phone = p;
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Contact *tmp = realloc(arr, capacity * sizeof(Contact));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                arr[count].email = strdup(email);
                arr[count].phone = strdup(phone);
                arr[count].note = strdup("");
                if (!arr[count].name || !arr[count].email || !arr[count].phone || !arr[count].note) exit(1);
                count++;
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *note_text = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note_text = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].note);
                    arr[i].note = strdup(note_text);
                    if (!arr[i].note) exit(1);
                    break;
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *keep = p;
            while (*p && *p != ' ') p++;
            char *remove = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                remove = p;
            }
            int idxK = -1, idxR = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, keep) == 0) idxK = (int)i;
                if (strcmp(arr[i].name, remove) == 0) idxR = (int)i;
            }
            if (idxK != -1 && idxR != -1) {
                size_t nlen = strlen(arr[idxK].note) + strlen(arr[idxR].note) + 2;
                char *nnote = malloc(nlen);
                if (!nnote) exit(1);
                snprintf(nnote, nlen, "%s %s", arr[idxK].note, arr[idxR].note);
                free(arr[idxK].note);
                arr[idxK].note = nnote;
                free(arr[idxR].name);
                free(arr[idxR].email);
                free(arr[idxR].phone);
                free(arr[idxR].note);
                for (size_t j = (size_t)idxR; j < count - 1; j++) {
                    arr[j] = arr[j + 1];
                }
                count--;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
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
            char *name = line + 5;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
        free(arr[i].note);
    }
    free(arr);
    free(line);
    return 0;
}
