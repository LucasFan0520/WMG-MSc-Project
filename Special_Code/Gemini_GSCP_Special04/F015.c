/* F015.c */
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
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
    Contact *contacts = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        if (*p == ' ') {
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
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *email = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *phone = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Contact *nc = realloc(contacts, cap * sizeof(Contact));
                    if (nc) contacts = nc;
                }
                char *nname = mystrdup(name);
                char *nemail = mystrdup(email);
                char *nphone = mystrdup(phone);
                char *nnote = mystrdup("");
                if (nname && nemail && nphone && nnote) {
                    contacts[count].name = nname;
                    contacts[count].email = nemail;
                    contacts[count].phone = nphone;
                    contacts[count].note = nnote;
                    count++;
                } else {
                    free(nname);
                    free(nemail);
                    free(nphone);
                    free(nnote);
                }
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nt = mystrdup(text);
                if (nt) {
                    free(contacts[found].note);
                    contacts[found].note = nt;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *remove = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int idxK = -1, idxR = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, keep) == 0) idxK = (int)i;
                if (strcmp(contacts[i].name, remove) == 0) idxR = (int)i;
            }
            if (idxK != -1 && idxR != -1) {
                size_t nlen = strlen(contacts[idxK].note) + strlen(contacts[idxR].note) + 2;
                char *nnote = malloc(nlen);
                if (nnote) {
                    if (strlen(contacts[idxK].note) > 0 && strlen(contacts[idxR].note) > 0) {
                        sprintf(nnote, "%s %s", contacts[idxK].note, contacts[idxR].note);
                    } else {
                        sprintf(nnote, "%s%s", contacts[idxK].note, contacts[idxR].note);
                    }
                    free(contacts[idxK].note);
                    contacts[idxK].note = nnote;
                }
                free(contacts[idxR].name);
                free(contacts[idxR].email);
                free(contacts[idxR].phone);
                free(contacts[idxR].note);
                for (size_t i = (size_t)idxR; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(contacts[found].name);
                free(contacts[found].email);
                free(contacts[found].phone);
                free(contacts[found].note);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s %s %s %s\n", contacts[found].name, contacts[found].email, contacts[found].phone, contacts[found].note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}
