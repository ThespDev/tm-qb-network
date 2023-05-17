#include "qb.h"

const char Usage_msg[] = ("USAGE: ./QB <TM-IP-Address> <CSV-File-location> <Language #(C,Java or Python)>");

void usage(){
  printf("\n%s\n",Usage_msg);
  exit(EXIT_FAILURE);
}


