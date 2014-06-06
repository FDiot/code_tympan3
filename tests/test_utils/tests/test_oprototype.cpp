//! \test test_oprototype.cpp

#include <memory>

#include "gtest/gtest.h"
#include "Tympan/Tools/OPrototype.h"

using namespace std;

class MyClass : public OPrototype
{
    public:
        MyClass() {}
        OPrototype* clone() const { return new MyClass(); }
};

void register_myclass()
{
    OPrototype::add_factory("MyClass", std::move(build_factory<MyClass>()));
}

TEST(TestTestUtils, oprototype_newclass)
{
    register_myclass();
    ASSERT_FALSE (OPrototype::findPrototype("MyClass") == -1);
    shared_ptr<OPrototype> pOProto (OPrototype::findAndClone("MyClass"));
    shared_ptr<MyClass> pMC = dynamic_pointer_cast<MyClass>(pOProto);
    ASSERT_FALSE (pMC == nullptr);
} // TEST(TestTestUtils, oprototype)


