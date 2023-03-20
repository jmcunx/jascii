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
/* jascii -- non-ASCII character analysis */

#ifndef _MSDOS
#include <sys/param.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _AIX
#include <getopt.h>
#endif

#ifdef OpenBSD
#include <err.h>
#endif

#include <errno.h>
#include <ctype.h>

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jascii.h"

#ifdef _MSDOS
#define RMODE "rb"
#else
#define RMODE "r"
#endif

#ifdef DEBUG
/*
 * raw_write_debug() -- write data (for debug)
 */
void raw_write_debug(struct s_raw *raw)

{
  struct s_raw *now = raw;
  long int rec_num = 0;

  while (now != NULL_S_RAW)
    {
      rec_num++;
      fprintf(stdout, "write Rec #: %ld -- %s\n    %s\n",
	      rec_num,
	      (now->next == NULL_S_RAW ? "NEXT NULL" : "NEXT as DATA"),
	      now->rec);
      now = now->next;
    }

} /* raw_write_debug() */

#endif /* DEBUG */

/*
 * add_summary() -- add summary
 */
void add_summary(struct s_counts *t, struct s_counts *c)

{
  t->rec_num++;
  t->count_ascii     += c->count_ascii;
  t->count_no_ascii  += c->count_no_ascii;
  t->count_ctrl      += c->count_ctrl;
  t->count_nulls     += c->count_nulls;
  t->total_chars     += c->total_chars;
  t->count_utfb      += c->count_utfb;
  t->count_eor       += c->count_eor;
  t->count_utfc      += c->count_utfc;

} /* add_summary() */

/*
 * get_char_type() -- Determine Character Type
 */
int get_char_type(char c, char delm)

{

  unsigned int u;

  u = (unsigned int) c;

  if ((u > 31u) && (u < 127u))
    return(CHAR_TYPE_ASCII);

  if (u > 126u)
    return(CHAR_TYPE_NOT_ASCII);

  if ((delm != JLIB2_CHAR_NULL) && (c == delm))
    return(CHAR_TYPE_ASCII);

  if (c == JLIB2_CHAR_NULL)
    return(CHAR_TYPE_NULL);

  if ((c == '\t') || (c == '\r'))
    return(CHAR_TYPE_CTRL);

  if (c == '\n')
    return(CHAR_TYPE_NLINE);

  if (u < 32u)
    return(CHAR_TYPE_CTRL);

  return(CHAR_TYPE_NOT_ASCII);

} /* get_char_type() */

/*
* sum_char_types() -- update character type sums
*/
void sum_char_types(struct s_work *w, struct s_raw *raw, char c)

{

  char new;

  new = get_char_type(c, w->delm);

  switch (new)
    {
    case CHAR_TYPE_NULL:
      (raw->amt.count_nulls)++;
      break;
    case CHAR_TYPE_ASCII:
      (raw->amt.count_ascii)++;
      break;
    case CHAR_TYPE_CTRL:
      (raw->amt.count_ctrl)++;
      break;
    case CHAR_TYPE_NLINE:
      (raw->amt.count_eor)++;
      break;
    case CHAR_TYPE_NOT_ASCII:
      (raw->amt.count_no_ascii)++;
      break;
    default:
      (raw->amt.count_no_ascii)++;
      break;
    }

} /* sum_char_types() */

/*
 * count_utf() -- count UTF-8 characters
 */
ssize_t count_utf(char *s)
{

  size_t len = 0;
  unsigned char c0 = (unsigned char) 0;
  unsigned char c1 = (unsigned char) 0;
  unsigned char c2 = (unsigned char) 0;
  unsigned char c3 = (unsigned char) 0;

  if (s == (char *) NULL)
    return((ssize_t) 0);

  len = strlen(s);

  if (len > 3)
    {
      c0 = (unsigned char) s[0];
      c1 = (unsigned char) s[1];
      c2 = (unsigned char) s[2];
      c3 = (unsigned char) s[3];
    }
  else
    {
      switch (len)
	{
	  case 2:
	    c0 = (unsigned char) s[0];
	    c1 = (unsigned char) s[1];
	    break;
	  case 3:
	    c0 = (unsigned char) s[0];
	    c1 = (unsigned char) s[1];
	    c2 = (unsigned char) s[2];
	    break;
	  default:
	    return((ssize_t) 0);
	    break;
	}
    }

  if (c0 < (unsigned char) 192)
    return((ssize_t) 0);
  if (c0 > (unsigned char) 247)
    return((ssize_t) 0);

  /*** 4 byte UTF-8 sequence ? ***/
  if ( (c0 > (unsigned char) 239) && (c0 < (unsigned char) 248) && (len >= 4) )
    {
      if ( (c1 > (unsigned char) 127) && (c1 < (unsigned char) 192) &&
           (c2 > (unsigned char) 127) && (c2 < (unsigned char) 192) &&
           (c3 > (unsigned char) 127) && (c3 < (unsigned char) 192) )
	return((ssize_t) 4);
    }

  /*** 3 byte UTF-8 sequence ? ***/
  if ( (c0 > (unsigned char) 223) && (c0 < (unsigned char) 240) && (len >= 3) )
    {
      if ( (c1 > (unsigned char) 127) && (c1 < (unsigned char) 192) &&
           (c2 > (unsigned char) 127) && (c2 < (unsigned char) 192) )
	return((ssize_t) 3);
    }

  /*** 2 byte UTF-8 sequence ? ***/
  if ( (c0 > (unsigned char) 191) && (c0 < (unsigned char) 224) && (len >= 2) )
    {
      if ( (c1 > (unsigned char) 127) && (c1 < (unsigned char) 192) )
	return((ssize_t) 2);
    }

  return((ssize_t) 0);

} /* count_utf() */

/*
 * raw_save() -- get memory and save data
 */
struct s_raw *raw_save(char *buf, ssize_t cbytes, char *fname, struct s_work *w)

{
  struct s_raw *raw = NULL_S_RAW;
  ssize_t i, j;
  size_t b;

  raw = calloc(1, sizeof (struct s_raw));
  if (raw == NULL_S_RAW)
    {
      fprintf(w->err.fp, MSG_ERR_E080, strerror(errno));
      exit(EXIT_FAILURE);
    }

  raw_init(raw);
  
  raw->amt.total_chars  = cbytes;

  /*** save record entry ***/
  b = ((size_t) cbytes) + (size_t) 2;
  raw->rec = calloc(b, sizeof(char));
  if (raw->rec == (char *) NULL)
    {
      fprintf(w->err.fp, MSG_ERR_E080, strerror(errno));
      exit(EXIT_FAILURE);
    }

  for (i = (ssize_t) 0, j = (ssize_t) 0; i < cbytes; i++, j++)
    {
      sum_char_types(w, raw, buf[i]);
      raw->rec[i] = buf[i];
    }

  if (raw->amt.total_chars == (raw->amt.count_ascii + raw->amt.count_eor))	
    raw->all_ascii = (int) TRUE;
  else
    raw->all_ascii = (int) FALSE;

  /*** look for UTF-8 and adjust sums ***/
  if (raw->amt.count_no_ascii > (size_t) 0)
    {
      for (i = 0; i < cbytes; i++)
	{
	  j = count_utf(&(buf[i]));
	  if (j > (ssize_t) 0)
	    {
	      (raw->amt.count_no_ascii) -= j;
	      (raw->amt.count_utfb) += j;
	      (raw->amt.count_utfc)++;
	    }
	}
    }

  /*** done ***/
  return(raw);

} /* raw_save() */

/*
* raw_load() -- load text data into memory
*/
struct s_raw *raw_load(struct s_work *w, char *fname, long max_recs)

{
  size_t bsize = (size_t) 0;
  char *buf = (char *) NULL;
  ssize_t cbytes = (ssize_t) 0;
  ssize_t creads = (ssize_t) 0;
  int close_in = (int) FALSE;
  FILE *fp;
  struct s_raw *raw = NULL_S_RAW;
  struct s_raw *now = NULL_S_RAW;
  struct s_raw *new = NULL_S_RAW;

  if (fname == (char *) NULL)
    fp = stdin;
  else
    {
      if (strcmp(fname, FILE_NAME_STDIN) == 0)
	fp = stdin;
      else
	{
	  fp = fopen(fname, RMODE);
	  close_in = (int) TRUE;
	}
    }

  if (fp == (FILE *) NULL)
    {
      fprintf(w->err.fp, MSG_ERR_E001, fname);
      fprintf(w->err.fp, "\t%s\n", strerror(errno));
      return(raw);
    }

  /* allocate initial read buffer memory (optional) */
  bsize = REC_SIZE_INIT;
  buf = (char *) calloc(REC_SIZE_INIT, sizeof(char));
  if (buf == (char *) NULL)
    {
      fprintf(w->err.fp, MSG_ERR_E023, fname);
      fprintf(w->err.fp, "\t%s\n", strerror(errno));
      return(raw);
    }

  while ((cbytes = j2_getline(&buf, &bsize, fp)) > (ssize_t) -1)
    {
      creads++;
      if ((max_recs > 0L) && (creads > max_recs))
	break;
      new = raw_save(buf, cbytes, fname, w);
      if (raw == NULL_S_RAW)
	raw = new;
      else
	now->next = new;
      now = new;
      memset(buf, '\0', bsize);
    }

  if (buf != (char *) NULL)
    free(buf);

  if (close_in)
    fclose(fp);
  return(raw);

} /* raw_load() */

/*
* main()
*/
int main(int argc, char **argv)

{
  int i;
  struct s_raw *raw = NULL_S_RAW;
  struct s_counts grandc;
  struct s_work w;

#ifdef OpenBSD
  if(pledge("stdio rpath wpath cpath",NULL) == -1)
    err(1,"pledge\n");
#endif

  work_init(&w, argv[0]);
  process_arg(argc, argv, &w);
  counts_init(&grandc);

  if (w.verbose > 1)
    show_args(&w);

  /*** process all files ***/
  for (i = optind; i < argc; i++)
    {
      raw = raw_load(&w, argv[i], w.max_recs);
      if (raw != NULL_S_RAW)
	{
	  show_data(&w, &grandc, w.verbose, raw, argv[i]);
	  raw_free(&raw);
	}
    }
  if (i == optind)
    {
      raw = raw_load(&w, (char *) NULL, w.max_recs);
      if (raw != NULL_S_RAW)
	{
	  show_data(&w, &grandc, w.verbose, raw, (char *) NULL);
	  raw_free(&raw);
	}
    }

  /*** Grand Totals, if more than 1 file ***/
  if (w.verbose > 1)
    {
      if (grandc.rec_num > 1)
	{
	  show_file_hdr(w.out.fp, "Grand Totals");
	  show_summary_totl(w.out.fp, "Grand Totals", "Files", grandc);
	}
    }

  if (w.pname != (char *) NULL)
    free(w.pname);

  if (w.out.ok_to_close == TRUE)
    fclose(w.out.fp);
  if (w.out.fname != (char *) NULL)
    free(w.out.fname);
  if (w.err.ok_to_close == TRUE)
    fclose(w.err.fp);
  if (w.err.fname != (char *) NULL)
    free(w.err.fname);

  exit(EXIT_SUCCESS);

} /* main() */
