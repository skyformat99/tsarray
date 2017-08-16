/*
 * dynarray - type-safe dynamic array library
 * Copyright 2012, 2015, 2016, 2017 Israel G. Lugo
 *
 * This file is part of dynarray.
 *
 * dynarray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dynarray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dynarray.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For suggestions, feedback or bug reports: israel.lugo@lugosys.com
 */



/*
 * dynarray.h - dynamic array module header
 */


#ifndef _DYNARRAY_H
#define _DYNARRAY_H

#if HAVE_CONFIG_H
#  include <config.h>
#endif


/* get NULL and size_t */
#include <stddef.h>

/* get memory allocation */
#include <stdlib.h>


#include "common.h"


/*
 * Error values returned by API functions. Always negative in case of error.
 */
enum dynarray_errno {
    DYNARRAY_EOK = 0,        /* Success */
    DYNARRAY_EINVAL = -1,    /* Invalid argument */
    DYNARRAY_ENOENT = -2,    /* No such entry */
    DYNARRAY_ENOMEM = -3,    /* Out of memory */
    DYNARRAY_EOVERFLOW = -4, /* Operation would overflow */
};


/* abstract versions; only for internal use (must match the subclassed
 * versions in DYNARRAY_TYPE_DECLARE) */
struct _dynarray_abs {
    int len;
    int used_count;
    int min_len;
    struct _item_abs *items;
};


int dynarray_add(struct _dynarray_abs *p_dynarray, const void *object,
        size_t obj_size, size_t item_size) __attribute__((nonnull (1)));

int dynarray_remove(struct _dynarray_abs *p_dynarray, int index,
        size_t item_size) __NON_NULL;

int dynarray_compact(struct _dynarray_abs *p_dynarray, int force,
        size_t obj_size, size_t item_size) __NON_NULL;

int dynarray_truncate(struct _dynarray_abs *p_dynarray, int len,
        size_t item_size) __NON_NULL;

int dynarray_setminlen(struct _dynarray_abs *p_dynarray, int min_len,
        size_t item_size) __NON_NULL;


/* Initializer for an empty dynarray. May be used directly as initializer on
 * a declaration, or as rvalue on an assignment expression (for an already
 * declared identifier). In the latter case, this must be must be transformed
 * into a compound literal (by prepending the type name in parenthesis), e.g.:
 *      a1 = (intarray)DYNARRAY_EMPTY;
 */
#define DYNARRAY_EMPTY { 0, 0, 0, NULL }


#define DYNARRAY_TYPE_DECLARE(name, type) \
    struct name##_item { \
        int used; \
        type object; \
    }; \
    typedef struct { \
        int len; \
        int used_count; \
        int min_len; \
        struct name##_item *items; \
    } name; \
    static inline int name##_add(name *array, type *object) { \
        return dynarray_add((struct _dynarray_abs *)array, object, \
                            sizeof(type), sizeof(struct name##_item)); \
    } \
    static inline int name##_remove(name *array, int index) { \
        return dynarray_remove((struct _dynarray_abs *)array, index, \
                               sizeof(struct name##_item)); \
    } \
    static inline type *name##_get_nth(name *array, int index) { \
        struct name##_item *item = &array->items[index]; \
        return likely(item->used) ? &item->object : NULL; \
    } \
    static inline int name##_compact(name *array, int force) { \
        return dynarray_compact((struct _dynarray_abs *)array, force, \
                                sizeof(type), sizeof(struct name##_item)); \
    } \
    static inline int name##_truncate(name *array, int len) { \
        return dynarray_truncate((struct _dynarray_abs *)array, len, \
                                 sizeof(struct name##_item)); \
    } \
    static inline int name##_setminlen(name *array, int len) { \
        return dynarray_setminlen((struct _dynarray_abs *)array, len, \
                                  sizeof(struct name##_item)); \
    }



#endif      /* not _DYNARRAY_H */


/* vim: set expandtab smarttab shiftwidth=4 softtabstop=4 tw=78 : */
