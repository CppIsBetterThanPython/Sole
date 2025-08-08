#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using std::vector;

struct tuple3D {
    double x;
    double y;
    double z;

    tuple3D() : x(0.0), y(0.0), z(0.0) {}

    tuple3D(double x, double y, double z) : x(x), y(y), z(z) {}

    tuple3D(vector<double> vector) : x(vector[0]), y(vector[1]), z(vector[2])
    {
        if (vector.size() != 3)
        {
            throw std::invalid_argument(
                "Tuple3D: expected vector of size 3 but got" + std::to_string(vector.size())
            );
        }
    }

    tuple3D operator-() const {
        return { -x, -y, -z };
    }

    operator vector<double>() const {
        return { x, y, z };
    }

    operator std::string() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
};

struct vector3D : public tuple3D {
    using tuple3D::tuple3D;

    vector3D operator -(const vector3D& other) {
        return { x - other.x, y - other.y, z - other.z };
    }

    vector3D operator +(vector3D point) {
        return { x + point.x, y + point.y, z + point.z };
    }

    vector3D operator * (double scalar) {
        return { x * scalar, y * scalar, z * scalar };
    }
};

struct point3D : public tuple3D {
    using tuple3D::tuple3D;

    vector3D operator -(const point3D& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    point3D operator -(const vector3D& vector) const {
        return { x - vector.x, y - vector.y, z - vector.z };
    }

    point3D operator +(const vector3D& vector) const {
        return { x + vector.x, y + vector.y, z + vector.z };
    }
};

int main()
{
    std::cout << "Hello World!\n";
}
