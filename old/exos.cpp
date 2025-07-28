#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
//#include <cstdio>

#include <memory>
#include <vector>
#include <variant>

extern "C" float sqrtf(float);

int sum(int x, int y) {
    return x + y;
}

float sum(float x, float y) {
    return x + y;
}

unsigned char sum(unsigned char x, unsigned char y) {
    return x + y;
}

struct CatConstructionError {};

// header file
class Cat {
protected:
    char* name;
    float weight;
    float length;
    unsigned int leg_count;
public:
    Cat(const char* name = "miaou");
    ~Cat();

    void print_name();

    void operator+(int x);
};

// compilation file

Cat::Cat(const char* name_): name(nullptr), weight(0), length(5), leg_count(4) {
    puts("ctor");
    char* n = strdup(name_);
    if (n == NULL) {
        throw CatConstructionError();
    }
}

Cat::~Cat() {
    puts("dtor");
    free(this->name);
}

void Cat::print_name() {
    printf("My name is %s\n", this->name);
}

void Cat::operator+(int x) {
    puts("ope+");
    printf("%s: %x", this->name, x);
    puts("");
}

// "has a"
// "is a"
struct LegLessCat: Cat {
    LegLessCat(const char* name_): Cat(name_) {
        this->leg_count = 0;
    }
};

/*
void cat_init(Cat* cat, const char* name) {
    puts("init");
    cat->name = strdup(name);
    cat->weight = 99;
    cat->length = 5;
    cat->leg_count = 4;
}

void cat_deinit(Cat* cat) {
    puts("deinit");
    free(cat->name);
}
*/

template<typename T>
struct Vec {
    size_t available;
    size_t used;
    T* data;

    Vec(): available(0), used(0), data(nullptr) {}

    ~Vec() {
        free(data);
    }

    void push(T x) {
        if (used == available) {
            // available = available ? 2 * available : 1;
            if (available) {
                available = 2 * available;
            } else {
                available = 1;
            }
            /*
            Rust:
            available = if available != 0 { 2 * available } else { 1 };
            */
            data = (T*) realloc(data, available * sizeof(T));
        }
        data[used++] = x;
    }
};

namespace old_school {
    enum ShapeType {
        Circle,
        Rectangle,
        Square,
        Triangle,
    };

    // struct sockaddr
    struct Shape {
        ShapeType shape_type;
    };

    // struct sockadddr_in
    struct Circle {
        ShapeType shape_type;
        float radius;
    };

    // struct sockadddr_in6
    struct Rectangle {
        ShapeType shape_type;
        float width;
        float height;
    };

    // …
    struct Square {
        ShapeType shape_type;
        float size;
    };

    // …
    struct Triangle {
        ShapeType shape_type;
        float base;
        float height;
    };

    float area(struct Shape* shape) {
        switch (shape->shape_type) {
            case ShapeType::Circle: {
                struct Circle* circle = (struct Circle*) shape;
                return circle->radius * circle->radius * M_PI;
            }
            case ShapeType::Rectangle: {
                struct Rectangle* rectangle = (struct Rectangle*) shape;
                return rectangle->width * rectangle->height;
            }
            case ShapeType::Square: {
                struct Square* square = (struct Square*) shape;
                return square->size * square->size;
            }
            case ShapeType::Triangle: {
                struct Triangle* triangle = (struct Triangle*) shape;
                return triangle->base * triangle->height / 2;
            }
            default:
                exit(1);
        }
    }
}

namespace tagged_union {
    enum ShapeType {
        Circle,
        Rectangle,
        Square,
        Triangle,
    };

    // struct sockadddr_in
    struct Circle {
        float radius;
    };

    // struct sockadddr_in6
    struct Rectangle {
        float width;
        float height;
    };

    // …
    struct Square {
        float size;
    };

    // …
    struct Triangle {
        float base;
        float height;
    };


    // struct sockaddr
    struct Shape {
        ShapeType shape_type;
        union {
            struct Circle circle;
            struct Rectangle rectangle;
            struct Square square;
            struct Triangle triangle;
        };
    };
    /*
    4 bytes  (int)   shape_type
    4 bytes  (float) circle.radius
    4 bytes  (padding)

    Shape circle_shape;
    circle_shape.rectangle.height;
    */

    float area(struct Shape* shape) {
        switch (shape->shape_type) {
            case ShapeType::Circle:
                return shape->circle.radius * shape->circle.radius * M_PI;
            case ShapeType::Rectangle:
                return shape->rectangle.width * shape->rectangle.height;
            case ShapeType::Square:
                return shape->square.size * shape->square.size;
            case ShapeType::Triangle:
                return shape->triangle.base * shape->triangle.height / 2;
            default:
                exit(1);
        }
    }
}

namespace variant {
    struct Circle {
        float radius;
    };

    struct Rectangle {
        float width;
        float height;
    };

    struct Square {
        float size;
    };

    struct Triangle {
        float base;
        float height;
    };

    using Shape = std::variant<Circle, Rectangle, Square, Triangle>;

    float area(const Shape& shape) {
        return std::visit([](const auto& s) -> float {
            using T = std::decay_t<decltype(s)>;
            if constexpr (std::is_same_v<T, Circle>) {
                return s.radius * s.radius * M_PI;
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return s.width * s.height;
            } else if constexpr (std::is_same_v<T, Square>) {
                return s.size * s.size;
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return s.base * s.height / 2.0f;
            }
        }, shape);
    }
}

namespace c_polymorphism {
    struct ShapeVTable {
        // area is a pointer to a function of signature float(struct Shape*)
        float (*area)(struct Shape*);
    };

    struct Shape {
        ShapeVTable* vtable;
    };

    struct Circle {
        ShapeVTable* vtable;
        float radius;
    };

    float circle_area(Circle* circle) { return circle->radius * circle->radius * M_PI; }

    struct Rectangle {
        ShapeVTable* vtable;
        float width;
        float height;
    };

    float rectangle_area(Rectangle* rectangle) { return rectangle->width * rectangle->height; }

    struct Square {
        ShapeVTable* vtable;
        float size;
    };

    float square_area(Square* square) { return square->size * square->size; }

    struct Triangle {
        ShapeVTable* vtable;
        float base;
        float height;
    };

    float triangle_area(Triangle* triangle) { return triangle->base * triangle->height; }

    void use_area(Shape* shape) {
        printf("%f\n", shape->vtable->area(shape));
    }

    void lol() {
        ShapeVTable circle_vtable = ShapeVTable {
            .area = (float(*)(Shape*) ) circle_area,
        };
        Circle circle = Circle {
            .vtable = &circle_vtable,  
            .radius = 1.f,
        };
        
        printf("%f\n", circle.vtable->area((Shape*) &circle));

        use_area((Shape*) &circle);

        /*
        std::vector<Shape> vec;
        vec.push_back(circle);
        vec.push_back(rectangle);
        */
    }
}

namespace cpp_polymorphism {
    struct Shape {
        virtual float area() = 0;
    };

    struct Circle: Shape {
        float radius;
        Circle(float radius) : radius(radius) {}
        float area() { return radius * M_PI; }
    };

    struct Rectangle: Shape {
        float width;
        float height;
        Rectangle(float width, float height) : width(width), height(height) {}
        float area() { return width * height; }
    };

    struct Square: Shape {
        float size;
        Square(float size) : size(size) {}
        float area() { return size * size; }
    };

    struct Triangle: Shape {
        float base;
        float height;
        Triangle(float base, float height) : base(base), height(height) {}
        float area() { return base * height / 2; }
    };

    void use_area(Shape* shape) {
        printf("%f\n", shape->area());
    }

    void lol() {
        Circle circle(42.f);
        printf("%f\n", circle.area());

        Rectangle rectangle(42.f, 45.f);
        printf("%f\n", rectangle.area());

        use_area(&circle);
        use_area(&rectangle);

        /*
        std::vector<Shape> vec;
        vec.push_back(circle);
        vec.push_back(rectangle);
        */
    }
}

int main() {
    c_polymorphism::lol();
    cpp_polymorphism::lol();

    sum(1, 2);
    sum(1.2f, 3.4f);

    sqrtf(1.2f);

    // RAII = Resource Acquisition Is Initialization
    // OBRM = Ownership Based Resource Management
    puts("before scope");
    {
        try {
            Cat cat = Cat("Misty");
            cat + 2;
            cat.print_name();
        } catch (CatConstructionError error) {
            return 1;
        }
        // ...
    }
    puts("after scope");

    Vec<int> v = Vec<int>();
    v.push(3);

    // smart pointer
    std::unique_ptr<int> smart_pointer = std::make_unique<int>();

    /*
    Cat* cat = (Cat*) malloc(sizeof(Cat));
    cat_init(cat);
    cat_deinit(cat);
    free(cat);
    */

    /*
    Cat* cat = new Cat(); // allocate memory + call constructor
    delete cat; // call destructor + deallocate memory

    Cat* cat = new Cat[10]; // allocate memory + call constructors
    delete[] cat; // call destructors + deallocate memory
    */

    std::vector<int> w;
    w.push_back(3);
    // C style
    for (size_t i = 0; i < w.size(); i++) {
        printf("%d\n", w[i]);
    }
    // C++
    for (std::vector<int>::iterator it = w.begin(); it != w.end(); ++it) {
        printf("%d\n", *it);
    }
    // C++ auto
    for (auto it = w.begin(); it != w.end(); ++it) {
        printf("%d\n", *it);
    }
    // C++ range-for loop
    for (auto val: w) {
        printf("%d\n", val);
    }
}

namespace bla {
    int x = 42;
}