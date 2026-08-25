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

int main(void) {
    Contact *list = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(line + 4, "%255s %255s %255s", name, email, phone) == 3) {
                Contact *tmp = realloc(list, (count + 1) * sizeof(Contact));
                if (tmp) {
                    list = tmp;
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    list[count].note = strdup("");
                    count++;
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char name[256], text[1024];
            if (sscanf(line + 5, "%255s %[^\n]", name, text) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        char *nt = strdup(text);
                        if (nt) {
                            free(list[i].note);
                            list[i].note = nt;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char keep[256], remove[256];
            if (sscanf(line + 6, "%255s %255s", keep, remove) == 2) {
                int kidx = -1, ridx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, keep) == 0) {
                        kidx = i;
                    }
                    if (strcmp(list[i].name, remove) == 0) {
                        ridx = i;
                    }
                }
                if (kidx != -1 && ridx != -1) {
                    size_t nlen = strlen(list[kidx].note) + 1 + strlen(list[ridx].note) + 1;
                    char *nn = malloc(nlen);
                    if (nn) {
                        if (strlen(list[kidx].note) > 0 && strlen(list[ridx].note) > 0) {
                            sprintf(nn, "%s %s", list[kidx].note, list[ridx].note);
                        } else if (strlen(list[kidx].note) > 0) {
                            strcpy(nn, list[kidx].note);
                        } else {
                            strcpy(nn, list[ridx].note);
                        }
                        free(list[kidx].note);
                        list[kidx].note = nn;
                    }
                    free(list[ridx].name);
                    free(list[ridx].email);
                    free(list[ridx].phone);
                    free(list[ridx].note);
                    for (int j = ridx; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].email);
                    free(list[i].phone);
                    free(list[i].note);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
        free(list[i].note);
    }
    free(list);
    return 0;
}
