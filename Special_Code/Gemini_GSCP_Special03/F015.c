// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    if (!s) return NULL;
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *email = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *phone = p;
            size_t phlen = strlen(phone);
            while (phlen > 0 && (phone[phlen-1] == '\n' || phone[phlen-1] == '\n')) {
                phone[phlen-1] = '\0';
                phlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Contact *nitems = realloc(items, cap * sizeof(Contact));
                if (nitems) items = nitems;
            }
            items[count].name = mystrdup(name);
            items[count].email = mystrdup(email);
            items[count].phone = mystrdup(phone);
            items[count].note = mystrdup("");
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *text = p;
            size_t tlen = strlen(text);
            while (tlen > 0 && (text[tlen-1] == '\n' || text[tlen-1] == '\n')) {
                text[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].note);
                    items[i].note = mystrdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *remove = p;
            size_t rlen = strlen(remove);
            while (rlen > 0 && (remove[rlen-1] == '\n' || remove[rlen-1] == '\n')) {
                remove[rlen-1] = '\0';
                rlen--;
            }
            int kidx = -1;
            int ridx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, keep) == 0) kidx = (int)i;
                if (strcmp(items[i].name, remove) == 0) ridx = (int)i;
            }
            if (kidx != -1 && ridx != -1) {
                size_t l1 = items[kidx].note ? strlen(items[kidx].note) : 0;
                size_t l2 = items[ridx].note ? strlen(items[ridx].note) : 0;
                char *nn = malloc(l1 + l2 + 2);
                if (nn) {
                    if (l1 > 0) {
                        strcpy(nn, items[kidx].note);
                        if (l2 > 0) {
                            strcat(nn, " ");
                            strcat(nn, items[ridx].note);
                        }
                    } else {
                        if (l2 > 0) {
                            strcpy(nn, items[ridx].note);
                        } else {
                            strcpy(nn, "");
                        }
                    }
                    free(items[kidx].note);
                    items[kidx].note = nn;
                }
                free(items[ridx].name);
                free(items[ridx].email);
                free(items[ridx].phone);
                free(items[ridx].note);
                for (size_t j = (size_t)ridx; j < count - 1; j++) {
                    items[j] = items[j+1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].email);
                    free(items[i].phone);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
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
