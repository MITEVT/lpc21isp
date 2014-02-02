#include <stdio.h>
#include <string.h>
#include <errno.h>
static void errorRW(int needed,int actual){
	fprintf(stderr,"%d bytes instead of %d bytes\nDetails: %s\n",actual,needed,strerror(errno));
}
void readError(size_t needed,size_t actual){
	fputs("Read ",stderr);
	errorRW((int)needed,(int)actual);
}
void writeError(size_t needed,size_t actual){
	fputs("Wrote ",stderr);
	errorRW(needed,actual);
}
