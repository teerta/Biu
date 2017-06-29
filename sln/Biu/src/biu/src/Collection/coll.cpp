
#include "stdafx.h"
#include "bcoll.h"
#include "bbase.h"
#include "bfunc.h"

#include <random>


using namespace std;
using namespace Biu;


uniform_int_distribution<unsigned int> g_CreateKeyRandNumGen;
mt19937 g_CreateKeyRandNumRng;


unsigned int 
Biu::
CreateKey_RandomNumber()
{
    return g_CreateKeyRandNumGen(g_CreateKeyRandNumRng);
}
