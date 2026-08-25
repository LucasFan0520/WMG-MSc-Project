// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main() {
    Contact *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *name = get_token(&p);
            char *email = get_token(&p);
            char *phone = get_token(&p);
            if (name && email && phone) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Contact *narr = realloc(arr, cap * sizeof(Contact));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].email = strdup(email);
                arr[count].phone = strdup(phone);
                arr[count].note = strdup("");
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = get_token(&p);
            char *text = get_rest(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].note);
                        arr[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = get_token(&p);
            char *remove = get_token(&p);
            if (keep && remove) {
                int kidx = -1, ridx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, keep) == 0) kidx = i;
                    if (strcmp(arr[i].name, remove) == 0) ridx = i;
                }
                if (kidx != -1 && ridx != -1) {
                    int nlen = strlen(arr[kidx].note) + 1 + strlen(arr[ridx].note) + 1;
                    char *nn = malloc(nlen);
                    if (nn) {
                        strcpy(nn, arr[kidx].note);
                        if (strlen(arr[kidx].note) > 0 && strlen(arr[ridx].note) > 0) {
                            strcat(nn, " ");
                        }
                        strcat(nn, arr[ridx].note);
                        free(arr[kidx].note);
                        arr[kidx].note = nn;
                    }
                    free(arr[ridx].name);
                    free(arr[ridx].email);
                    free(arr[ridx].phone);
                    free(arr[ridx].note);
                    for (int j = ridx; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&p);
            if (name) {
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
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
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
