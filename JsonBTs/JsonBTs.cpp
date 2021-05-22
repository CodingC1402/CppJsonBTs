// JsonBTs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "BTs.h"

int main()
{
    BTs tree;
    tree.Load("bt.json");
    std::cout << "Hello World!\n";
}