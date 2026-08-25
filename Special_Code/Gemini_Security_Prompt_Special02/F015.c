// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main() {
    Contact *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
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
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(Contact));
            }
            items[count].name = mystrdup(name);
            items[count].email = mystrdup(email);
            items[count].phone = mystrdup(phone);
            items[count].note = mystrdup("");
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].note);
                    items[i].note = mystrdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *remove_name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Contact *keep = NULL;
            Contact *remove = NULL;
            size_t rem_idx = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) keep = &items[i];
                if (strcmp(items[i].name, remove_name) == 0) {
                    remove = &items[i];
                    rem_idx = i;
                }
            }
            if (keep && remove) {
                size_t l1 = strlen(keep->note);
                size_t l2 = strlen(remove->note);
                char *nn = malloc(l1 + l2 + 2);
                strcpy(nn, keep->note);
                if (l1 > 0 && l2 > 0) strcat(nn, " ");
                strcat(nn, remove->note);
                free(keep->note);
                keep->note = nn;
                free(remove->name);
                free(remove->email);
                free(remove->phone);
                free(remove->note);
                for (size_t j = rem_idx; j < count - 1; j++) {
                    items[j] = items[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].email);
                    free(items[i].phone);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s %s %s\n", items[i].name, items[i].email, items[i].phone, items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", items[i].name, items[i].email, items[i].phone, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].email);
        free(items[i].phone);
        free(items[i].note);
    }
    free(items);
    return 0;
}
