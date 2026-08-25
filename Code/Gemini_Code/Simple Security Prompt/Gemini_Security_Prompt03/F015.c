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

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

int main(void) {
    Contact *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *email = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *phone = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            list = realloc(list, (count + 1) * sizeof(Contact));
            list[count].name = strdup(name);
            list[count].email = strdup(email);
            list[count].phone = strdup(phone);
            list[count].note = strdup("");
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].note);
                    list[i].note = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *remove = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idxK = -1, idxR = -1;
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, keep) == 0) idxK = i;
                if (list[i].name && strcmp(list[i].name, remove) == 0) idxR = i;
            }
            if (idxK != -1 && idxR != -1) {
                size_t len1 = strlen(list[idxK].note);
                size_t len2 = strlen(list[idxR].note);
                char *new_note = malloc(len1 + len2 + 2);
                strcpy(new_note, list[idxK].note);
                if (len1 > 0 && len2 > 0) strcat(new_note, " ");
                strcat(new_note, list[idxR].note);
                free(list[idxK].note);
                list[idxK].note = new_note;
                free(list[idxR].name);
                free(list[idxR].email);
                free(list[idxR].phone);
                free(list[idxR].note);
                list[idxR].name = NULL;
                list[idxR].email = NULL;
                list[idxR].phone = NULL;
                list[idxR].note = NULL;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].email);
                    free(list[i].phone);
                    free(list[i].note);
                    list[i].name = NULL;
                    list[i].email = NULL;
                    list[i].phone = NULL;
                    list[i].note = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    printf("%s <%s> Phone: %s Note: %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].name) {
                    printf("%s\n", list[i].name);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].name) {
            free(list[i].name);
            free(list[i].email);
            free(list[i].phone);
            free(list[i].note);
        }
    }
    free(list);
    return 0;
}
