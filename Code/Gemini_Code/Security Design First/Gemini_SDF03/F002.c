// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} MessagePair;

int main(void) {
    MessagePair *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            char *msg = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                msg = p;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                MessagePair *tmp = realloc(arr, capacity * sizeof(MessagePair));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].tag = strdup(tag);
            arr[count].message = strdup(msg);
            if (!arr[count].tag || !arr[count].message) exit(1);
            count++;
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            char *tag2 = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                tag2 = p;
            }
            while (*p && *p != ' ') p++;
            char *newtag = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                newtag = p;
            }
            char *msg1 = NULL;
            char *msg2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!msg1 && strcmp(arr[i].tag, tag1) == 0) {
                    msg1 = arr[i].message;
                }
                if (!msg2 && strcmp(arr[i].tag, tag2) == 0) {
                    msg2 = arr[i].message;
                }
            }
            if (msg1 && msg2) {
                size_t nlen = strlen(msg1) + strlen(msg2) + 2;
                char *nmsg = malloc(nlen);
                if (!nmsg) exit(1);
                snprintf(nmsg, nlen, "%s %s", msg1, msg2);
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    MessagePair *tmp = realloc(arr, capacity * sizeof(MessagePair));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].tag = strdup(newtag);
                arr[count].message = nmsg;
                if (!arr[count].tag) exit(1);
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') tag++;
            for (size_t i = 0; i < count; ) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    free(arr[i].tag);
                    free(arr[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            while (*tag == ' ') tag++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    char *t = arr[i].message;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].tag, arr[i].message);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].tag);
        free(arr[i].message);
    }
    free(arr);
    free(line);
    return 0;
}
