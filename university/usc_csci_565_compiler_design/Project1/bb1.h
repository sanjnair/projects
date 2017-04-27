/*  Basic blocks + Control flow graph */

#if !defined __BB1_H__
#define __BB1_H__

#include <suif_copyright.h>

#include <stdio.h>
#include <simple.h>
#include <assert.h>
#include "print.h"

struct basic_block
{
  int num;
  simple_instr *first, *last;
  simple_sym *label;
  int is_label;                        /* Start with a true label? */
  int is_jump;                         /* End with a jump or jumps? */
  int is_cond;                         /* Is the branch conditional? */
  int link_num;
  struct basic_block **links;
  simple_sym **target_labels;
  struct basic_block *next;
};

struct list_of_links
{
  struct basic_block* data;
  struct list_of_links *next;
};

#endif //__BB1_H__
