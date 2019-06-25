#include <dlfcn.h>
#include <malloc.h>
#include <pthread.h>
#include <sys/types.h>

/* Prototypes for our hooks.  */
static void my_init_hook(void);
static void *my_malloc_hook(size_t, const void *);
static void *my_malloc_hook(size_t, const void *);
static void my_free_hook(void *, const void *);

void *(*old_malloc_hook)(size_t __size, const void *);

static void my_init(void) {
  old_malloc_hook = __malloc_hook;
  __malloc_hook = my_malloc_hook;
}

int allowed_mallocs = 1;
int block_malloc;
pthread_t blocked_thread;
static void *my_malloc_hook(size_t size, const void *caller) {
  void *result;
  while (block_malloc) {
    if (size != 64) {
      break;
    } else {
      allowed_mallocs = 0;
    }
  }
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  /* Call recursively */
  result = malloc(size);
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  /* printf might call malloc, so protect it too. */
  if (size == 64) {
	  printf("malloc 64");
  }
  // printf ("malloc %u (%u) returns %p\n", pthread_self(), (unsigned int) size,
  // result);
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  return result;
}

void *inc_x(void *x_void_ptr) {

  /* increment x to 100 */
  int *x_ptr = (int *)x_void_ptr;
  while (++(*x_ptr) < 100)
    ;

  printf("x increment finished\n");

  /* the function must return something - NULL will do */
  return NULL;
}

/* this function is run by the second thread */
void *do_dlopen(void *x_void_ptr) {

  /* increment x to 100 */
  int *x_ptr = (int *)x_void_ptr;
  while (*x_ptr == 0) {
  }

  dlopen("libGL.so", 1);
  printf("done open\n");

  return NULL;
}

pthread_t inc_x_thread;
pthread_t thread2;
main() {
  /* this variable is our reference to the second thread */

  dlopen("libexpat.so", 1);
  dlopen("libfontconfig.so", 1);
  my_init();
  int x = 0;
  /* create a second thread which executes inc_x(&x) */
  if (pthread_create(&inc_x_thread, NULL, do_dlopen, &x)) {

    fprintf(stderr, "Error creating thread\n");
    return 1;
  }
  blocked_thread = inc_x_thread;
  block_malloc = 1;
  x = 1;

  /* this variable is our reference to the second thread */

  int y;
  while (allowed_mallocs) {
  }
  /* create a second thread which executes inc_x(&x) */
  if (pthread_create(&thread2, NULL, inc_x, &y)) {

    fprintf(stderr, "Error creating thread\n");
    return 1;
  }
  block_malloc = 0;
  while (1) {
  }
}
