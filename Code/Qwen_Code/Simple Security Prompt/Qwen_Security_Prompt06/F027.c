// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Rem {
    int pri;
    int ord;
    char *title;
    char *note;
    struct Rem *next;
} Rem;

Rem *rems = NULL;
int order_cnt = 0;

Rem *find_rem(const char *title) {
    for (Rem *r = rems; r; r = r->next) {
        if (strcmp(r->title, title) == 0) return r;
    }
    return NULL;
}

void add_rem(int pri, const char *title, const char *note) {
    if (find_rem(title)) return;
    Rem *r = malloc(sizeof(Rem));
    r->pri = pri;
    r->ord = order_cnt++;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = rems;
    rems = r;
}

void bump_rem(const char *title, int delta) {
    Rem *r = find_rem(title);
    if (r) r->pri += delta;
}

void remove_rem(const char *title) {
    Rem *prev = NULL, *curr = rems;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else rems = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int cmp_rem(const void *a, const void *b) {
    Rem *ra = *(Rem **)a;
    Rem *rb = *(Rem **)b;
    if (ra->pri != rb->pri) return rb->pri - ra->pri;
    return ra->ord - rb->ord;
}

void top_n(int n) {
    int count = 0;
    for (Rem *r = rems; r; r = r->next) count++;
    if (count == 0) return;
    
    Rem **arr = malloc(count * sizeof(Rem *));
    Rem *curr = rems;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    qsort(arr, count, sizeof(Rem *), cmp_rem);
    
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->pri, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_rems() {
    for (Rem *r = rems; r; r = r->next) {
        printf("%d %s %s\n", r->pri, r->title, r->note);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        int num = 0;
        sscanf(line, "%15s %255s %d", cmd, arg1, &num);
        
        if (strcmp(cmd, "ADD") == 0) {
            char *note = get_rest(line, 3);
            if (note) add_rem(num, arg1, note);
        }
        else if (strcmp(cmd, "BUMP") == 0) {
            sscanf(line, "%*s %*s %d", &num);
            bump_rem(arg1, num);
        }
        else if (strcmp(cmd, "REMOVE") == 0) remove_rem(arg1);
        else if (strcmp(cmd, "TOP") == 0) top_n(num);
        else if (strcmp(cmd, "LIST") == 0) list_rems();
        
        free(line);
    }
    return 0;
}