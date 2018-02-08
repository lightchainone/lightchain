#include <cstdio>
#include <iostream>
#include "idm_interface.h"
using namespace IDL;

int main(int argc,char **argv)
{
  int i;
  char *file;
  idm_t *idm_data;
  for(i=1;i<argc;i++){
    file=argv[i];
    idm_data=alloc_idm();
    load_idm(file,idm_data);
    fprintf(stderr,"********************IDL file '%s' structure********************\n",file);
    print_idm(idm_data);
    free_idm(idm_data);
  }
  return 0;
}
  
  


