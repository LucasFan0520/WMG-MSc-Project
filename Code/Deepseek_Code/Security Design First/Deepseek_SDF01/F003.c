// filename: F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AliasNode {
    char *name;
    char *expansion;
    struct AliasNode *next;
} AliasNode;

static AliasNode *head = NULL;

static void cleanup(void) {
    while (head) {
        AliasNode *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->expansion);
        free(tmp);
    }
}

static AliasNode *find_alias(const char *name) {
    for (AliasNode *n = head; n; n = n->next)
        if (strcmp(n->name, name) == 0)
            return n;
    return NULL;
}

static void define_alias(const char *name, const char *exp) {
    AliasNode *exist = find_alias(name);
    if (exist) {
        char *new = strdup(exp);
        if (!new) return;
        free(exist->expansion);
        exist->expansion = new;
        return;
    }
    AliasNode *n = malloc(sizeof(AliasNode));
    if (!n) return;
    n->name = strdup(name);
    n->expansion = strdup(exp);
    if (!n->name || !n->expansion) {
        free(n->name);
        free(n->expansion);
        free(n);
        return;
    }
    n->next = head;
    head = n;
}

static void expand_alias(const char *name) {
    AliasNode *n = find_alias(name);
    if (!n) return;
    for (char *p = n->expansion; *p; ++p)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void rename_alias(const char *old, const char *new) {
    AliasNode *n = find_alias(old);
    if (!n) return;
    if (find_alias(new)) return;
    char *newname = strdup(new);
    if (!newname) return;
    free(n->name);
    n->name = newname;
}

static void erase_alias(const char *name) {
    AliasNode *prev = NULL;
    for (AliasNode *n = head; n; n = n->next) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n->name);
            free(n->expansion);
            free(n);
            return;
        }
        prev = n;
    }
}

static void dump_aliases(void) {
    for (AliasNode *n = head; n; n = n->next)
        printf("%s %s\n", n->name, n->expansion);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "DEFINE") == 0) {
            if (*arg1) define_alias(arg1, arg2);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (*arg1) expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (*arg1 && *arg2) rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (*arg1) erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free(line);
    return 0;
}