
#include "randgen.hpp"

const str randgen::rand_dir = file::environ("RINDFILE_PATH") ;
unsigned long int randgen::used = 0 ;

