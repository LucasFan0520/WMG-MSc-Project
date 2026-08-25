// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *encoded;
    struct Encoded *next;
} Encoded;

Encoded *head = NULL;

Encoded* find_encoded(const char *name) {
    Encoded *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int decode_encoded(const char *enc, char **decoded) {
    // parse counts and chars
    int total_len = 0;
    const char *p = enc;
    while (*p) {
        if (!isdigit(*p)) return 0; // must start with digit
        long count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            if (count > 1000000) return 0; // treat very large as invalid
            p++;
        }
        if (*p == '\0') return 0; // no char after count
        char ch = *p;
        p++;
        total_len += count;
        if (total_len > 1000000) return 0; // too large
    }
    char *out = malloc(total_len + 1);
    char *ptr = out;
    p = enc;
    while (*p) {
        long count = 0;
        while (isdigit(*p)) {
            count = count * 10 + (*p - '0');
            p++;
        }
        char ch = *p++;
        for (long i = 0; i < count; i++) {
            *ptr++ = ch;
        }
    }
    *ptr = 0;
    *decoded = out;
    return 1;
}

void store_encoded(const char *name, const char *enc) {
    if (find_encoded(name)) return;
    // validate basic structure (starts with digit, ends with char)
    if (!isdigit(enc[0])) return;
    // We'll store even if later decode fails, but spec says store encoded string
    Encoded *new = malloc(sizeof(Encoded));
    new->name = strdup(name);
    new->encoded = strdup(enc);
    new->next = NULL;
    if (!head) head = new;
    else {
        Encoded *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void decode_show(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    char *decoded;
    if (!decode_encoded(e->encoded, &decoded)) return;
    char *p = decoded;
    while (*p) {
        if (*p == ' ') putchar('_');
        else putchar(*p);
        p++;
    }
    putchar('\n');
    free(decoded);
}

void delete_encoded(const char *name) {
    Encoded *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->encoded);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_encoded() {
    Encoded *cur = head;
    while (cur) {
        printf("%s\n", cur->name);
        cur = cur->next;
    }
}

void free_all() {
    Encoded *cur = head;
    while (cur) {
        Encoded *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->encoded);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "STORE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *enc = strtok(NULL, "");
            if (!enc) enc = "";
            store_encoded(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) decode_show(name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free_all();
    return 0;
}