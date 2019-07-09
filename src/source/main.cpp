#include "Writr.h"

#include <iostream>

using namespace writr;

int main()
{
    Writr writr = Writr("file.txt");
    
    writr.start();

    return 0;
}