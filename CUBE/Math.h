#pragma once
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <numbers>

constexpr auto M_PI = std::numbers::pi;
constexpr double Epsilon = 0.00000000001;

using std::vector, std::array;

inline bool isEqual(const double& a, const double& b, const double& epsilon = 1e-9) {
    return std::fabs(a - b) < epsilon;
}

struct dimensions {
    size_t x;
    size_t y;

    dimensions() : x(0), y(0) {};

    dimensions(size_t x, size_t y) : x(x), y(y) {};

    bool operator ==(const dimensions& rightPoint) const {
        const dimensions* leftPoint = this;
        return leftPoint->x == rightPoint.x && leftPoint->y == rightPoint.y;
    }

    dimensions operator *(size_t scalar) {
        return { x * scalar, y * scalar };
    }
};

class Matrix;

struct point {
    double x;
    double y;

    point() : x(0.0), y(0.0) {};

    point(double x, double y) : x(x), y(y) {};

    point operator -(point point) {
        return { x - point.x, y - point.y };
    }

    point(vector<double> vector) : x(vector[0]), y(vector[1])
    {
        if (vector.size() != 2)
            throw std::invalid_argument( "Point: expected vector of size 2 but got" + std::to_string(vector.size()) );
    }

    explicit operator vector<double>() {
        vector<double> pointVector = { x, y };
        return pointVector;
    }

    bool operator ==(const point& rightPoint) const {
        const point* leftPoint = this;
        return leftPoint->x == rightPoint.x && leftPoint->y == rightPoint.y;
    }
};

template<typename Derived>
struct tuple3D {
    double x;
    double y;
    double z;

    tuple3D() : x(0.0), y(0.0), z(0.0) {}

    tuple3D(double x, double y, double z) : x(x), y(y), z(z) {}

    tuple3D(const vector<double>& vector) : x(vector[0]), y(vector[1]), z(vector[2])
    {
        if (vector.size() != 3)
            throw std::invalid_argument( "Tuple3D: expected vector of size 3 but got" + std::to_string(vector.size()) );
    }

    Derived operator-() const {
        return { -x, -y, -z };
    }

    bool operator ==(const Derived& tuple) const {
        return x == tuple.x && y == tuple.y && z == tuple.z;
    }

    operator vector<double>() const {
        return { x, y, z };
    }

    operator std::string() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
};

struct vector3D : public tuple3D<vector3D> {
    using tuple3D::tuple3D;
    using tuple3D::operator-;
    using tuple3D::operator==;

    vector3D operator -(const vector3D& vector) const {
        return { x - vector.x, y - vector.y, z - vector.z };
    }

    vector3D operator +(vector3D vector) const {
        return { x + vector.x, y + vector.y, z + vector.z };
    }

    void operator +=(vector3D vector) {
        *this = *this + vector;
    }

    void operator -=(vector3D vector) {
        *this = *this - vector;
    }

    vector3D operator * (double scalar)const {
        return { x * scalar, y * scalar, z * scalar };
    }

    explicit operator Matrix() const;
};

struct point3D : public tuple3D<point3D> {
    using tuple3D::tuple3D;
    using tuple3D::operator-;
    using tuple3D::operator==;

    vector3D operator -(const point3D& point) const {
        return { x - point.x, y - point.y, z - point.z };
    }

    point3D operator -(const vector3D& vector) const {
        return { x - vector.x, y - vector.y, z - vector.z };
    }

    point3D operator +(const vector3D& vector) const {
        return { x + vector.x, y + vector.y, z + vector.z };
    }

    explicit operator Matrix() const;
};

inline vector3D cross(const vector3D& a, const vector3D& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline double dot(const vector3D& a, const vector3D& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double dot(const vector3D& a, const point3D& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vector3D normalise(const vector3D& v) {
    double length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0) return { 0, 0, 0 }; // avoid divide by zero
    return { v.x / length, v.y / length, v.z / length };
}

struct Quaternion;

struct Angle {
    double yaw;
    double pitch;
    double roll;

    Angle() : yaw(0.0), pitch(0.0), roll(0.0) {};

    Angle(double yaw, double pitch, double roll) : yaw(yaw), pitch(pitch), roll(roll) {};

    Angle Negate() const {
        return Angle(-yaw, -pitch, -roll);
    }

    vector3D Normal() {
        double x = cos(pitch) * sin(yaw);
        double y = sin(pitch);
        double z = cos(pitch) * cos(yaw);

        return normalise({ x, y, z });
    }
};

struct Quaternion {
    double w, x, y, z;

    Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {}

    Quaternion(Angle angle);

    vector3D Normal() const;

    // Quaternion multiplication (combining rotations)
    Quaternion operator*(const Quaternion& q) const;

    // Convert back to Euler angles
    operator Angle() const;
};

class Matrix {
public:
    dimensions size;
    vector<vector<double>> matrix;

    Matrix(dimensions size);

    Matrix(vector<vector<double>> matrix);

    Matrix(std::initializer_list<vector<double>> matrix);

    vector<double>& operator [](const size_t& index) {
        return matrix[index];
    }

    vector<double> operator [](const size_t& index) const {
        return matrix[index];
    }

    Matrix operator *(const Matrix& rightMatrix) const;

    template <typename ContainerType>
    ContainerType operator *(const ContainerType& tuple) const {
        static_assert(
            std::is_same<ContainerType, point3D>::value || std::is_same<ContainerType, vector3D>::value,
            "Matrix multiplication only supported with point3D or vector3D"
            );

        Matrix transformedPoint = *this * static_cast<Matrix>(tuple);

        return static_cast<ContainerType>(transformedPoint);
    }

    Matrix operator *(const double& scalar) const;

    Matrix operator +(const Matrix& rightMatrix) const;

    Matrix operator -(const Matrix& rightMatrix) const;

    operator std::string() const;

    explicit operator point3D() const;

    explicit operator vector3D() const;
};

Matrix RotateZ(double roll);

Matrix RotateX(double pitch);

Matrix RotateY(double yaw);

Matrix Rotate(Angle angle);

Matrix Rotate(Quaternion quaternion);

Matrix InverseRotate(Angle angle);

Matrix Translate(point3D point);

Matrix Scale(vector3D scale);

extern const Matrix identityMatrix;