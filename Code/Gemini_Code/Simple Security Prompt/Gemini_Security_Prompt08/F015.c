// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
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

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = arg1;
            p = arg1;
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
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Contact *nb = realloc(list, cap * sizeof(Contact));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].name = strdup(name);
                list[count].email = strdup(email);
                list[count].phone = strdup(phone);
                list[count].note = strdup("");
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].note);
                    list[i].note = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *remove = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t k_idx = (size_t)-1;
            size_t r_idx = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, keep) == 0) k_idx = i;
                if (strcmp(list[i].name, remove) == 0) r_idx = i;
            }
            if (k_idx != (size_t)-1 && r_idx != (size_t)-1) {
                size_t nlen = strlen(list[k_idx].note) + strlen(list[r_idx].note) + 2;
                char *nn = malloc(nlen);
                if (nn) {
                    if (strlen(list[k_idx].note) > 0 && strlen(list[r_idx].note) > 0) {
                        sprintf(nn, "%s %s", list[k_idx].note, list[r_idx].note);
                    } else if (strlen(list[k_idx].note) > 0) {
                        strcpy(nn, list[k_idx].note);
                    } else {
                        strcpy(nn, list[r_idx].note);
                    }
                    free(list[k_idx].note);
                    list[k_idx].note = nn;
                }
                free(list[r_idx].name);
                free(list[r_idx].email);
                free(list[r_idx].phone);
                free(list[r_idx].note);
                for (size_t j = r_idx; j < count - 1; j++) {
                    list[j] = list[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
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
