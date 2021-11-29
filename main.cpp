#include <iostream>
#include <QVector>
#include <sol/sol.hpp>
#include <nlohmann/json.hpp>

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

int main(int, char**) {
    std::cout << "=== containers ===" << std::endl;

    sol::state lua;
    lua.open_libraries();

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
