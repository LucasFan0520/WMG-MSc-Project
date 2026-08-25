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

int main() {
    Contact *list = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *email = strtok(NULL, " ");
            char *phone = strtok(NULL, " ");
            if (name && email && phone) {
                list = realloc(list, (count + 1) * sizeof(Contact));
                list[count].name = strdup(name);
                list[count].email = strdup(email);
                list[count].phone = strdup(phone);
                list[count].note = strdup("");
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (name && text) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        free(list[i].note);
                        list[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = strtok(NULL, " ");
            char *remove = strtok(NULL, " ");
            if (keep && remove) {
                int idxK = -1, idxR = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, keep) == 0) idxK = i;
                    if (strcmp(list[i].name, remove) == 0) idxR = i;
                }
                if (idxK != -1 && idxR != -1) {
                    int len = strlen(list[idxK].note) + 1 + strlen(list[idxR].note) + 1;
                    char *new_note = malloc(len);
                    if (strlen(list[idxK].note) > 0) {
                        sprintf(new_note, "%s %s", list[idxK].note, list[idxR].note);
                    } else {
                        strcpy(new_note, list[idxR].note);
                    }
                    free(list[idxK].note);
                    list[idxK].note = new_note;
                    free(list[idxR].name);
                    free(list[idxR].email);
                    free(list[idxR].phone);
                    free(list[idxR].note);
                    for (int j = idxR; j < count - 1; j++) {
                        list[j] = list[j + 1];
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
            char *name = strtok(NULL, " ");
            if (name) {
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
                        i--;
                    }
                }
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Contact));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
            }
        }
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
