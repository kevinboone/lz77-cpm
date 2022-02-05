/*==========================================================================

tree.h

Original authors : David Costa and Pietro De Rosa
Modified for CP/M by Kevin Boone

==========================================================================*/

#ifndef __TREE_H 
#define __TREE_H 

struct node; /* Defined in tree.c */

#ifdef CPM
struct node *tr_create ();
void         tr_destroy ();
void         tr_insert ();
void         tr_find ();
void         tr_delete ();
void         tr_update ();
#else
/* Create a tree with the specified number of initial nodes */
struct node *tr_create (int size);

/* Destroy the node and free its memory */
void tr_destroy (struct node *tree);

/* Insert a node in the tree
 * Parameters  tree - pointer to the binary tree array
 *             root - index of the root in the array
 *             window - pointer to the buffer
 *             abs_off - absolute offset of the sequence
 *             len - length of the sequence
 *             max - size of the tree array */
void         tr_insert (struct node *tree, int *root, 
                        unsigned char *window, int off, int len, int max);

/* Find the longest match in the tree
 * Parameters    tree - pointer to the binary tree array
 *               root - index of the root in the array
 *               window - pointer to the buffer
 *               index - starting index of the lookahead
 *               size - actual lookahead size
 * Returned      offset and length of the best match */
void         tr_find (struct node *tree, int root, unsigned char *window, 
                        int index, int size, int *off, int *len);

/* Delete the node from the tree in the specific offset
 * Parameters  tree - pointer to the binary tree
 *             root - index of the root of the binary tree
 *             window - pointer to the buffer
 *             abs_sb - actual starting index of the search buffer
 *             max - size of the tree array */
void         tr_delete (struct node *tree, int *root, unsigned char *window, 
                        int abs_sb, int max);

/* Update the offset when the buffer is shifted
 * Parameters tree - binary tree
 *            n - value to subtract
 *            max - length of the array */
void         tr_update (struct node *tree, int n, int max); 

#endif

#endif
