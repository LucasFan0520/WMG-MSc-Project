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
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(ptr, "%255s %255s %255s", name, email, phone) == 3) {
                list = realloc(list, (count + 1) * sizeof(Contact));
                list[count].name = strdup(name);
                list[count].email = strdup(email);
                list[count].phone = strdup(phone);
                list[count].note = strdup("");
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256];
            int n_len = 0;
            if (sscanf(ptr, "%255s%n", name, &n_len) > 0) {
                char *text = ptr + n_len;
                while (*text == ' ') text++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        free(list[i].note);
                        list[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove_name[256];
            if (sscanf(ptr, "%255s %255s", keep, remove_name) == 2) {
                int k_idx = -1, r_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, keep) == 0) k_idx = i;
                    if (strcmp(list[i].name, remove_name) == 0) r_idx = i;
                }
                if (k_idx != -1 && r_idx != -1) {
                    int new_len = strlen(list[k_idx].note) + strlen(list[r_idx].note) + 2;
                    char *new_note = malloc(new_len);
                    if (strlen(list[k_idx].note) > 0) {
                        sprintf(new_note, "%s %s", list[k_idx].note, list[r_idx].note);
                    } else {
                        strcpy(new_note, list[r_idx].note);
                    }
                    free(list[k_idx].note);
                    list[k_idx].note = new_note;
                    free(list[r_idx].name);
                    free(list[r_idx].email);
                    free(list[r_idx].phone);
                    free(list[r_idx].note);
                    for (int i = r_idx; i < count - 1; i++) {
                        list[i] = list[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        list = realloc(list, count * sizeof(Contact));
                    } else {
                        free(list);
                        list = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
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
                    if (count > 0) {
                        list = realloc(list, count * sizeof(Contact));
                    } else {
                        free(list);
                        list = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s %s %s %s\n", list[found].name, list[found].email, list[found].phone, list[found].note);
                } else {
                    printf("NOT_FOUND\n");
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
