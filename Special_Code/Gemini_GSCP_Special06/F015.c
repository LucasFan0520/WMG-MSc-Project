// F015.c
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
    char *note;
} Contact;

int main() {
    Contact *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '	' || *p == '\n' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *email = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *phone = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Contact *narr = realloc(arr, cap * sizeof(Contact));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].name = mystrdup(name);
                arr[count].email = mystrdup(email);
                arr[count].phone = mystrdup(phone);
                arr[count].note = mystrdup("");
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *text = p;
            size_t tl = strlen(text);
            while (tl > 0 && (text[tl-1] == '\n' || text[tl-1] == '\n')) {
                text[tl-1] = '\0';
                tl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].note);
                    arr[i].note = mystrdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *keep = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *remove = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int k_idx = -1;
            int r_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, keep) == 0) k_idx = i;
                if (strcmp(arr[i].name, remove) == 0) r_idx = i;
            }
            if (k_idx != -1 && r_idx != -1) {
                size_t kl = strlen(arr[k_idx].note);
                size_t rl = strlen(arr[r_idx].note);
                char *nn = malloc(kl + rl + 2);
                if (nn) {
                    strcpy(nn, arr[k_idx].note);
                    if (kl > 0 && rl > 0) {
                        strcat(nn, " ");
                        strcat(nn, arr[r_idx].note);
                    } else if (rl > 0) {
                        strcpy(nn, arr[r_idx].note);
                    }
                    free(arr[k_idx].note);
                    arr[k_idx].note = nn;
                }
                free(arr[r_idx].name);
                free(arr[r_idx].email);
                free(arr[r_idx].phone);
                free(arr[r_idx].note);
                for (int j = r_idx; j < count - 1; j++) {
                    arr[j] = arr[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].email);
                    free(arr[i].phone);
                    free(arr[i].note);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
