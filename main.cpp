#include <iostream>
#include <QVector>
#include <sol/sol.hpp>

static sol::state lua;

struct Simple {
    Simple(): m_val(0) {}
    Simple(int val) : m_val(val) {}

    int val() { return m_val; }
    void setVal(int val) { m_val = val; }

private:
    int m_val;
};

void initLua() {
    lua.open_libraries(sol::lib::base);
}

void test() {
    std::cout << "test!!!" << std::endl;
}

void createSimple() {
    sol::usertype<Simple> simple_type = lua.new_usertype<Simple>("Simple",
        sol::constructors<Simple(), Simple(int)>());

    simple_type["val"] = sol::property(&Simple::val, &Simple::setVal);

    return;
}

void createQVector() {

}



int main() {
    initLua();
    createSimple();

    lua.script_file("script.lua");

    return 0;
}
