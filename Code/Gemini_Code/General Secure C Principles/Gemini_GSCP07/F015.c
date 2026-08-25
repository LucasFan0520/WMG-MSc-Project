// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} ContactRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    ContactRec *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *email = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *phone = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                ContactRec *nl = realloc(list, capacity * sizeof(ContactRec));
                if (!nl) break;
                list = nl;
            }
            list[count].name = strdup(name);
            list[count].email = strdup(email);
            list[count].phone = strdup(phone);
            list[count].note = NULL;
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    char *nn = strdup(text);
                    if (nn) {
                        free(list[i].note);
                        list[i].note = nn;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *remove = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int kidx = -1, ridx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, keep) == 0) kidx = (int)i;
                if (strcmp(list[i].name, remove) == 0) ridx = (int)i;
            }
            if (kidx != -1 && ridx != -1) {
                size_t len1 = list[kidx].note ? strlen(list[kidx].note) : 0;
                size_t len2 = list[ridx].note ? strlen(list[ridx].note) : 0;
                char *nn = malloc(len1 + len2 + 2);
                if (nn) {
                    nn[0] = '\0';
                    if (list[kidx].note) {
                        strcpy(nn, list[kidx].note);
                    }
                    if (len1 > 0 && len2 > 0) {
                        strcat(nn, " ");
                    }
                    if (list[ridx].note) {
                        strcat(nn, list[ridx].note);
                    }
                    free(list[kidx].note);
                    list[kidx].note = nn;
                }
                free(list[ridx].name);
                free(list[ridx].email);
                free(list[ridx].phone);
                free(list[ridx].note);
                for (size_t j = (size_t)ridx; j < count - 1; j++) {
                    list[j] = list[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].email);
                    free(list[i].phone);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note ? list[i].note : "");
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note ? list[i].note : "");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
        free(list[i].note);
    }
    free(list);
    return 0;
}
