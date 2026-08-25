// F023.c
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
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
} Mail;

int main() {
    Mail *arr = NULL;
    int count = 0;
    int cap = 0;
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
        if (strcmp(cmd, "MAIL") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *sender = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *body = p;
            size_t bl = strlen(body);
            while (bl > 0 && (body[bl-1] == '\r' || body[bl-1] == '\n')) {
                body[bl-1] = '\0';
                bl--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Mail *narr = realloc(arr, cap * sizeof(Mail));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].sender = mystrdup(sender);
                arr[count].subject = mystrdup(subject);
                arr[count].body = mystrdup(body);
                arr[count].tags = NULL;
                arr[count].tag_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subject) == 0) {
                    arr[i].tags = realloc(arr[i].tags, (arr[i].tag_count + 1) * sizeof(char *));
                    if (arr[i].tags) {
                        arr[i].tags[arr[i].tag_count] = mystrdup(tag);
                        arr[i].tag_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subject) == 0) {
                    for (int j = 0; j < arr[i].tag_count; j++) {
                        if (strcmp(arr[i].tags[j], tag) == 0) {
                            free(arr[i].tags[j]);
                            for (int k = j; k < arr[i].tag_count - 1; k++) {
                                arr[i].tags[k] = arr[i].tags[k + 1];
                            }
                            arr[i].tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subject) == 0) {
                    free(arr[i].sender);
                    free(arr[i].subject);
                    free(arr[i].body);
                    for (int j = 0; j < arr[i].tag_count; j++) {
                        free(arr[i].tags[j]);
                    }
                    free(arr[i].tags);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < arr[i].tag_count; j++) {
                    if (strcmp(arr[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", arr[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", arr[i].sender, arr[i].subject, arr[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].sender);
        free(arr[i].subject);
        free(arr[i].body);
        for (int j = 0; j < arr[i].tag_count; j++) {
            free(arr[i].tags[j]);
        }
        free(arr[i].tags);
    }
    free(arr);
    return 0;
}
