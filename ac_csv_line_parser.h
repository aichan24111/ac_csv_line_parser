#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* AcCsvColumns */

typedef struct {
    char **columns;
    size_t len;
    size_t capa;
} AcCsvColumns;

void
ac_csv_columns_destroy(AcCsvColumns *self);

AcCsvColumns *
ac_csv_columns_init(AcCsvColumns *self);

void
ac_csv_columns_clear(AcCsvColumns *self);

AcCsvColumns *
ac_csv_columns_resize(AcCsvColumns *self, size_t new_capa);

AcCsvColumns *
ac_csv_columns_move_back(AcCsvColumns *self, char *move_ptr);

const char *
ac_csv_columns_getc(const AcCsvColumns *self, size_t index);

/* Utility */

/**
 * Parse CSV line and store columns at the columns argument. 
 */
AcCsvColumns *
ac_parse_csv_line_string(AcCsvColumns *columns, const char **ptr, char **error);
