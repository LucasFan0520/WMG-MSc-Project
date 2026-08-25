// F015.c
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
    if (c == EOF && len == 0) {
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
    Contact *items = NULL;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", items[i].name, items[i].email, items[i].phone, items[i].note ? items[i].note : "");
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
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
            if (*p) {
                *p = '\0';
            }
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Contact *nb = realloc(items, cap * sizeof(Contact));
                if (nb) items = nb;
            }
            items[count].name = strdup(name);
            items[count].email = strdup(email);
            items[count].phone = strdup(phone);
            items[count].note = NULL;
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].note);
                    items[i].note = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *remove = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int k_idx = -1, r_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, keep) == 0) k_idx = (int)i;
                if (strcmp(items[i].name, remove) == 0) r_idx = (int)i;
            }
            if (k_idx != -1 && r_idx != -1) {
                char *k_note = items[k_idx].note;
                char *r_note = items[r_idx].note;
                char *n_note = NULL;
                if (k_note && r_note) {
                    size_t nl = strlen(k_note) + 1 + strlen(r_note) + 1;
                    n_note = malloc(nl);
                    if (n_note) {
                        strcpy(n_note, k_note);
                        strcat(n_note, " ");
                        strcat(n_note, r_note);
                    }
                } else if (k_note) {
                    n_note = strdup(k_note);
                } else if (r_note) {
                    n_note = strdup(r_note);
                }
                free(items[k_idx].note);
                items[k_idx].note = n_note;
                free(items[r_idx].name);
                free(items[r_idx].email);
                free(items[r_idx].phone);
                free(items[r_idx].note);
                for (size_t j = r_idx; j < count - 1; j++) {
                    items[j] = items[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
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
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s %s %s\n", items[i].name, items[i].email, items[i].phone, items[i].note ? items[i].note : "");
                    break;
                }
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
