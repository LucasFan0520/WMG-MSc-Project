// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *tag;
    char *message;
} Record;

int main() {
    Record *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *tag_start = line + 4;
            char *space = strchr(tag_start, ' ');
            char *msg_start = "";
            if (space) {
                *space = '\0';
                msg_start = space + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(Record));
            }
            arr[count].tag = strdup(tag_start);
            arr[count].message = strdup(msg_start);
            count++;
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p = line + 7;
            char *tag1 = p;
            char *space1 = strchr(tag1, ' ');
            if (space1) {
                *space1 = '\0';
                char *tag2 = space1 + 1;
                char *space2 = strchr(tag2, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *newtag = space2 + 1;
                    char *m1 = NULL;
                    char *m2 = NULL;
                    for (size_t i = 0; i < count; i++) {
                        if (!m1 && strcmp(arr[i].tag, tag1) == 0) {
                            m1 = arr[i].message;
                        }
                        if (!m2 && strcmp(arr[i].tag, tag2) == 0) {
                            m2 = arr[i].message;
                        }
                    }
                    if (m1 && m2) {
                        size_t nlen = strlen(m1) + 1 + strlen(m2) + 1;
                        char *nmsg = malloc(nlen);
                        strcpy(nmsg, m1);
                        strcat(nmsg, " ");
                        strcat(nmsg, m2);
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            arr = realloc(arr, cap * sizeof(Record));
                        }
                        arr[count].tag = strdup(newtag);
                        arr[count].message = nmsg;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag_start = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].tag, tag_start) == 0) {
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
            char *tag_start = line + 5;
            char *found_msg = NULL;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].tag, tag_start) == 0) {
                    found_msg = arr[i].message;
                    break;
                }
            }
            if (found_msg) {
                while (*found_msg) {
                    if (*found_msg == ' ') putchar('_');
                    else putchar(*found_msg);
                    found_msg++;
                }
                putchar('\n');
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].tag, arr[i].message);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].tag);
        free(arr[i].message);
    }
    free(arr);
    return 0;
}
