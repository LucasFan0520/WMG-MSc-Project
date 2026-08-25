// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

int main(int argc, char **argv) {
    CSVContact *arr = NULL;
    int count = 0;
    int cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline;
            while ((fline = read_line(f)) != NULL) {
                char *c1 = strchr(fline, ',');
                if (c1) {
                    char *c2 = strchr(c1 + 1, ',');
                    if (c2) {
                        *c1 = '\0';
                        *c2 = '\0';
                        char *name = fline;
                        char *email = c1 + 1;
                        char *phone = c2 + 1;
                        size_t pl = strlen(phone);
                        while (pl > 0 && (phone[pl-1] == '\r' || phone[pl-1] == '\n')) {
                            phone[pl-1] = '\0';
                            pl--;
                        }
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            CSVContact *narr = realloc(arr, cap * sizeof(CSVContact));
                            if (narr) arr = narr;
                        }
                        if (count < cap) {
                            arr[count].name = mystrdup(name);
                            arr[count].email = mystrdup(email);
                            arr[count].phone = mystrdup(phone);
                            count++;
                        }
                    }
                }
                free(fline);
            }
            fclose(f);
        }
    }
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *args = p;
            size_t al = strlen(args);
            while (al > 0 && (args[al-1] == '\r' || args[al-1] == '\n')) {
                args[al-1] = '\0';
                al--;
            }
            char *c1 = strchr(args, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = args;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        CSVContact *narr = realloc(arr, cap * sizeof(CSVContact));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count].name = mystrdup(name);
                        arr[count].email = mystrdup(email);
                        arr[count].phone = mystrdup(phone);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            size_t nl = strlen(name);
            while (nl > 0 && (name[nl-1] == '\r' || name[nl-1] == '\n')) {
                name[nl-1] = '\0';
                nl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].email);
                    free(arr[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            size_t nl = strlen(name);
            while (nl > 0 && (name[nl-1] == '\r' || name[nl-1] == '\n')) {
                name[nl-1] = '\0';
                nl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
    }
    free(arr);
    return 0;
}
