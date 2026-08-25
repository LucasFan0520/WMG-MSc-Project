// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line_from(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} ContactCSV;

int main(int argc, char **argv) {
    ContactCSV *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line_from(fp);
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
                        size_t phlen = strlen(phone);
                        while (phlen > 0 && (phone[phlen-1] == '\n' || phone[phlen-1] == '\n')) {
                            phone[phlen-1] = '\0';
                            phlen--;
                        }
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            ContactCSV *nitems = realloc(items, cap * sizeof(ContactCSV));
                            if (nitems) items = nitems;
                        }
                        items[count].name = mystrdup(name);
                        items[count].email = mystrdup(email);
                        items[count].phone = mystrdup(phone);
                        count++;
                    }
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line_from(stdin);
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
            char *args = p;
            size_t alen = strlen(args);
            while (alen > 0 && (args[alen-1] == '\n' || args[alen-1] == '\n')) {
                args[alen-1] = '\0';
                alen--;
            }
            char *c1 = strchr(args, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = args;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        ContactCSV *nitems = realloc(items, cap * sizeof(ContactCSV));
                        if (nitems) items = nitems;
                    }
                    items[count].name = mystrdup(name);
                    items[count].email = mystrdup(email);
                    items[count].phone = mystrdup(phone);
                    count++;
                }
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
                    printf("%s,%s,%s\n", items[i].name, items[i].email, items[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", items[i].name, items[i].email, items[i].phone);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].email);
        free(items[i].phone);
    }
    free(items);
    return 0;
}
