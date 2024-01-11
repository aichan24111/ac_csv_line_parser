#include "ac_csv_line_parser.h"
#include <assert.h>
#include <string.h>

void 
test_parse_csv_line_string(void) {
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        char *error;
        assert(ac_parse_csv_line_string(&cols, NULL, &error) == NULL);
        assert(strcmp(error, "ptr is null") == 0);
        free(error);

        ac_csv_columns_destroy(&cols);
    }
    {
        const char *s = "";
        char *error;
        assert(ac_parse_csv_line_string(NULL, &s, &error) == NULL);
        assert(strcmp(error, "columns is null") == 0);
        free(error);
    }
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        const char *s = "";
        char *error;
        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);
        assert(cols.len == 0);

        ac_csv_columns_destroy(&cols);
    }
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        const char *s = "";
        char *error;
        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);
        assert(cols.len == 0);
        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);
        assert(cols.len == 0);

        ac_csv_columns_destroy(&cols);
    }
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        const char *s = "123";
        char *error;
        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 1);
        assert(strcmp(cols.columns[0], "123") == 0);
        assert(cols.columns[1] == NULL);
        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);

        ac_csv_columns_destroy(&cols);
    }
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        const char *s = "123, \"223\",323";
        char *error;
        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 3);
        assert(strcmp(cols.columns[0], "123") == 0);
        assert(strcmp(cols.columns[1], " \"223\"") == 0);
        assert(strcmp(cols.columns[2], "323") == 0);
        assert(cols.columns[3] == NULL);
        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);

        ac_csv_columns_destroy(&cols);
    }
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        const char *s = "123,223,323";
        char *error;
        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 3);
        assert(strcmp(cols.columns[0], "123") == 0);
        assert(strcmp(cols.columns[1], "223") == 0);
        assert(strcmp(cols.columns[2], "323") == 0);
        assert(cols.columns[3] == NULL);
        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);

        ac_csv_columns_destroy(&cols);
    }
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        const char *s = "\\\n123,223\\\t,3\\\r23";
        char *error;
        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 3);
        assert(strcmp(cols.columns[0], "\n123") == 0);
        assert(strcmp(cols.columns[1], "223\t") == 0);
        assert(strcmp(cols.columns[2], "3\r23") == 0);
        assert(cols.columns[3] == NULL);
        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);

        ac_csv_columns_destroy(&cols);
    }
    {
        AcCsvColumns cols;
        assert(ac_csv_columns_init(&cols) != NULL);

        const char *s = "\"123 123\",223,323\r\nabc,\"def \\\n def\",ghi\rABC,DEF,\"GHI GHI\"\n423,523,623";
        char *error;

        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 3);
        assert(strcmp(cols.columns[0], "123 123") == 0);
        assert(strcmp(cols.columns[1], "223") == 0);
        assert(strcmp(cols.columns[2], "323") == 0);
        assert(cols.columns[3] == NULL);

        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 3);
        assert(strcmp(cols.columns[0], "abc") == 0);
        assert(strcmp(cols.columns[1], "def \n def") == 0);
        assert(strcmp(cols.columns[2], "ghi") == 0);
        assert(cols.columns[3] == NULL);

        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 3);
        assert(strcmp(cols.columns[0], "ABC") == 0);
        assert(strcmp(cols.columns[1], "DEF") == 0);
        assert(strcmp(cols.columns[2], "GHI GHI") == 0);
        assert(cols.columns[3] == NULL);

        puts("----------------------------------------------");
        assert(ac_parse_csv_line_string(&cols, &s, &error) != NULL);
        assert(cols.len == 3);
        assert(strcmp(cols.columns[0], "423") == 0);
        assert(strcmp(cols.columns[1], "523") == 0);
        assert(strcmp(cols.columns[2], "623") == 0);
        assert(cols.columns[3] == NULL);

        assert(ac_parse_csv_line_string(&cols, &s, &error) == NULL);

        ac_csv_columns_destroy(&cols);
    }
    {
        const char *s = "\"123 123\",223,323\r\nabc,\"def \\\n def\",ghi\rABC,DEF,\"GHI GHI\"\n423,523,623";
        char *error;

        // allocate memory.
        AcCsvColumns cols;
        if (ac_csv_columns_init(&cols) == NULL) {
            perror("ac_csv_columns_init");
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
    }
}

int
main(void) {
    test_parse_csv_line_string();
    return 0;
}
