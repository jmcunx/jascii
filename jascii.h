/*
 * Copyright (c) 2000 2001 2002 ... 2021 2022
 *     John McCue <jmccue@jmcunx.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
   ASCII display utility
*/
#ifndef JASCII_H

#define JASCII_H "INCLUDED"

#define PNAME    "jascii"
#define BAD_CHAR '*'

struct s_counts
{
  size_t rec_num;        /* number of records / total recs       */
  size_t count_ascii;    /* number of 7 bit characters found     */
  size_t count_no_ascii; /* number of non 7 bit characters found */
  size_t count_ctrl;     /* number of non ctrl  characters found */
  size_t count_nulls;    /* number of NULL characters found      */
  size_t count_eor;      /* number of end of record chars  found */
  size_t count_utfb;     /* number of UTF-8 Bytes                */
  size_t total_chars;    /* total characters                     */
  size_t count_utfc;     /* number of UTF-8 Characters           */
} ;
struct s_raw
{
  int all_ascii;          /* TRUE all 7 bit ascii                 */
  struct s_counts amt;
  char *rec;
  struct s_raw *next;
} ;

struct s_file_info
{
  FILE *fp;
  char *fname;
} ;

struct s_work
{
  struct s_file_info out;         /* default stdout                         */
  struct s_file_info err;         /* default stderr                         */
  char *pname;                    /* real program name                      */
  char delm;                      /* delimiter, treat as ASCII              */
  long max_recs;                  /* process max_recs records               */
  char replace_char;              /* non-ASCII replace char                 */
  int  verbose;                   /* numeric verbose level                  */
  int  force;                     /* T/F - Force Create Files               */
  int  only_stats;                /* T/F - do NOT print detail              */
  int  all_chars;                 /* T/F - do NOT change non-ascii chars    */
  int  only_7bit;                 /* T/F - print letters/numbers/others     */
  int  only_letters;              /* T/F - print only letters/numbers       */
  int  new_line;                  /* print \n after # chars                 */
  int  only_non_ascii;            /* T/F - only print non-ascii lines       */
} ;

#define NULL_S_RAW ((struct s_raw *) NULL)
#define REC_SIZE_INIT 500

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/*** character types ***/
#define CHAR_TYPE_NULL       0
#define CHAR_TYPE_ASCII      1
#define CHAR_TYPE_CTRL       2
#define CHAR_TYPE_NLINE      3
#define CHAR_TYPE_NOT_ASCII  4

/*** prototypes ***/
int  get_char_type(char, char);
void show_file_hdr(FILE *, char *);
char *show_fmt_c(char);
void add_summary(struct s_counts *, struct s_counts *);
void show_data(struct s_work *, struct s_counts *, int, struct s_raw *, char *);
void show_summary_totl(FILE *, char *, char *, struct s_counts);
void show_count_hdr(FILE *, char *);
void show_count_line(FILE *, struct s_counts);
void show_args(struct s_work *);
void raw_free(struct s_raw **);
void raw_init(struct s_raw *);
void work_init(struct s_work *, char *);
void counts_init(struct s_counts *);
void process_arg(int, char **, struct s_work *);
void show_rev(FILE *, char *);
void show_brief_help(FILE *, char *);
int  open_out(FILE *, struct s_file_info *, char *, int);

#endif /*  JASCII_H  */
