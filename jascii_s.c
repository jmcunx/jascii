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
 * show data module
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jascii.h"

#define SFMT 20

/*
 * show_fmt_c() -- write a formatted character
 */
char *show_fmt_c(char c)

{

  static char fmt[SFMT];

  memset(fmt, '\0', SFMT);

  if ((c > 31) && (c < 127))
    snprintf(fmt, SFMT, "%c", c);
  else
    snprintf(fmt, SFMT, "0x%02X", c);

  return(fmt);

} /* show_fmt_c() */

/*
 * show_args() -- write agruments
 */
void show_args(struct s_work *w)

{

  fprintf(w->out.fp, "Arguments:\n");

  fprintf(w->out.fp, "    %-30s : %s\n", "non-ASCII caracter Replace",
	 show_fmt_c(w->replace_char));
  fprintf(w->out.fp, "    %-30s : %s\n", "Force Create Files",
	 (w->force ? "yes" : "no"));

  if (w->delm != CHAR_TYPE_NULL)
    fprintf(w->out.fp, "    %-30s : %s\n", "Field Delimiter", show_fmt_c(w->delm));
  if (w->max_recs > 0)
    fprintf(w->out.fp, "    %-30s : %ld\n", "Maximum newlines to Process",
	   w->max_recs);
  if (w->new_line > 0)
    fprintf(w->out.fp, "    %-30s : %d\n", "Maximum Print Line Size", w->new_line);

  fprintf(w->out.fp, "    %-30s : %s\n", "What to Print",
	 (w->only_stats ? "File Stats Only" : "Detail Data and File Stats"));

  if (w->only_stats == FALSE)
    {
      if (w->only_7bit == TRUE)
	fprintf(w->out.fp, "    %-30s : %s\n", "Detail Data to Print",
		"Only Letters/Numbers/Special");
      else
	fprintf(w->out.fp, "    %-30s : %s\n", "Detail Data to Print",
		(w->only_letters ? "Only Letters/Numbers" : "All Detail Data"));
      if (w->only_non_ascii)
	fprintf(w->out.fp, "    %-30s : %s\n", " ",
	       "Only Lines with 1 or more non-ASCII chars");
    }

  fprintf(w->out.fp, "\n");

} /* show_args() */

/*
 * show_file_hdr() -- Write File Heading
 */
void show_file_hdr(FILE *fp, char *fname)

{

  fprintf(fp, "%s\n", LIT_C80);

  if (fname == (char *) NULL)
    fprintf(fp, "%s\n", LIT_STDIN);
  else
    {
      if (strcmp(fname, FILE_NAME_STDIN) == 0)
	fprintf(fp, "%s\n", LIT_STDIN);
      else
	fprintf(fp, "%s\n", fname);
    }

  fprintf(fp, "%s\n", LIT_C80);

} /* show_file_hdr() */

/*
 * cnv_char() -- convert the character
 */
char cnv_char(int *only_letter, struct s_work *w, char c)

{
  int char_type;
  char newc = c;

  char_type = get_char_type(c, w->delm);
  (*only_letter) = FALSE;

  switch (char_type)
    {
    case CHAR_TYPE_NULL:
      newc = w->replace_char;
      break;
    case CHAR_TYPE_ASCII:
      if ((c > 47) && (c < 58)) /* 0 -- 9 */
	(*only_letter) = TRUE;
      else
	{
	  if ((c > 64) && (c < 91)) /* A -- Z */
	    (*only_letter) = TRUE;
	  else
	    {
	      if ((c > 96) && (c < 123)) /* a -- z */
		(*only_letter) = TRUE;
	    }
	}
      if (((*only_letter) == FALSE) && (w->only_7bit == TRUE))
	{
	  if ((c > 32) && (c < 127)) /* ! -- ~ */
	      (*only_letter) = TRUE;
	}
      break;
    case CHAR_TYPE_CTRL:
      if (w->all_chars == FALSE)
	newc = w->replace_char;
      break;
    case CHAR_TYPE_NLINE:
      if (w->new_line > 0)
	newc = w->replace_char;
      break;
    case CHAR_TYPE_NOT_ASCII:
    default:
      if (w->all_chars == FALSE)
	newc = w->replace_char;
      break;
    }

  return(newc);

} /* cnv_char() */

/*
 * ok_to_print() -- Print the entry ?
 */
int ok_to_print(struct s_work *w, struct s_raw *raw, int is_letter, char c)

{

  if ((w->only_letters == TRUE) || (w->only_7bit == TRUE))
    {
      if (is_letter == TRUE)
	return(TRUE);
      else
	return(FALSE);
    }

  if (w->only_non_ascii)
    {
      if (raw->all_ascii == TRUE)
	return(FALSE);
      else
	return(TRUE);
    }

  return(TRUE);

} /* ok_to_print() */

/*
 * show_line() -- Write a data line
 */
void show_line(struct s_work *w, struct s_raw *raw, size_t *cprt)

{
  size_t i;
  char c;
  int is_letter;

  for (i = 0; i < raw->amt.total_chars; i++)
    {
      c = cnv_char(&is_letter, w, raw->rec[i]);
      if ( ! ok_to_print(w, raw, is_letter, c))
	continue;
      fprintf(w->out.fp, "%c", c);
      (*cprt)++;
      if ((w->new_line > 0) && ((*cprt) >= w->new_line))
	{
	  (*cprt) = 0;
	  fprintf(w->out.fp, "\n");
	}
    }

} /* show_line() */

/*
 * show_data() -- write data
 */
void show_data(struct s_work *w, struct s_counts *grandc, int verbose, struct s_raw *raw, char *fname)

{
  struct s_raw *now = raw;
  struct s_counts filec;
  size_t rcount = (size_t) 0;
  size_t chars_printed = (size_t) 0;
  
  counts_init(&filec);

  if (verbose > 0)
    show_file_hdr(w->out.fp, fname);

  /*** Show File Detail  ***/
  if (w->only_stats == FALSE)
    {
      fprintf(w->out.fp, "\n");
      now = raw;
      while (now != NULL_S_RAW)
	{
	  show_line(w, now, &chars_printed);
	  now = now->next;
	}
      fprintf(w->out.fp, "\n\n");
    }
  
  /*** show file summary ***/
  if (verbose > 2)
    show_count_hdr(w->out.fp, "rec_num");
  now = raw;
  while (now != NULL_S_RAW)
    {
      rcount++;
      now->amt.rec_num = rcount;
      add_summary(&filec, &now->amt);
      if (verbose > 2)
	show_count_line(w->out.fp, now->amt);
      now = now->next;
    }

  if (verbose > 1)
    show_summary_totl(w->out.fp, "Summary Totals", "Records", filec);
  add_summary(grandc, &filec);

} /* show_data() */

/*
 * show_count_hdr() -- show count headings
 */
void show_count_hdr(FILE *fp, char *title)

{

  fprintf(fp, "%8s %8s %8s %8s %7s %8s %8s %8s %8s\n",
	 "",
	 "",  
	 "",  
	 "",  
	 "",  
	 "UTF-8",
	 "",
	 "",
	 "UTF-8");
  fprintf(fp, "%8s %8s %8s %8s %7s %8s %8s %8s %8s\n",
	 title,
	 "ascii",  
	 "!ascii",  
	 "control",  
	 "NULLS",  
	 "bytes",
	 "eor",
	 "bytes",
	 "chars");

  fprintf(fp, "%8s %8s %8s %8s %8s %8s %8s %8s %8s\n",
	 LIT_ULINE_08,
	 LIT_ULINE_08,
	 LIT_ULINE_08,
	 LIT_ULINE_08,
	 LIT_ULINE_07,
	 LIT_ULINE_08,
	 LIT_ULINE_08,
	 LIT_ULINE_08,
	 LIT_ULINE_08);

} /* show_count_hdr() */

/*
 * show_summary_totl() -- write data
 */
void show_summary_totl(FILE *fp, char *title, 
                       char *rec_title, struct s_counts c)

{

  fprintf(fp, "\n%s\n", title);
  fprintf(fp, "================\n");

  show_count_hdr(fp, rec_title);
  show_count_line(fp, c);

} /* show_summary_totl() */

/*
 * show_count_line() -- write data
 */
void show_count_line(FILE *fp, struct s_counts amt)

{
  fprintf(fp, "%8ld %8ld %8ld %8ld %7ld %8ld %8ld %8ld %8ld\n",
	  (long) amt.rec_num,
	  (long) amt.count_ascii,
	  (long) amt.count_no_ascii,
	  (long) amt.count_ctrl,
	  (long) amt.count_nulls,
	  (long) amt.count_utfb,
	  (long) amt.count_eor,
	  (long) amt.total_chars,
	  (long) amt.count_utfc);

} /* show_count_line() */
