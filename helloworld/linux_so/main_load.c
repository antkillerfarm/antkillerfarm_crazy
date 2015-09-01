#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char argv[]) 
{
  void *dl_handle;
  void (*func)(void);
  char *error;

  /* Open the shared object */
  dl_handle = dlopen("./libhello.so", RTLD_LAZY );
  if (!dl_handle) {
    printf( "!!! %s\n", dlerror() );
    return;
  }

  /* Resolve the symbol (method) from the object */
  func = dlsym( dl_handle,"print_hello" );
  error = dlerror();
  if (error != NULL) {
    printf( "!!! %s\n", error );
    return;
  }

  (*func)();

  /* Close the object */
  dlclose( dl_handle );

  return 0;
}
