// F015.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *email = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *phone = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            Contact *temp = realloc(list, (count + 1) * sizeof(Contact));
            if (temp) {
                list = temp;
                list[count].name = strdup(name);
                list[count].email = strdup(email);
                list[count].phone = strdup(phone);
                list[count].note = strdup("");
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *text = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].note);
                    list[i].note = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *keep = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *remove = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int keep_idx = -1;
            int remove_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, keep) == 0) keep_idx = (int)i;
                if (strcmp(list[i].name, remove) == 0) remove_idx = (int)i;
            }
            if (keep_idx != -1 && remove_idx != -1) {
                size_t new_note_len = strlen(list[keep_idx].note) + strlen(list[remove_idx].note) + 2;
                char *new_note = malloc(new_note_len);
                if (new_note) {
                    if (strlen(list[keep_idx].note) > 0) {
                        sprintf(new_note, "%s %s", list[keep_idx].note, list[remove_idx].note);
                    } else {
                        strcpy(new_note, list[remove_idx].note);
                    }
                    free(list[keep_idx].note);
                    list[keep_idx].note = new_note;
                }
                free(list[remove_idx].name);
                free(list[remove_idx].email);
                free(list[remove_idx].phone);
                free(list[remove_idx].note);
                for (size_t j = (size_t)remove_idx; j < count - 1; j++) {
                    list[j] = list[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
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
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
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
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
        free(list[i].note);
    }
    free(list);
    free(line);
    return 0;
}
