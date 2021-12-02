//#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>
#include <QVector>


struct Point {
    Point()
        : m_azt()
        , m_el()
        , m_time() {    }

    Point(float azt, float el, float time)
        : m_azt(azt)
        , m_el(el)
        , m_time(time) {    }

    float azt() const { return m_azt; }
    float el()  const { return m_el; }
    float time() const { return m_time; }

    void setAzt(float azt) { m_azt = azt; }
    void setEl(float el) { m_el = el; }
    void setTime(float time) { m_time = time; }

private:
    float m_azt;
    float m_el;
    float m_time; // May be julian-Time )))
};

void printVector(const QVector<Point>& vector) {
    std::cout << "Print vector: " << std::endl;
    for(auto& point: vector) {
        std::cout << "{ " << point.azt() << ", "
            << point.el() << ", " << point.time() << " }";
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
    // L is the lua state, which you can wrap in a state_view if necessary
    // maybe_exception will contain exception, if it exists
    // description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
    std::cout << "An exception occurred in a function, here's what it says ";
    if (maybe_exception) {
        std::cout << "(straight from the exception): ";
        const std::exception& ex = *maybe_exception;
        std::cout << ex.what() << std::endl;
    }
    else {
        std::cout << "(from the description parameter): ";
        std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
        std::cout << std::endl;
    }

    // you must push 1 element onto the stack to be
    // transported through as the error object in Lua
    // note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
    // so we push a single string (in our case, the description of the error)
    return sol::stack::push(L, description);
}

void will_throw() {
    throw std::runtime_error("oh no not an exception!!!");
}

int main(int, char**) {

    sol::state lua;
    lua.open_libraries(sol::lib::base);

    std::cout << "=== exception_handler ===" << std::endl;

    lua.set_exception_handler(&my_exception_handler);

    lua.set_function("will_throw", &will_throw);

    sol::protected_function_result pfr = lua.safe_script("will_throw()", &sol::script_pass_on_error);

    //c_assert(!pfr.valid());

    sol::error err = pfr;
    std::cout << err.what() << std::endl;

    pfr = lua.safe_script(R"(
        a -
    )", &sol::script_pass_on_error);

    err = pfr;
    std::cout << err.what() << std::endl;

    std::cout << std::endl;



    std::cout << "=== containers ===" << std::endl;

    // Add our type to LUA
    sol::usertype<Point> point_type = lua.new_usertype<Point>("Point",
        sol::constructors<Point(), Point(float, float, float)>());
    point_type["azt"]  = sol::property(&Point::azt, &Point::setAzt);
    point_type["el"]   = sol::property(&Point::el, &Point::setEl);
    point_type["time"] = sol::property(&Point::time, &Point::setTime);

    // Have the function we
    // just defined in Lua
    sol::function print_container  = lua["print_container"];
    sol::function append_container = lua["append_container"];

    // Set a global variable called
    // "arr" to be a vector of 3 Points
    lua["arr"] = QVector<Point>{{}, {0.0, 1.1, 0.1}};

    lua.script(R"(
        p = Point.new()
        arr:add(p)
    )");

    QVector<Point>& reference_to_arr = lua["arr"];

    lua.script(R"(
        p = Point.new(2.0, 2.0, 19.0)
        arr:add(p)
    )");
    printVector(reference_to_arr);

    reference_to_arr.push_back(Point(7.0, 7.0, 999.0));
    printVector(reference_to_arr);

    lua.script(R"(
        p = Point.new(99.0, 99.0, 99.0)
        arr[1] = p
    )");
    printVector(reference_to_arr);

    lua.script(R"(
        function f (x)
            print("container has:")
                for k=1,#x do
                    v = x[k]
                    print("\t", k, v.azt, v.el, v.time)
                end
            print()
        end

        arr[2].azt = 113.0
        arr[2].el  = 114.0
        arr[2].time = 200.0
        f(arr)
    )");

    return 0;
}



