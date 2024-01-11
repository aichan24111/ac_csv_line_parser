#include "ac_csv_line_parser.h"

/* String */

typedef struct {
    char *str;
    size_t len;
    size_t capa;
} String;

static String *
str_init(String *self) {
    self->len = 0;
    self->capa = 4;

    size_t byte = sizeof(char);
    size_t size = byte * self->capa + byte;
    self->str = malloc(size);
    if (self->str == NULL) {
        return NULL;
    }

    self->str[self->len] = 0;

    return self;
}

static void
str_destroy(String *self) {
    if (self == NULL) {
        return;
    }

    free(self->str);
}

static char *
str_esc_destroy(String *self) {
    if (self == NULL) {
        return NULL;
    }

    char *s = self->str;

    self->len = 0;
    self->capa = 4;
    size_t byte = sizeof(char);
    size_t size = byte * self->capa + byte;
    self->str = malloc(size);
    if (self->str == NULL) {
        free(s);
        return NULL;
    }
    self->str[self->len] = 0;

    return s;
}

static String *
str_resize(String *self, size_t new_capa) {
    size_t byte = sizeof(char);
    size_t size = byte * new_capa + byte;
    char *tmp = realloc(self->str, size);
    if (tmp == NULL) {
        return NULL;
    }

    self->str = tmp;
    self->capa = new_capa;

    return self;
}

static String *
str_push_back(String *self, char c) {
    if (self->len >= self->capa) {
        if (str_resize(self, self->capa * 2) == NULL) {
            return NULL;
        }
    }

    self->str[self->len++] = c;
    self->str[self->len] = 0;
    return self;
}

/* AcCsvColumns */

void
ac_csv_columns_destroy(AcCsvColumns *self) {
    if (self == NULL) {
        return;
    }

    for (size_t i = 0; i < self->len; i++) {
        free(self->columns[i]);
    }
    free(self->columns);
}

AcCsvColumns *
ac_csv_columns_init(AcCsvColumns *self) {
    self->len = 0;
    self->capa = 4;

    size_t byte = sizeof(char *);
    size_t size = byte * self->capa + byte;
    self->columns = malloc(size);
    if (self->columns == NULL) {
        return NULL;
    }

    self->columns[self->len] = NULL;

    return self;
}

void
ac_csv_columns_clear(AcCsvColumns *self) {
    for (size_t i = 0; i < self->len; i++) {
        free(self->columns[i]);
        self->columns[i] = NULL;
    }

    self->len = 0;
}

AcCsvColumns *
ac_csv_columns_resize(AcCsvColumns *self, size_t new_capa) {
    size_t byte = sizeof(char *);
    size_t size = byte * new_capa + byte;
    char **tmp = realloc(self->columns, size);
    if (tmp == NULL) {
        return NULL;
    }

    self->columns = tmp;
    self->capa = new_capa;

    return self;
}

AcCsvColumns *
ac_csv_columns_move_back(AcCsvColumns *self, char *move_ptr) {
    if (self->len >= self->capa) {
        if (ac_csv_columns_resize(self, self->capa * 2) == NULL) {
            return NULL;
        }
    }

    self->columns[self->len++] = move_ptr;
    self->columns[self->len] = NULL;
    return self;
}

const char *
ac_csv_columns_getc(const AcCsvColumns *self, size_t index) {
    if (index >= self->len) {
        return NULL;
    }
    return self->columns[index];
}

/* Utility */

char *
_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p == NULL) {
        return NULL;
    }

    strcpy(p, s);
    return p;
}

#define store() {\
        char *es = str_esc_destroy(&s);\
        if (ac_csv_columns_move_back(columns, es) == NULL) {\
            *error = _strdup("failed to move back column at columns");\
            goto fail;\
        }\
    }\

#define push(c) {\
        if (str_push_back(&s, (c)) == NULL) {\
            *error = _strdup("failed to push back character at string");\
            goto fail;\
        }\
    }\

AcCsvColumns *
ac_parse_csv_line_string(AcCsvColumns *columns, const char **ptr, char **error) {
    *error = NULL;

    if (columns == NULL) {
        *error = _strdup("columns is null");
        return NULL;
    }
    if (ptr == NULL) {
        *error = _strdup("ptr is null");
        return NULL;
    }
    if (**ptr == '\0') {
        return NULL;  // ok
    }

    const char *q = *ptr;
    int m = 0;
    String s;
    if (str_init(&s) == NULL) {
        *error = _strdup("failed to initialize string");
        return NULL;
    }

    ac_csv_columns_clear(columns);

    for (; *q; q++) {
        char c1 = *q;
        char c2 = *(q + 1);

        // printf("m[%d] c1[%c] c2[%c]\n", m, c1, c2);

        switch (m) {
        case 0:
            if (c1 == '\r' && c2 == '\n') {
                q += 2;
                goto done;
            } else if (c1 == '\r' || c1 == '\n') {
                q++;
                goto done;
            } else if (c1 == '\\') {
                q++;
                push(*q);
                m = 200;
            } else if (c1 == ',') {
                store();
            } else if (c1 == '"') {
                m = 100;
            } else {
                m = 200;
                push(c1);
            }
            break;
        case 100:
            if (c1 == '\\') {
                q++;
                push(*q);
            } else if (c1 == '"') {
                store();
                m = 150;
            } else {
                push(c1);
            }
            break;
        case 150:
            if (c1 == ',') {
                m = 0;
            } else if (c1 == '\r' && c2 == '\n') {
                q += 2;
                goto done;
            } else if (c1 == '\r' || c1 == '\n') {
                q++;
                goto done;
            }
            break;
        case 200:
            if (c1 == '\r' && c2 == '\n') {
                q += 2;
                store();
                goto done;
            } else if (c1 == '\r' || c1 == '\n') {
                store();
                q++;
                goto done;
            } else if (c1 == '\\') {
                q++;
                push(*q);                
            } else if (c1 == ',') {
                store();
                m = 0;
            } else {
                push(c1);
            }
            break;
        }
    }

done:
    if (s.len) {
        store();
    }
    str_destroy(&s);
    *ptr = q;
    return columns;

fail:
    str_destroy(&s);
    return NULL;
}
