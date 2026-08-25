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

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Contact *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note ? list[i].note : "");
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *email = rest2;
                    char *phone = p3 + 1;
                    list = realloc(list, (count + 1) * sizeof(Contact));
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    list[count].note = NULL;
                    count++;
                }
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *text = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        free(list[i].note);
                        list[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *keep = rest;
                char *remove = p2 + 1;
                int idx_k = -1, idx_r = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, keep) == 0) idx_k = i;
                    if (strcmp(list[i].name, remove) == 0) idx_r = i;
                }
                if (idx_k != -1 && idx_r != -1) {
                    char *k_note = list[idx_k].note ? list[idx_k].note : "";
                    char *r_note = list[idx_r].note ? list[idx_r].note : "";
                    size_t len = strlen(k_note) + strlen(r_note) + 2;
                    char *new_note = malloc(len);
                    strcpy(new_note, k_note);
                    if (list[idx_k].note && list[idx_r].note) {
                        strcat(new_note, " ");
                    }
                    strcat(new_note, r_note);
                    free(list[idx_k].note);
                    list[idx_k].note = new_note;
                    free(list[idx_r].name);
                    free(list[idx_r].email);
                    free(list[idx_r].phone);
                    free(list[idx_r].note);
                    for (int i = idx_r; i < count - 1; i++) {
                        list[i] = list[i + 1];
                    }
                    count--;
                    if (count == 0) {
                        free(list);
                        list = NULL;
                    } else {
                        list = realloc(list, count * sizeof(Contact));
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].email);
                free(list[found].phone);
                free(list[found].note);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Contact));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note ? list[i].note : "");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
        free(list[i].note);
    }
    free(list);
    return 0;
}