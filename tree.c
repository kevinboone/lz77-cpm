/*==========================================================================

tree.c

Original authors : David Costa and Pietro De Rosa
Modified for CP/M by Kevin Boone

This is an implementation of a tree formed from a flat array. Each
entry in the array contains the index of its parent, which is how
the hierarchical structure is maintained.
  
See tree.c for function details 
 
==========================================================================*/

#include "compat.h"
#include <stdio.h>
#ifndef CPM
#include <stdlib.h>
#include <string.h>
#endif
#include "tree.h"

/*==========================================================================
struct node
Nodes are composed of the offset of the sequence in the buffer, the
lengt hof the sequence, the index of the parent node in the tree, 
and indices of its children in the tree.
==========================================================================*/
struct node 
  {
  int len, off;
  int parent;
  int left, right;
  };

/*==========================================================================
tr_create
==========================================================================*/
struct node *tr_create (size)
int size;
  {
  struct node *tree = calloc (size, sizeof(struct node));
  return tree;
  }

/*==========================================================================
tr_destroy
==========================================================================*/
void tr_destroy (tree)
struct node *tree;
  {
  free(tree);
  }

/*==========================================================================
tr_insert
==========================================================================*/
void tr_insert (tree, root, window, abs_off, len, max)
struct node *tree; 
int *root; 
unsigned char *window; 
int abs_off; 
int len; 
int max;
  {
  int i, tmp;
  int off = abs_off % max;    
    
  /* no root: the new node becomes the root */
  if (*root == -1)
    {
    *root = off;
    tree[*root].parent = -1;
    } 
  else
    {
    i = *root;
        
    while (TRUE) 
      {
      tmp = i;
      if (_memcmp (&(window[abs_off]), &(window[tree[i].off]), len) < 0)
        {
        /* go to the left child */
        i = tree[i].left;
        if (i == -1)
          {
          /* set parent-child relation */
          tree[tmp].left = off;
          tree[off].parent = tmp;
          break;
          }
        }
      else
        {
        /* go to the right child */
        i = tree[i].right;
        if (i == -1)
          {
          /* set parent-child relation */
          tree[tmp].right = off;
          tree[off].parent = tmp;
          break;
          }
        }
      }
    }
    
  tree[off].off = abs_off;
  tree[off].len = len;
  tree[off].left = -1;
  tree[off].right = -1;
  }

/*==========================================================================
tr_find 
==========================================================================*/
void tr_find (tree, root, window, index, size, off, len)
struct node *tree; 
int root; 
unsigned char *window; 
int index; 
int size; 
int *off; 
int *len;
  {
  int i, j;
  int _off, _len; 
    
  /* initialize as non-match values */
  _off = 0;
  _len = 0;
    
  if (root == -1)
    {
    *off = 0;
    *len = 0;
    return;
    }
  j = root;
    
  /* flow the tree finding the longest match node */
  while (TRUE)
    {
    /* Look for how many characters are equal between the 
          lookahead and the node */
    for (i = 0; window[index+i] == window[tree[j].off + i] && i < size-1; i++)
      {}
        
    /* if the new match is better than the previous one, save the values */
    if (i > _len)
       {
       _off = index - tree[j].off;
       _len = i;
       }
        
    if (window[index+i] < window[tree[j].off + i] && tree[j].left != -1)
      j = tree[j].left;
    else if (window[index+i] > window[tree[j].off + i] && tree[j].right != -1)
      j = tree[j].right;
    else break;
    }
    
  *off = _off;
  *len = _len;
  }

/*==========================================================================
tr_min_child 
Find the minimum node in the tree and return its index
==========================================================================*/
static int tr_min_child (tree, index)
struct node *tree;
int index;
  {
  int min = index;
    
  while (tree[min].left != -1)
    min = tree[min].left;
    
  return min;
  }

/*==========================================================================
tr_delete
==========================================================================*/
void tr_delete (tree, root, window, abs_sb, max)
struct node *tree; 
int *root; 
unsigned char *window; 
int abs_sb; 
int max;
  {
  int parent, child, sb;
    
  sb = abs_sb % max;  /* from absolute index to relative index (array) */
    
  if (tree[sb].left == -1)
    {
    /* the node to be deleted has not the left child */
    child = tree[sb].right;
    if (child != -1)
      tree[child].parent = tree[sb].parent;
    parent = tree[sb].parent;
    }
  else if (tree[sb].right == -1)
    {
    /* the node to be deleted has not the right child */
    child = tree[sb].left;
    tree[child].parent = tree[sb].parent;
    parent = tree[sb].parent;
    }
  else
    {
    /* The node to be deleted has both the children: it will be replaced
           by the minimum child of its right subtree */
    child = tr_min_child (tree, tree[sb].right);
        
    if (tree[child].parent == sb)
      {
      /* just the left child has to be updated */
      parent = tree[sb].parent;
      tree[child].parent = parent;
      }
    else
      {
      /* also the right child has to be updated */
      parent = tree[child].parent;
      tree[parent].left = tree[child].right;
      if(tree[child].right != -1)
        tree[tree[child].right].parent = parent;
        
      tree[child].right = tree[sb].right;
      tree[child].parent = tree[sb].parent;
        
      if (tree[child].right != -1)
        tree[tree[child].right].parent = child;
        
      parent = tree[child].parent;
      }
        
    tree[child].left = tree[sb].left;
    if (tree[child].left != -1)
      tree[tree[child].left].parent = child;
    }
    
    /* set the parent's child or the child as the root */
    if (parent != -1)
      {
      if (tree[parent].right == sb){
            tree[parent].right = child;
      }
    else
      {
      tree[parent].left = child;
      }
    }
  else
   *root = child;
  }

/*==========================================================================
tr_update
==========================================================================*/
void tr_update (tree, n, max)
struct node *tree; 
int n; 
int max;
  {
  int i;
    
  for (i = 0; i < max; i++)
    {
    if (tree[i].off != -1)
      tree[i].off -= n;
    }
  }

