#include "totolib.hpp"

MyElement::MyElement(string name) : _name(name)
{
    cout << "Class MyElement: " << _name << " created" << endl;
}

MyElement::MyElement() : _name("Unknown")
{
    cout << "Class MyElement: " << _name << " created" << endl;
}

MyElement::~MyElement()
{
    cout << "My Element " << _name << " got destructed." <<endl;
}
