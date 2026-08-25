// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
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
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *email = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                email++;
            }
            while (*email == ' ') email++;
            char *email_end = email;
            while (*email_end && *email_end != ' ') email_end++;
            char *phone = email_end;
            if (*email_end != '\0') {
                *email_end = '\0';
                phone++;
            }
            while (*phone == ' ') phone++;
            char *phone_end = phone;
            while (*phone_end && *phone_end != ' ') phone_end++;
            *phone_end = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Contact *new_list = realloc(list, capacity * sizeof(Contact));
                if (new_list) list = new_list;
            }
            list[count].name = mystrdup(name);
            list[count].email = mystrdup(email);
            list[count].phone = mystrdup(phone);
            list[count].note = mystrdup("");
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *text = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                text++;
            }
            while (*text == ' ') text++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].note);
                    list[i].note = mystrdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = p;
            while (*keep == ' ') keep++;
            char *keep_end = keep;
            while (*keep_end && *keep_end != ' ') keep_end++;
            char *remove = keep_end;
            if (*keep_end != '\0') {
                *keep_end = '\0';
                remove++;
            }
            while (*remove == ' ') remove++;
            char *remove_end = remove;
            while (*remove_end && *remove_end != ' ') remove_end++;
            *remove_end = '\0';
            int k_idx = -1, r_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, keep) == 0) k_idx = (int)i;
                if (strcmp(list[i].name, remove) == 0) r_idx = (int)i;
            }
            if (k_idx != -1 && r_idx != -1) {
                char *k_note = list[k_idx].note;
                char *r_note = list[r_idx].note;
                size_t k_len = strlen(k_note);
                size_t r_len = strlen(r_note);
                char *new_note = NULL;
                if (k_len > 0 && r_len > 0) {
                    new_note = malloc(k_len + r_len + 2);
                    if (new_note) {
                        strcpy(new_note, k_note);
                        strcat(new_note, " ");
                        strcat(new_note, r_note);
                    }
                } else if (r_len > 0) {
                    new_note = mystrdup(r_note);
                } else {
                    new_note = mystrdup(k_note);
                }
                free(list[k_idx].note);
                list[k_idx].note = new_note;
                free(list[r_idx].name);
                free(list[r_idx].email);
                free(list[r_idx].phone);
                free(list[r_idx].note);
                for (size_t j = (size_t)r_idx; j < count - 1; j++) {
                    list[j] = list[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
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
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
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
