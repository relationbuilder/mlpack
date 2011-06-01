/* MLPACK 0.2
 *
 * Copyright (c) 2008, 2009 Alexander Gray,
 *                          Garry Boyer,
 *                          Ryan Riegel,
 *                          Nikolaos Vasiloglou,
 *                          Dongryeol Lee,
 *                          Chip Mappus,
 *                          Nishant Mehta,
 *                          Hua Ouyang,
 *                          Parikshit Ram,
 *                          Long Tran,
 *                          Wee Chin Wong
 *
 * Copyright (c) 2008, 2009 Georgia Institute of Technology
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
/**
 * @file common.h
 *
 * The bare necessities of FASTlib programming in C, including
 * standard types, formatted messages to stderr, and useful libraries
 * and compiler directives.
 *
 * This file should be included before all built-in libraries because
 * it includes the _REENTRANT definition needed for thread-safety.
 * Files base.h or fastlib.h include this file first and may serve as
 * surrogates.
 *
 * @see compiler.h
 */

#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "basic_types.h" /*generated by build*/
#include "compiler.h"
#include "ansi_colors.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A no-op used in some macros. */
#define NOP ((void)0)

/** Potentially useful for nasty macro expansion. */
#define COMMA ,



/* Types and definitions to assist managment of problem scale. */

/* Ensure that one and only one problem scale is selected. */
#if defined(SCALE_MASSIVE)
#if defined(SCALE_LARGE) || defined(SCALE_NORMAL)
#error Only one of SCALE_MASSIVE, SCALE_LARGE, or SCALE_NORMAL may be defined.
#endif
#elif defined(SCALE_LARGE)
#if defined(SCALE_NORMAL)
#error Only one of SCALE_MASSIVE, SCALE_LARGE, or SCALE_NORMAL may be defined.
#endif
#elif !defined(SCALE_NORMAL)
#define SCALE_NORMAL
#endif

/**
 * Index type used in FASTlib for array sizes, etc.
 *
 * Define one of the following (default SCALE_NORMAL):
 *
 * SCALE_NORMAL - Problems are as large can be indexed with your
 *   machine's standard integers.  As of 2007, this is 32 bits.
 *
 * SCALE_LARGE - Problems are as large as your machine's architecture
 *   can support.  Unless you have more than 2 billion data points,
 *   this will waste some space and reduce cache efficiency.
 *
 * SCALE_MASSIVE - Problems are 64-bit indexed even on 32-bit
 *   machines, rendering them larger than RAM.
 *
 * Values are signed to allow uninitialized indices of -1 as well as
 * consideration of potentially negative differences between indices.
 */

/**
 * Length modifier for emitting index_t with printf; presumably,
 *   adoption of cout and cerr will obviate this check and tag
 *
 * Example:
 * @code
 *   index_t i = 42;
 *   printf("%"LI"\n", i);
 * @endcode
 */

#include <inttypes.h>
typedef size_t index_t;
#if __WORDSIZE == 64
//# ifdef __USE_ISOC99 /* presumably, these macros are the same */
# if __STDC_VERSION__ >= 199901L
#  define LI "zu"
# else
  /* using an older version of C */
#  define LI "lu"
# endif
#else
# define LI "u"
#endif

/** Size of a kilobyte in bytes. */
#define KILOBYTE (((size_t)1) << 10)
/** Size of a megabyte in bytes. */
#define MEGABYTE (((size_t)1) << 20)
/** Size of a gigabyte in bytes. */
#define GIGABYTE (((size_t)1) << 30)
/* Add more of these as necessary. */



/* Tools for FASTlib stderr messages, warnings, and errors. */

/** Whether to segfault instead of calling C's abort(). */
extern int segfault_on_abort;
/** Whether to treat nonfatal warnings as fatal. */
extern int abort_on_nonfatal;
/** Whether to wait for user input after nonfatal warnings. */
extern int pause_on_nonfatal;
/** Whether to print call locations for notifications. */
extern int print_notify_locs;

/** Different types of messages FASTlib prints to stderr. */
typedef enum {
  /** Message for an unrecoverable error. */
  FL_MSG_FATAL = 0,
  /** Message for a potentially recoverable warning. */
  FL_MSG_NONFATAL = 1,
  /** Message for a significant but non-problematic event. */
  FL_MSG_NOTIFY_STAR = 2,
  /** Message for a standard, non-problematic event. */
  FL_MSG_NOTIFY = 3
} fl_msg_t;

/** Default markers for FASTlib messages. */
extern char fl_msg_marker[];
/** Default colors for FASTlib message markers. */
extern const char *fl_msg_color[];

/**
 * Terminates with an error, flushing all streams.
 *
 * Behavior adjustable with segfault_on_abort, which may be handy for
 * valgrind or other debuggers.
 */
__attribute__((noreturn)) void fl_abort(void);

/**
 * Waits for the user to press return.
 *
 * This function will obliterate anything in the stdin buffer.
 */
void fl_pause(void);

/** Prints a colored message header. */
void fl_print_msg_header(char marker, const char *color);

/** Print a location in code. */
void fl_print_msg_loc(const char *file, const char *func, const int line);

/** Implementation for FATAL. */
__attribute__((noreturn, format(printf, 4, 5)))
void fl_print_fatal_msg(const char *file, const char *func, const int line,
                        const char* format, ...);

/** Implementation for NONFATAL, NOTIFY_STAR, and NOTIFY. */
__attribute__((format(printf, 5, 6)))
void fl_print_msg(const char *file, const char *func, const int line,
                  fl_msg_t msg_type, const char* format, ...);

/**
 * Aborts, printing call location and a message to stderr.
 *
 * @param msg_params format string and variables, as in printf
 */
#define FATAL(msg_params...) (fl_print_fatal_msg( \
    __FILE__, __FUNCTION__, __LINE__, msg_params))

/**
 * (Possibly) aborts or pauses, printing call location and a message
 * to stderr.
 *
 * @param msg_params format string and variables, as in printf
 */
#define NONFATAL(msg_params...) (fl_print_msg( \
    __FILE__, __FUNCTION__, __LINE__, FL_MSG_NONFATAL, msg_params))

/**
 * Prints (possibly) call location and a message with special marker
 * to stderr.
 *
 * @param msg_params format string and variables, as in printf
 */
#define NOTIFY_STAR(msg_params...) (fl_print_msg( \
    __FILE__, __FUNCTION__, __LINE__, FL_MSG_NOTIFY_STAR, msg_params))

/**
 * Prints (possibly) call location and a message with standard marker
 * to stderr.
 *
 * @param msg_params format string and variables, as in printf
 */
#define NOTIFY(msg_params...) (fl_print_msg( \
    __FILE__, __FUNCTION__, __LINE__, FL_MSG_NOTIFY, msg_params))

/**
 * Prints a progress bar using ANSI commands to stay in place.
 *
 * For proper formatting, desc should be 22 characters or fewer and
 * the program should avoid output of other text while emitting a
 * progress bar.  When finished with a progress bar, emit a newline
 * ('\n') to keep the bar on screen or a line of spaces followed by a
 * carriage return ('\r') to clear it.
 *
 * @param name printed next to the progress bar; max 22 characters
 * @param perc percentage of bar filled; should range from 0 to 100
 */
void fl_print_progress(const char *name, int perc);



/**
 * Writes a string to a stream, converting non-alphanumeric characters
 * other than those found in ok_char to format '%XX', where XX is the
 * hexadecimal ASCII value.
 *
 * @param stream an output stream
 * @param src the string to be written, hexed
 * @param ok_char characters not converted to '%XX'
 */
void hex_to_stream(FILE *stream, const char *src, const char *ok_char);

/**
 * Writes a source string to a given destination, converting
 * non-alphanumeric characters other than those found in ok_char to
 * format '%XX', where XX is the hexadecimal ASCII value.
 *
 * The destination is assumed large enough to store the hexed string,
 * which may at most tripple in size.  The destination should not
 * overlap with the source.
 *
 * @param dest a memory location to receive the copy
 * @param src the string to be copied, hexed
 * @param ok_char characters not converted to '%XX'
 * @returns a pointer to the null-character terminating dest
 */
char *hex_to_string(char *dest, const char *src, const char *ok_char);

/**
 * Replaces substrings '%XX' with the ASCII character represented by
 * hexadecimal XX.  Percent signs with non-hexadecimal trailing
 * characters are unchanged.
 *
 * This function may be paired with fgets or other stream input to
 * invert the hex_to_stream operation.
 *
 * @param str the string to be modified
 * @returns a pointer to the null-character terminating str
 */
char *unhex_in_place(char *str);



/* Tools for expressing success or failure of FASTlib functions. */

/**
 * Type for indicating success or failure.
 *
 * Return these rather than ints to indicate your functions' results;
 * ints are interchangeably interpreted with either zero or nonzero
 * for success, but values of this type have fixed meaning.
 *
 * You may extend the meaning of this type in your code by returning
 * integer values other than SUCCESS_FAIL or SUCCESS_PASS, but ensure
 * that failure values are in the range SUCCESS_FAIL-[0,31], success
 * values are in the range SUCCESS_PASS+[0,31], and warning values are
 * in the range SUCCESS_WARN+[-16,15].
 *
 * You may combine error codes with bit-wise & and |.  These result in
 * success values equal in rank to the lesser or greater of the two
 * inputs, respectively.  Extended information (recorded in the least
 * significant bits) will likely be lost, but PASSED and FAILED checks
 * will work as perscribed.
 */
typedef enum {
  /** Upper-bound value indicating failed operation. */
  SUCCESS_FAIL = 31,
  /** A generic warning value. */
  SUCCESS_WARN = 48,
  /** Lower-bound value indicating successful operation. */
  SUCCESS_PASS = 96
} success_t;

/**
 * True on SUCCESS_PASS or greater; false otherwise.
 *
 * Distinct from !FAILED(x).  Optimized for the passing case.
 */
#define PASSED(x) (likely((x) >= SUCCESS_PASS))

/**
 * True on SUCCESS_FAIL or less; false otherwise.
 *
 * Distinct from !PASSED(x).  Optimized for the non-failing case.
 */
#define FAILED(x) (unlikely((x) <= SUCCESS_FAIL))

/**
 * Asserts that an operation passes; otherwise, aborts with a given
 * message.
 *
 * This optimized check occurs regardless of debug mode.
 */
#define MUST_PASS_MSG(x, msg_params...) \
    (likely(x >= SUCCESS_PASS) ? NOP : FATAL(msg_params))

/**
 * Asserts that an operation passes; otherwise, aborts with a standard
 * message.
 *
 * This optimized check occurs regardless of debug mode.
 */
#define MUST_PASS(x) \
    MUST_PASS_MSG(x, "MUST_PASS failed: %s", #x)

/**
 * Asserts that an operation does not fail; otherwise, aborts with a
 * given message.
 *
 * This optimized check occurs regardless of debug mode.
 */
#define MUST_NOT_FAIL_MSG(x, msg_params...) \
    (likely(x > SUCCESS_FAIL) ? NOP : FATAL(msg_params))

/**
 * Asserts that an operation does not fail; otherwise, aborts with a
 * standard message.
 *
 * This optimized check occurs regardless of debug mode.
 */
#define MUST_NOT_FAIL(x) \
    MUST_NOT_FAIL_MSG(x, "MUST_NOT_FAIL failed: %s", #x)

/** Converts C library non-negative success into a success_t. */
#define SUCCESS_FROM_C(x) (unlikely((x) < 0) ? SUCCESS_FAIL : SUCCESS_PASS)

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* BASE_COMMON_H */
