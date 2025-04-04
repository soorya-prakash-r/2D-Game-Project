#include <iostream>
using namespace std;
#include "0.h" 
#include "2.h"
#include "1.h"
#include "3.h"
#include "4.h"

int main() 
{
    int p = 4;
    try
    {
    while (p < 8)
    { 
        if (p == 4)  
        {
            p = menu();
            continue;
        }
        else if (p == 6)
        {
            p = level1();
            continue;
        }
        else if (p == 2)
        {
            p = level2();
            continue;
        }
        else if (p == 3)           
        {
            p = level3();
            continue;
        }
        else if (p == 7)
        {
            p = instructions();
            continue;
        }         
    } 
    }
    catch (const exception &e)
    {
        cout<<e.what()<<endl;
    }
    return 0;
}
