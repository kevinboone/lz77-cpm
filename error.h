/*============
 * error.h
 *===========*/

#ifdef CPM
void show_error ();
#else
void show_error (CONST char *msg, int err);
#endif



