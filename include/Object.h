#pragma once
#include <functional>

#include "math.h"

struct Colour {
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    bool operator ==(const Colour& colour) const {
        return r == colour.r && g == colour.g && b == colour.b;
    }

    Colour operator *(const double& scalar) const {
        return { r * scalar, g * scalar, b * scalar };
    }

    Colour operator /(const double& scalar) const {
        return { r / scalar, g / scalar, b / scalar };
    }

    Colour operator +(const Colour& colour) const {
        return { r + colour.r, g + colour.g, b + colour.b };
    }
};

class Object {
public:
    Quaternion orientation;
    point3D position;
    vector3D velocity;

    Object* parent = nullptr;

    std::function<void(Object&, float)> onUpdate;

    Object(point3D position, Angle orientation, Object* parent) : position(position), orientation(orientation), parent(parent) {};

    virtual void Update(float deltaTime) {
        position = position + velocity * deltaTime;
        if (onUpdate) {
            onUpdate(*this, deltaTime);
        }
    }

    virtual Matrix getTransformMatrix() const {
        return ((parent) ? parent->getTransformMatrix() : identityMatrix) * Translate(position) * Rotate(orientation);
    }

    virtual Matrix getInverseTransformMatrix() const {
        return InverseRotate(orientation) * Translate(-position) * ((parent) ? parent->getInverseTransformMatrix() : identityMatrix);
    }

    void rotate(Angle angle) {
        orientation = static_cast<Quaternion>(angle) * orientation;
    }

    vector3D getDirection() const {
        return -orientation.Normal();
    }
};

class Face {
public:
    vector <size_t> indices;
    vector3D normal;

    Face() : indices({0, 0, 0}) {}

    Face(const vector<size_t>& indices) : indices(indices) {};

    Face(const std::initializer_list<size_t>& list) : indices(list) {}

    Face(const std::initializer_list<size_t>& list, const point3D& normal) : indices(list), normal(normal) {}

    void findNormal(const vector<point3D>& vertices) {
        vector3D edge1 = vertices[2] - vertices[0];
        vector3D edge2 = vertices[1] - vertices[0];

        normal = cross(edge1, edge2);
        normal = normalise(normal);
    }

    bool operator ==(const Face& face) const {
        return face.indices == indices;
    }
};

struct MeshBase {
    vector <point3D> vertices;
    vector <Face> indices;

    void TransformNormals(const Matrix& matrix) {
        for (Face& face : indices) {
            face.normal = normalise(matrix * face.normal);
        }
    }

    void TransformVertices(const Matrix& matrix) {
        for (point3D& vertice : vertices) {
            vertice = matrix * vertice;
        }
    }

    void Transform(const Matrix& matrix) {
        TransformNormals(matrix);
        TransformVertices(matrix);
    }
};

struct RenderMeshBase {
    vector <point4D> vertices;
    vector <Face> indices;

    RenderMeshBase(MeshBase meshBase) : indices(meshBase.indices) {
        vertices.reserve(meshBase.vertices.size());

        for (point3D vertice : meshBase.vertices) {
            vertices.push_back(static_cast<point4D>(vertice));
        }
    }

    void TransformNormals(const Matrix& matrix) {
        for (Face& face : indices) {
            face.normal = normalise(matrix * face.normal);
        }
    }

    void TransformVertices(const Matrix& matrix) {
        for (point4D& vertice : vertices) {
            vertice = matrix * vertice;
        }
    }

    void Transform(const Matrix& matrix) {
        TransformNormals(matrix);
        TransformVertices(matrix);
    }

    void PerspectiveDivide() {
        for (point4D& vertice : vertices) {
            vertice.perspectiveDivide();
        }
    }

    explicit operator MeshBase() {
        MeshBase meshBase;

        meshBase.indices = indices;

        meshBase.vertices.reserve(vertices.size());

        for (point4D vertice : vertices) {
            meshBase.vertices.push_back(static_cast<point3D>(vertice));
        }

        return meshBase;
    }
};

class Mesh : public Object {
public:
    MeshBase data;

    vector3D scale;

    Mesh(Object* parent, point3D position, Angle orientation, vector3D scale, MeshBase data) : Object(position, orientation, parent), scale(scale), data(data) {};

    Matrix getTransformMatrix() const override {
        return Object::getTransformMatrix() * Scale(scale);
    }

    Matrix getInverseTransformMatrix() const override {

        return Scale({ 1/scale.x, 1/scale.y, 1/scale.z }) * Object::getInverseTransformMatrix();
    }

    MeshBase getWorldViewData() const {
        MeshBase worldViewData = data;
        worldViewData.Transform(getTransformMatrix());
        return worldViewData;
    }

    void findFaceNormals() {
        for (Face& face : data.indices) {
            vector<point3D> vertices = { data.vertices[face.indices[0]], data.vertices[face.indices[1]], data.vertices[face.indices[2]] };

            face.findNormal(vertices);
        }
    }
};

class Camera : public Object {
public:
    double fov;
    double aspectRatio;
    double nearClipPlane;
    double farClipPlane;
    dimensions viewPortSize;
    size_t downscaling;

    Camera(Object* parent, point3D position, Angle orientation, dimensions viewPortSize, size_t downscaling = 1, double fov = 1.35, double nearClipPlane = 0.1, double farClipPlane = 1000.0) :
        Object(position, orientation, parent), fov(fov), nearClipPlane(nearClipPlane), farClipPlane(farClipPlane), viewPortSize(viewPortSize), downscaling(downscaling) {

        aspectRatio = static_cast<double>(viewPortSize.x) / viewPortSize.y;
    }

    Matrix getViewMatrix() const {
        return InverseRotate(orientation) * Translate(-position);
    }

    Matrix getPerspectiveMatrix() const {
        double focalLength = 1.0 / tan(fov / 2.0);
        double depth = farClipPlane - nearClipPlane;

        return {
            {focalLength / aspectRatio, 0,           0,                                           0},
            {0,                          -focalLength, 0,                                           0},
            {0,                          0,          (farClipPlane + nearClipPlane) / depth, -(2 * nearClipPlane * farClipPlane) / depth},
            {0,                          0,          1,                                           0}
        };
    }

    Matrix getScreenMatrix() const {
        return Translate( { viewPortSize.x * downscaling / 2.0, viewPortSize.y * downscaling / 2.0, 0 }) *
               Scale(     { viewPortSize.x * downscaling / 2.0, viewPortSize.y * downscaling / 2.0, 1});
    }
};

class Light : public Object {
public:
    double intensity;
    Colour colour;

    Light(Object*parent, point3D position, Angle orientation, double intensity = 1.0, Colour colour = { 255, 255, 255 }) : Object(position, orientation, parent), intensity(intensity), colour(colour) {};
};