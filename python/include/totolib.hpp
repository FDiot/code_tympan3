#ifndef MY_ELEMENT_H
#define MY_ELEMENT_H

#include <string>
#include <iostream>
#include <string>

using namespace std;

class MyElement
{
    public:
        MyElement ();
        MyElement (string);
        string Name(){ return _name;}
        virtual ~MyElement ();

    protected:
        string _name;
};

#endif
