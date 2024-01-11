# ac_csv_line_parser

CSV Parser made in C.

```c
#include "ac_csv_line_parser.h"

int main(void) {
    const char *s = "\"123 123\",223,323\r\nabc,\"def \\\n def\",ghi\rABC,DEF,\"GHI GHI\"\n423,523,623";
    char *error;

    // allocate memory.
    AcCsvColumns cols;
    if (ac_csv_columns_init(&cols) == NULL) {
        perror("ac_csv_columns_init");
        return 1;
    }

    // parse lines.
    while (ac_parse_csv_line_string(&cols, &s, &error) != NULL) {
        for (size_t i = 0; i < cols.len; i++) {
            const char *col = ac_csv_columns_getc(&cols, i);
            printf("%ld [%s]\n", i, col);
        }
    }
    if (error) {
        // if that occures errors then `error` is not null.
        fprintf(stderr, "error: %s\n", error);
        free(error);
    }

    // free memory.
    ac_csv_columns_destroy(&cols);
    
    return 0;
}
```

Lisence: MIT
