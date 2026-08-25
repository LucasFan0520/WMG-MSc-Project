/* F016.c */
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
} Contact;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Contact *contacts = NULL;
    size_t count = 0;
    size_t cap = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            char *c1 = strchr(line, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = line;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Contact *nc = realloc(contacts, cap * sizeof(Contact));
                        if (nc) contacts = nc;
                    }
                    char *nname = mystrdup(name);
                    char *nemail = mystrdup(email);
                    char *nphone = mystrdup(phone);
                    if (nname && nemail && nphone) {
                        contacts[count].name = nname;
                        contacts[count].email = nemail;
                        contacts[count].phone = nphone;
                        count++;
                    } else {
                        free(nname);
                        free(nemail);
                        free(nphone);
                    }
                }
            }
            free(line);
        }
        fclose(fp);
    }
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
            char *c1 = strchr(p, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = p;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;
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
                        if (nname && nemail && nphone) {
                            contacts[count].name = nname;
                            contacts[count].email = nemail;
                            contacts[count].phone = nphone;
                            count++;
                        } else {
                            free(nname);
                            free(nemail);
                            free(nphone);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
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
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s,%s,%s\n", contacts[found].name, contacts[found].email, contacts[found].phone);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
