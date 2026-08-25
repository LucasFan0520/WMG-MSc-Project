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
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *email = rest;
                    char *phone = sp2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Contact *next = realloc(list, capacity * sizeof(Contact));
                        if (!next) break;
                        list = next;
                    }
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    list[count].note = strdup("");
                    count++;
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *text = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        free(list[i].note);
                        list[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *keep = args;
                char *remove = space + 1;
                int idx_k = -1;
                int idx_r = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].name, keep) == 0) idx_k = (int)i;
                    if (strcmp(list[i].name, remove) == 0) idx_r = (int)i;
                }
                if (idx_k != -1 && idx_r != -1) {
                    size_t l1 = strlen(list[idx_k].note);
                    size_t l2 = strlen(list[idx_r].note);
                    char *combined = malloc(l1 + l2 + 2);
                    if (combined) {
                        if (l1 > 0 && l2 > 0) {
                            strcpy(combined, list[idx_k].note);
                            combined[l1] = ' ';
                            strcpy(combined + l1 + 1, list[idx_r].note);
                        } else if (l1 > 0) {
                            strcpy(combined, list[idx_k].note);
                        } else {
                            strcpy(combined, list[idx_r].note);
                        }
                        free(list[idx_k].note);
                        list[idx_k].note = combined;
                    }
                    free(list[idx_r].name);
                    free(list[idx_r].email);
                    free(list[idx_r].phone);
                    free(list[idx_r].note);
                    for (size_t j = (size_t)idx_r; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
        free(list[i].note);
    }
    free(list);
    return 0;
}
