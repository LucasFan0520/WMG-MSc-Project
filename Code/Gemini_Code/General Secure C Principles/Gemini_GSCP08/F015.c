// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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

int main(void) {
    Contact *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s %s\n", arr[i].name, arr[i].email, arr[i].phone);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *email = p2 + 1;
                char *p3 = strchr(email, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *phone = p3 + 1;
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            size_t ncap = cap == 0 ? 4 : cap * 2;
                            Contact *narr = realloc(arr, ncap * sizeof(Contact));
                            if (narr) {
                                arr = narr;
                                cap = ncap;
                            }
                        }
                        if (count < cap) {
                            arr[count].name = strdup(name);
                            arr[count].email = strdup(email);
                            arr[count].phone = strdup(phone);
                            arr[count].note = NULL;
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        char *nnote = strdup(text);
                        if (nnote) {
                            free(arr[i].note);
                            arr[i].note = nnote;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = args;
            char *p2 = strchr(keep, ' ');
            if (p2) {
                *p2 = '\0';
                char *remove = p2 + 1;
                int k_idx = -1, r_idx = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, keep) == 0) k_idx = (int)i;
                    if (strcmp(arr[i].name, remove) == 0) r_idx = (int)i;
                }
                if (k_idx != -1 && r_idx != -1) {
                    char *knote = arr[k_idx].note;
                    char *rnote = arr[r_idx].note;
                    char *nmsg = NULL;
                    if (knote && rnote) {
                        size_t nlen = strlen(knote) + 1 + strlen(rnote) + 1;
                        nmsg = malloc(nlen);
                        if (nmsg) sprintf(nmsg, "%s %s", knote, rnote);
                    } else if (knote) {
                        nmsg = strdup(knote);
                    } else if (rnote) {
                        nmsg = strdup(rnote);
                    }
                    free(arr[k_idx].note);
                    arr[k_idx].note = nmsg;
                    free(arr[r_idx].name);
                    free(arr[r_idx].email);
                    free(arr[r_idx].phone);
                    free(arr[r_idx].note);
                    for (size_t j = (size_t)r_idx; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].email);
                    free(arr[i].phone);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s %s", arr[i].name, arr[i].email, arr[i].phone);
                    if (arr[i].note) {
                        printf(" %s", arr[i].note);
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
