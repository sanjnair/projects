/*  Basic blocks + Control flow graph */

#include "bb1.h"
#include "prj1.h"

struct basic_block *list, *tail;
struct list_of_links *work_list = NULL;

void new_block( simple_instr* first, simple_instr* last )
{
  int i;

  if( list == NULL )
    tail = list = (struct basic_block *)malloc(sizeof(struct basic_block));
  else
    {
      tail->next = (struct basic_block *)malloc(sizeof(struct basic_block));
      tail = tail->next;
    }

  tail->first = first;
  tail->last = last;
  if( first->opcode == LABEL_OP )
    tail->label = first->u.label.lab;
  else
    tail->label = NULL;
  tail->is_label = 0;  /* not yet decided whether it is a true label */
  tail->is_jump = 1;   /* assert it is a jump */
  tail->next = NULL;

  switch (last->opcode) {

	case BTRUE_OP:
	case BFALSE_OP: {
	  tail->is_cond = 1;
	  tail->link_num = 2;
	  tail->target_labels = (simple_sym **)malloc(sizeof(simple_sym *) * tail->link_num);
	  tail->target_labels[0] = last->u.bj.target;
	  tail->target_labels[1] = NULL;
	  break;
	}

	case MBR_OP: {
	  tail->is_cond = 0;
	  tail->link_num = last->u.mbr.ntargets + 1;
	  tail->target_labels = (simple_sym **)malloc(sizeof(simple_sym *) * tail->link_num);
	  for( i = 0; i < tail->link_num - 1; i++ )
	    tail->target_labels[i] = last->u.mbr.targets[i];
	  tail->target_labels[i] = last->u.mbr.deflab;
	  break;
	}

	case RET_OP: {
	  tail->is_cond = 0;
	  tail->link_num = 1;
	  tail->target_labels = (simple_sym **)malloc(sizeof(simple_sym *) * tail->link_num);
	  tail->target_labels[0] = NULL;
	  break;
	}

	case JMP_OP: {
	  tail->is_cond = 0;
	  tail->link_num = 1;
	  tail->target_labels = (simple_sym **)malloc(sizeof(simple_sym *) * tail->link_num);
	  tail->target_labels[0] = last->u.bj.target;
	  break;
	}

        default: {
	  tail->is_jump = 0;
	  tail->link_num = 1;
	  tail->target_labels = (simple_sym **)malloc(sizeof(simple_sym *) * tail->link_num);
	  tail->target_labels[0] = NULL;
	  break;
	}
  }

  tail->links = NULL;
}

void add_to_work_list( struct basic_block* data )
{
  struct list_of_links *p = (struct list_of_links *)malloc(sizeof(struct list_of_links));
  p->data = data;
  p->next = work_list;
  work_list = p;
}

int work_list_is_empty()
{
  return (work_list == NULL);
}

struct basic_block* get_from_work_list()
{
  struct basic_block* p = work_list->data;
  struct list_of_links *q = work_list;
  work_list = q->next;
  free(q);
  return p;
}

/*  Read through the list of instructions and print them out.  */

simple_instr *
do_procedure (simple_instr *inlist, char *proc_name)
{
  simple_instr *s, *first, *last;
  struct basic_block *p, *q, *r;
  int prev_num;
  int i, j, k;
  FILE *fd = stdout;
  int last_jump;
  int instrNum=0;

  list = tail = NULL;
    
  printf( "========================================================================\n");
  printf("\n \t\t\tProject 1\n");
  printf( "\n========================================================================");
   
  printf("\nPROCEDURE %s:\n", proc_name);

  first = s = inlist;
  last = NULL;
  k = 1;
  last_jump = 1;

  while (s) {

    if( s->opcode == LABEL_OP && last != NULL && !last_jump )
      {
	new_block(first, last);
	first = s;
      }

    /* whether s can be the last instruction of a block */
    if( s->opcode == BTRUE_OP || 
	s->opcode == BFALSE_OP || 
	s->opcode == MBR_OP || 
	s->opcode == RET_OP || 
	s->opcode == JMP_OP ) 
      {
	new_block(first, s);
	first = s->next;
	last_jump = 1;
      }
    else
      last_jump = 0;

    last = s;
    s = s->next;
  }

  if(first != NULL)
    new_block(first, last); 

  /* add prologue block and epilogue block */
  p = (struct basic_block *)malloc(sizeof(struct basic_block));
  p->first = p->last = NULL;
  p->is_label = p->is_jump = 0;
  p->link_num = 1;
  p->target_labels = (simple_sym **)malloc(sizeof(simple_sym *) * tail->link_num);
  p->target_labels[0] = NULL;
  p->links = NULL;
  p->next = list;
  list = p;
  p = (struct basic_block *)malloc(sizeof(struct basic_block));
  p->first = p->last = NULL;
  p->is_label = p->is_jump = 0;
  p->link_num = 0;
  p->target_labels = NULL;
  p->links = NULL;
  p->next = NULL;
  tail->next = p;
  tail = p;

  /* match up links */
  for( p = list; p->next; p = p->next )
    {
      assert( p->link_num > 0 );
      p->links = (struct basic_block **)malloc(sizeof(struct basic_block *) * p->link_num);
      for( i = 0; i < p->link_num; i++ )
	{
	  if( p->target_labels[i] != NULL )
	    {
	      for( q = list; q->next; q = q->next )
		if( q->label == p->target_labels[i] )
		  break;
	      assert(q->next != NULL);
	      p->links[i] = q;
	    }
	  else
	    {
	      if( !p->is_jump || p->is_cond ) // then there should be a link to the next block
		{
		  p->links[i] = p->next;
		}
	      else
		{
		  assert(p->last->opcode == RET_OP); // it must be a return
		  p->links[i] = tail;
		}
	    }
	}
    }

  /* mark reachable blocks */
  for( p = list; p; p = p->next )
    p->num = -1; 

  work_list = NULL;
  add_to_work_list( list );
  while( ! work_list_is_empty() )
    {
      p = get_from_work_list();
      p->num = 0;
      for( i = 0; i < p->link_num; i++ )
	{
	  assert(p->links[i] != NULL);
	  if( p->links[i]->num < 0 )
	    add_to_work_list(p->links[i]);
	}
    }

  assert(list->num == 0);
  assert(tail->num == 0);
  
  /* delete unreachable blocks */
  for( p = list->next, q = list; p->next; q = p, p = p->next )
    if( p->num < 0 )
      {
	assert( p->link_num > 0 );
	assert( p->target_labels != NULL );
	assert( p->links != NULL );
	free( p->target_labels );
	free( p->links );
	q->next = p->next;
	p = q;
      }

  /* mark unnecessary blocks */
  list->num = 1;
  list->next->num = 1;
  for( p = list; p->next; p = p->next )
    {
      if( p->is_jump )
	{
	  p->next->num = 1;
	  for( i = 0; i < p->link_num; i++ )
	    {
	      p->links[i]->num = 1;
	      p->links[i]->is_label = 1;
	    }
	}
    }

  assert(tail->num == 1);

  /* merge unnecessary blocks */
  for( p = list->next, q = list; p->next; q = p, p = p->next )
    if( p->num == 0 )
      {
	assert( q->link_num == 1 );
	assert( q->links[0] == p );
	assert( q->target_labels != NULL );
	free( q->target_labels );
	assert( q->links != NULL );
	free( q->links );

	q->target_labels = p->target_labels;
	q->links = p->links;
	q->link_num = p->link_num;
	q->last = p->last;
	q->is_jump = p->is_jump;
	q->is_cond = p->is_cond;
	q->next = p->next;

	p = q;
      }

  /* shrink links */
  for( p = list; p; p = p->next )
    {
      for( k = i = 0; i < p->link_num; i++ )
	{
	  for( j = 0; j < k; j++ )
	    if( p->links[i] == p->links[j] )
	      break;
	  if( j >= k )
	    p->links[k++] = p->links[i];
	}
      p->link_num = k;
    }

  /* number the blocks */
  for( p = list, k = 0, j = 0; p; p = p->next, k++ )
    {
      assert(p->num == 1);
      p->num = k;
      j += p->link_num;
    }

  printf ("\n============Basic Blocks===============\n\n");

 printf ("\n----------Block 0-----------\n");
 printf ("Prologue\n");
 r=list->next;

 for( p = r; p; p = p->next )
   {
     simple_instr *q, *l;
    

     printf ("\n----------Block %d-----------\n", p->num);
    
     if (p->next != NULL)
       { 
     l=p->last;    
      q=p->first;
      
     while ( q != l->next)  
       {
         
          instrNum = instrNum + 1;
          printf ("[%d]:  ", instrNum);
          fprint_instr(stdout, q);
	  
         
         q=q->next;
       }  
      
      printf ("\n");
       }
   }  
      
   printf ("Epilogue\n\n"); 


  fprintf( fd, "NUMBER OF NODES: %d\n", k );
  fprintf( fd, "NUMBER OF EDGES: %d\n", j );

  fprintf( fd, "EDGES:\n" );
  for( p = list; p; p = p->next )
    {
      for( i = 0; i < p->link_num; i++ )
	{
	  assert(p->links[i]);
	  fprintf( fd, "(%d -> %d)\n", p->num, p->links[i]->num );
	}
    }
/* ADD YOUR CODE HERE */
   execute(list,fd);


  for( q = list->next, p = list; q; p = q, q = q->next )
    {
      if( p->target_labels != NULL )
	free( p->target_labels );
      if( p->links != NULL )
	free( p->links );
      free(p);
    }

  free(p);
  return inlist;
}







