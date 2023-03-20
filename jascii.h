/*
 * Copyright (c) 2000 2001 2002 ... 2023 2024
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

#ifndef JLIB2_CHAR_NULL
#define NO_JLIB 1 /* fuure use */
#define JLIB2_CHAR_NULL  ((char) '\0')
#ifdef _MSDOS
#define SSIZE_T unsigned long int
#else
#define SSIZE_T ssize_t
#endif
#endif /* JLIB2_CHAR_NULL */

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
  int ok_to_close;
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

#ifdef NO_JLIB
#define ARG_ADDNL         'n'  /* add new line to output             */
#define ARG_CHAR          'c'  /* Replace Character                  */
#define ARG_DELM          'd'  /* Field Delimiter                    */
#define ARG_ERR           'e'  /* Output Error File                  */
#define ARG_FORCE         'f'  /* force create files                 */
#define ARG_HELP       'h'  /* Show Help                      */
#define ARG_KEEP          'k'  /* keep value                         */
#define ARG_LETTERS       'L'  /* show only ASCII letters            */
#define ARG_MAX_LINES     'm'  /* stop search after num matches      */
#define ARG_ONLY_7BIT     'l'  /* show only ASCII 7bit characters    */
#define ARG_OUT           'o'  /* Output File                        */
#define ARG_SHOW_NONASCII 'N'  /* Only show lines with non-ASCII char*/
#define ARG_STATS_ONLY    'S'  /* Print only stats                   */
#define ARG_VERBOSE       'v'  /* Verbose                            */
#define ARG_VERSION       'V'  /* Show Version Information           */
#define FILE_NAME_STDIN  "-"
#define FILE_NAME_STDOUT "-"
#define LIT_INFO_01  "\tBuild: %s %s\n"
#define LIT_REV           "Revision"
#define MSG_ERR_E000    "Try '%s %c%c' for more information\n"
#define MSG_ERR_E001    "ERROR E001: Cannot open '%s' for read, file processing bypassed\n"
#define MSG_ERR_E002    "ERROR E002: Cannot open '%s' for write, processing aborted\n"
#define MSG_ERR_E006    "ERROR E006: '%s' is an invalid value for %c%c\n"
#define MSG_ERR_E023    "ERROR E023: Cannot allocate memory when processing file %s, file bypassed\n"
#define MSG_ERR_E025    "ERROR E025: File %s cannot be created, already exists\n"
#define MSG_ERR_E049    "ERROR E049: delimiter '%s' invalid\n"
#define MSG_ERR_E080    "ERROR E080: cannot allocate initial memory : %s\n"
#define SWITCH_CHAR   '-'
#define USG_MSG_ARG_ADDNL                "\t%c%c x\t\t: add a new line after display of 'x' bytes\n"
#define USG_MSG_ARG_CHAR_1A              "\t%c%c x\t\t: replace non-ASCII with character 'x' instead\n"
#define USG_MSG_ARG_CHAR_1B              "\t\t\t  of the default character '%c' on output\n"
#define USG_MSG_ARG_DELM                 "\t%c%c c\t\t: Field delimiter, the actual character OR\n"
#define USG_MSG_ARG_DELM_A               "\t\t\t  the decimal # representing the character.\n"
#define USG_MSG_ARG_DELM_B               "\t\t\t  Ex: 7 = '^G', 9 = TAB, 124 = '|' or just '|'.\n"
#define USG_MSG_ARG_DELM_V               "\t\t\t  Will be treated as a 7-bit ASCII character.\n"
#define USG_MSG_ARG_ERR                  "\t%c%c file\t\t: Write errors to file 'file', default stderr\n"
#define USG_MSG_ARG_FORCE                "\t%c%c\t\t: force create of files when found\n"
#define USG_MSG_ARG_HELP                 "\t%c%c\t\t: Show brief help and exit\n"
#define USG_MSG_ARG_KEEP_1               "\t%c%c\t\t: do NOT replace non-ASCII characters\n"
#define USG_MSG_ARG_LETTERS              "\t%c%c\t\t: Show only ASCII letters/numbers\n"
#define USG_MSG_ARG_MAX_LINES            "\t%c%c n\t\t: Stop display after showing 'n' lines\n"
#define USG_MSG_ARG_ONLY_7BIT            "\t%c%c\t\t: Show only ASCII letters/numbers/special\n"
#define USG_MSG_ARG_OUT                  "\t%c%c file\t\t: Write output to file 'file', default stdout\n"
#define USG_MSG_ARG_SHOW_NONASCII        "\t%c%c\t\t: Show only non-ASCII Lines\n"
#define USG_MSG_ARG_STATS_ONLY           "\t%c%c\t\t: Only print File Stats\n"
#define USG_MSG_ARG_VERBOSE_8            "\t%c%c\t\t: verbose level, each time specified level increases\n"
#define USG_MSG_ARG_VERSION              "\t%c%c\t\t: Show revision information and exit\n"
#define USG_MSG_OPTIONS                  "Options\n"
#define USG_MSG_USAGE                    "usage:\t%s [OPTIONS] [FILES ...]\n"
#define LIT_ULINE_07      "-------"
#define LIT_ULINE_08      "--------"
#define LIT_STDIN         "(standard input)"
#define LIT_C80           "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
#endif /* NO_JLIB */

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

#ifdef NO_JLIB
int j2_f_exist(char *file_name);
SSIZE_T j2_getline(char **buf, size_t *n, FILE *fp);
char *j2_get_prgname(char *argv_0, char *default_name);
int j2_is_numr(char *s);
long int j2_clr_str(char *s, char c, int size);
#endif

#endif /*  JASCII_H  */
