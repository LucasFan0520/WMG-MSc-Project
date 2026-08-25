// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *tag; } Tag;
typedef struct { Tag *d; size_t len, cap; } TagL;
typedef struct { char *sender; char *subject; char *body; TagL tags; } Mail;
typedef struct { Mail *d; size_t len, cap; } ML;

char *read_line(void) {
    char *l = NULL;
    size_t len = 0;
    if (getline(&l, &len, stdin) == -1) {
        free(l);
        return NULL;
    }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

char *next_token(char **s) {
    if (!*s) return NULL;
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *st = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return st;
}

void print_text(const char *t) {
    for (size_t i = 0; t[i]; i++) putchar(t[i] == ' ' ? '_' : t[i]);
    putchar('\n');
}

int find_mail_sub(ML *l, const char *sub) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].subject, sub) == 0) return i;
    return -1;
}

int find_tag(TagL *l, const char *tag) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].tag, tag) == 0) return i;
    return -1;
}

int main(void) {
    ML list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = next_token(&p);
            char *subject = next_token(&p);
            char *body = p;
            if (sender && subject && body) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Mail *nd = realloc(list.d, nc * sizeof(Mail));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].sender = strdup(sender);
                    list.d[list.len].subject = strdup(subject);
                    list.d[list.len].body = strdup(body);
                    list.d[list.len].tags.d = NULL;
                    list.d[list.len].tags.len = 0;
                    list.d[list.len].tags.cap = 0;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char *sub = next_token(&p);
            char *tag = p;
            if (sub && tag) {
                int mi = find_mail_sub(&list, sub);
                if (mi != -1) {
                    TagL *tl = &list.d[mi].tags;
                    if (find_tag(tl, tag) == -1) {
                        if (tl->len == tl->cap) {
                            size_t nc = tl->cap ? tl->cap * 2 : 4;
                            Tag *nd = realloc(tl->d, nc * sizeof(Tag));
                            if (nd) { tl->d = nd; tl->cap = nc; }
                        }
                        if (tl->len < tl->cap) {
                            tl->d[tl->len++].tag = strdup(tag);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *sub = next_token(&p);
            char *tag = p;
            if (sub && tag) {
                int mi = find_mail_sub(&list, sub);
                if (mi != -1) {
                    TagL *tl = &list.d[mi].tags;
                    int ti = find_tag(tl, tag);
                    if (ti != -1) {
                        free(tl->d[ti].tag);
                        for (size_t i = ti; i < tl->len - 1; i++) tl->d[i] = tl->d[i + 1];
                        tl->len--;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *sub = next_token(&p);
            if (sub) {
                int mi = find_mail_sub(&list, sub);
                if (mi != -1) {
                    free(list.d[mi].sender);
                    free(list.d[mi].subject);
                    free(list.d[mi].body);
                    for (size_t i = 0; i < list.d[mi].tags.len; i++) free(list.d[mi].tags.d[i].tag);
                    free(list.d[mi].tags.d);
                    for (size_t i = mi; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = next_token(&p);
            if (tag) {
                for (size_t i = 0; i < list.len; i++) {
                    if (find_tag(&list.d[i].tags, tag) != -1) {
                        printf("%s\n", list.d[i].subject);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %s ", list.d[i].sender, list.d[i].subject);
                print_text(list.d[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].sender);
        free(list.d[i].subject);
        free(list.d[i].body);
        for (size_t j = 0; j < list.d[i].tags.len; j++) free(list.d[i].tags.d[j].tag);
        free(list.d[i].tags.d);
    }
    free(list.d);
    return 0;
}