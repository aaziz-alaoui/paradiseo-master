#include <iostream>
#include <string>

#include <eo>

struct OpInterface
{
    std::string _name;
    OpInterface(std::string name) : _name(name) {}
    virtual void operator()() = 0;
    virtual ~OpInterface() {}
};

struct OpA : public OpInterface
{
    OpA(std::string name) : OpInterface(name) {}
    void operator()(){std::cout << _name << std::endl;}
};

struct OpB : public OpInterface
{
    OpB(std::string name, std::string suffix) : OpInterface(name+suffix) {}
    void operator()(){std::cout << _name << std::endl;}
};

int main(int /*argc*/, char** /*argv*/)
{
    // Forge container using indices.
    eoForgeVector<OpInterface> indexed_factories;

    // Capture constructor's parameters and defer instantiation.
    indexed_factories.add<OpA>("I'm A");
    indexed_factories.setup<OpA>(0, "I'm actually A"); // Edit
    indexed_factories.add<OpB>("I'm B", " prime");
    indexed_factories.add<OpB>("I'm a B", " junior");

    // Actually instantiante and call.
    indexed_factories.instantiate(0)();
    indexed_factories.instantiate(1)();
    indexed_factories.instantiate(2)();
}
