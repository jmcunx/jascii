/*
 * Copyright (c) 2000 2001 2002 ... 2020 2021
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
 * jascii_i.c -- Init / free Routines
 */

#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _AIX
#include <getopt.h>
#endif

#include <errno.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jascii.h"

#define SCKARG 80

/*** globals ***/
char *jascii_i_c="$Id: jascii_i.c,v 4.11 2021/07/06 14:12:45 jmccue Exp $";

/*** prototype ***/
int init_get_delm(FILE *, char *, char *);

/*
 * raw_free() -- free memory
 */
void raw_free(struct s_raw **raw)

{

  if ((*raw) == NULL_S_RAW)
    return;

  if ((*raw)->next != NULL_S_RAW)
    raw_free(&((*raw)->next));
  
  if ((*raw)->rec != (char *) NULL)
    {
      free((*raw)->rec);
      (*raw)->rec = (char *) NULL;
    }
  free((*raw));
  (*raw) = NULL_S_RAW;

} /* raw_free() */

/*
 * file_info_init() -- Initialize work info
 */
void file_info_init(struct s_file_info *f)

{
  f->fp    = (FILE *) NULL;
  f->fname = (char *) NULL;

} /* file_info_init() */

/*
 * init_work() -- Initialize work info
 */
void work_init(struct s_work *w, char *arg0)

{

  file_info_init(&(w->out));
  file_info_init(&(w->err));

  w->out.fp = stdout;
  w->err.fp = stderr;

  w->pname            = j2_get_prgname(arg0, PNAME);
  w->delm             = JLIB2_CHAR_NULL;
  w->max_recs         = 100000L;
  w->new_line         = 0;
  w->verbose          = 0;
  w->force            = (int) FALSE;
  w->only_stats       = (int) FALSE;
  w->only_non_ascii   = (int) FALSE;
  w->only_letters     = (int) FALSE;
  w->only_7bit        = (int) FALSE;
  w->all_chars        = (int) FALSE;
  w->replace_char     = BAD_CHAR;

} /* init_work() */

/*
 * raw_init() -- Initialize rec info
 */
void raw_init(struct s_raw *r)

{

  r->all_ascii        = (int) TRUE;
  r->rec              = (char *) NULL;
  r->next             = NULL_S_RAW;

  counts_init(&r->amt);

} /* raw_init() */

/*
 * counts_init() -- Initialize rec info
 */
void counts_init(struct s_counts *r)

{

  r->rec_num          = (size_t) 0;;
  r->count_nulls      = (size_t) 0;;
  r->count_no_ascii   = (size_t) 0;;
  r->count_ascii      = (size_t) 0;;
  r->count_ctrl       = (size_t) 0;;
  r->count_eor        = (size_t) 0;;
  r->count_utfb       = (size_t) 0;;
  r->total_chars      = (size_t) 0;;
  r->count_utfc       = (size_t) 0;;

} /* counts_int() */


/*
 * process_arg() -- process arguments
 */
void process_arg(int argc, char **argv, struct s_work *w)

{
  char ckarg[SCKARG];
  int opt;
  int ok;
  char *fout = (char *) NULL; 
  char *ferr = (char *) NULL; 
  
  snprintf(ckarg, SCKARG, "%c%c:%c:%c:%c%c%c%c%c:%c:%c:%c%c%c%c", 
	  ARG_CHAR,  ARG_DELM, ARG_ERR,     ARG_HELP, 
          ARG_FORCE, ARG_KEEP, ARG_LETTERS, ARG_ONLY_7BIT, ARG_MAX_LINES,
          ARG_ADDNL, ARG_OUT,  ARG_VERBOSE, ARG_VERSION,
	  ARG_STATS_ONLY, ARG_SHOW_NONASCII);
  
  while ((opt = getopt(argc, argv, ckarg)) != -1)
    {
      switch (opt)
	{
	case ARG_CHAR:
	    if (strlen(optarg) == 1)
	      w->replace_char = (*optarg);
	    else
	      {
		fprintf(w->err.fp, MSG_ERR_E006, optarg, SWITCH_CHAR, ARG_CHAR);
		fprintf(w->err.fp, MSG_ERR_E000, w->pname, SWITCH_CHAR, ARG_HELP);
		exit(EXIT_FAILURE);
	      }
	  break;
	case ARG_DELM:
	  w->delm  = (char) init_get_delm(w->err.fp, optarg, w->pname);
	  break;
	case ARG_ERR:
	  ferr = optarg;
	  break;
	case ARG_HELP:
	  show_brief_help(w->err.fp, w->pname);
	  break;
	case ARG_FORCE:
	  w->force = (int) TRUE;
	  break;
	case ARG_KEEP:
	  w->all_chars = (int) TRUE;
	  break;
	case ARG_LETTERS:
	  w->only_letters = (int) TRUE;
	  break;
	case ARG_ONLY_7BIT:
	  w->only_7bit    = (int) TRUE;
	  break;
	case ARG_MAX_LINES:
	  w->max_recs = atol(optarg);
	  break;
	case ARG_ADDNL:
	  w->new_line = atoi(optarg);
	  break;
	case ARG_VERBOSE:
	  w->verbose++;
	  break;
	case ARG_VERSION:
	  show_rev(w->err.fp, w->pname);
	  break;	  
	case ARG_OUT:
	  fout = optarg;
	  break;
	case ARG_STATS_ONLY:
	  w->only_stats = (int) TRUE;
	  break;
	case ARG_SHOW_NONASCII:
	  w->only_non_ascii = (int) TRUE;
	  break;
	default:
	  fprintf(w->err.fp, MSG_ERR_E000, w->pname, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	  break;
	}
    }
  
  /*** if necessary - save stdout/err files and open ***/
  ok = open_out(stderr, &(w->err), ferr, w->force);
  if (ok == EXIT_SUCCESS)
    ok = open_out(w->err.fp, &(w->out), fout, w->force);
  if (ok != EXIT_SUCCESS)
    {
      fprintf(w->err.fp, MSG_ERR_E000, w->pname, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  /*** override some args if others are sey ***/
  if (w->only_stats == (int) TRUE)
    {
      if (w->verbose < 2)
	w->verbose = 2;
    }
  
} /* process_arg() */

/*
 * open_out() -- save the file anem and check status
 */
int open_out(FILE *wfp, struct s_file_info *f, char *fname, int force)

{
  int errsave;
  
  if (fname == (char *) NULL)
    return(EXIT_SUCCESS);
  if (strcmp(fname, FILE_NAME_STDOUT) == 0)
    return(EXIT_SUCCESS);

  if (force == (int) FALSE)
    {
      if ( j2_f_exist(fname) )
	{
	  fprintf(wfp, MSG_ERR_E025, fname);
	  return(EXIT_FAILURE);
	}
    }
  
  f->fp = fopen(fname, "w");
  errsave = errno;
  if (f->fp == (FILE *) NULL)
    {
      f->fp = stderr;  /* needs to be something */
      fprintf(wfp, MSG_ERR_E002, fname);
      fprintf(wfp, "\t%s\n", strerror(errsave));
      return(EXIT_FAILURE);
    }
  
  /*** success, save file name ***/
  f->fname = strdup(fname);
  return(EXIT_SUCCESS);
  
} /* open_out() */

/*
 * init_get_delm() -- translate a string into a delimiter
 */
int init_get_delm(FILE *fp, char *s, char *prog_name)
  
{
  int d;
  int c = JLIB2_CHAR_NULL;
  
  if (s == (char *) NULL)
    return(c);
  
  if (strlen(s) == 1)
    {
      if ( ((*s) > 47)  && ((*s) < 58) ) /* 0 -- 9 */
	c = (*s) - 48;
      else
	c = (*s);
    }
  else
    {
      if (j2_is_numr(s) == (int) TRUE)
	{
	  d = atoi(s);
	  if ((d > -1) && (d < 128))
	    c = (char) d;
	  else
	    {
	      fprintf(fp, MSG_ERR_E049, s);
	      fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	}
      else
	{
	  fprintf(fp, MSG_ERR_E049, s);
	  fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }
  
  return(c);
  
} /* init_get_delm() */

/* END: jascii_i.c */
