// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tag_count;
    size_t tag_capacity;
} Message;

int main(void) {
    Message *box = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = p;
            while (*sender == ' ') sender++;
            char *sender_end = sender;
            while (*sender_end && *sender_end != ' ') sender_end++;
            char *subject = sender_end;
            if (*sender_end != '\0') {
                *sender_end = '\0';
                subject++;
            }
            while (*subject == ' ') subject++;
            char *subject_end = subject;
            while (*subject_end && *subject_end != ' ') subject_end++;
            char *body = subject_end;
            if (*subject_end != '\0') {
                *subject_end = '\0';
                body++;
            }
            while (*body == ' ') body++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Message *new_box = realloc(box, capacity * sizeof(Message));
                if (new_box) box = new_box;
            }
            box[count].sender = mystrdup(sender);
            box[count].subject = mystrdup(subject);
            box[count].body = mystrdup(body);
            box[count].tags = NULL;
            box[count].tag_count = 0;
            box[count].tag_capacity = 0;
            count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = p;
            while (*subject == ' ') subject++;
            char *subject_end = subject;
            while (*subject_end && *subject_end != ' ') subject_end++;
            char *tag = subject_end;
            if (*subject_end != '\0') {
                *subject_end = '\0';
                tag++;
            }
            while (*tag == ' ') tag++;
            char *tag_end = tag;
            while (*tag_end && *tag_end != ' ') tag_end++;
            *tag_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(box[i].subject, subject) == 0) {
                    Message *m = &box[i];
                    if (m->tag_count >= m->tag_capacity) {
                        m->tag_capacity = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
                        char **new_tags = realloc(m->tags, m->tag_capacity * sizeof(char *));
                        if (new_tags) m->tags = new_tags;
                    }
                    m->tags[m->tag_count++] = mystrdup(tag);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = p;
            while (*subject == ' ') subject++;
            char *subject_end = subject;
            while (*subject_end && *subject_end != ' ') subject_end++;
            char *tag = subject_end;
            if (*subject_end != '\0') {
                *subject_end = '\0';
                tag++;
            }
            while (*tag == ' ') tag++;
            char *tag_end = tag;
            while (*tag_end && *tag_end != ' ') tag_end++;
            *tag_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(box[i].subject, subject) == 0) {
                    Message *m = &box[i];
                    for (size_t j = 0; j < m->tag_count; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            free(m->tags[j]);
                            for (size_t k = j; k < m->tag_count - 1; k++) {
                                m->tags[k] = m->tags[k + 1];
                            }
                            m->tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = p;
            while (*subject == ' ') subject++;
            char *subject_end = subject;
            while (*subject_end && *subject_end != ' ') subject_end++;
            *subject_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(box[i].subject, subject) == 0) {
                    free(box[i].sender);
                    free(box[i].subject);
                    free(box[i].body);
                    for (size_t j = 0; j < box[i].tag_count; j++) {
                        free(box[i].tags[j]);
                    }
                    free(box[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        box[j] = box[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = p;
            while (*tag == ' ') tag++;
            char *tag_end = tag;
            while (*tag_end && *tag_end != ' ') tag_end++;
            *tag_end = '\0';
            for (size_t i = 0; i < count; i++) {
                int has_tag = 0;
                for (size_t j = 0; j < box[i].tag_count; j++) {
                    if (strcmp(box[i].tags[j], tag) == 0) {
                        has_tag = 1;
                        break;
                    }
                }
                if (has_tag) {
                    printf("%s\n", box[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", box[i].sender, box[i].subject, box[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(box[i].sender);
        free(box[i].subject);
        free(box[i].body);
        for (size_t j = 0; j < box[i].tag_count; j++) {
            free(box[i].tags[j]);
        }
        free(box[i].tags);
    }
    free(box);
    return 0;
}
