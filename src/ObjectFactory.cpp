#include "Scene.hpp"

Object* ObjectFactory::CreateCube(Scene& scene, Object* parent, point3D origin, double sideLength,
    Angle orientation, std::function<void(Object&, float)> onUpdate)
{
    double x = origin.x;
    double y = origin.y;
    double z = origin.z;
    vector <point3D> vertices = {
        {+1, +1, +1},
        {+1, +1, -1},
        {+1, -1, +1},
        {+1, -1, -1},
        {-1, +1, +1},
        {-1, +1, -1},
        {-1, -1, +1},
        {-1, -1, -1}
    };

    vector <Face> indices = {
        // Front face (z+)
        { 0, 2, 4 },
        { 2, 6, 4 },
        // Back face (z-)
        { 1, 5, 3 },
        { 3, 5, 7 },
        // Right face (x+)
        { 0, 1, 2 },
        { 1, 3, 2 },
        // Left face (x-)
        { 4, 6, 5 },
        { 5, 6, 7 },
        // Top face (y+)
        { 0, 4, 1 },
        { 1, 4, 5 },
        // Bottom face (y-)
        { 2, 3, 6 },
        { 3, 7, 6 }
    };

    auto cube = std::make_unique<Mesh>(
        parent,
        origin,
        orientation,
        vector3D{ sideLength, sideLength, sideLength },
        MeshBase(vertices, indices)
    );

    cube.get()->onUpdate = onUpdate;

    scene.objects.push_back(std::move(cube));

    return scene.objects.back().get();
}

Object* ObjectFactory::CreateSphere(Scene& scene, Object* parent, point3D origin, double radius, size_t stacks, int slices,
    Angle orientation, std::function<void(Object&, float)> onUpdate)
{
    vector<point3D> vertices;
    vector<Face> indices;

    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        double phi = M_PI * i / stacks; // from 0 to pi

        for (int j = 0; j <= slices; ++j) {
            double theta = 2.0 * M_PI * j / slices; // from 0 to 2pi

            double x = radius * sin(phi) * cos(theta);
            double y = radius * cos(phi);
            double z = radius * sin(phi) * sin(theta);

            vertices.push_back({ x, y, z });
        }
    }

    // Generate faces (two triangles per quad)
    for (size_t i = 0; i < stacks; ++i) {
        for (size_t j = 0; j < slices; ++j) {
            size_t first = i * (slices + 1) + j;
            size_t second = first + slices + 1;

            indices.push_back({ first, second, first + 1 });
            indices.push_back({ first + 1, second, second + 1 });
        }
    }

    auto sphere = std::make_unique<Mesh>(
        parent,
        origin,
        orientation,
        vector3D{ radius * 2, radius * 2, radius * 2 },
        MeshBase(vertices, indices)
    );

    sphere->onUpdate = onUpdate;

    scene.objects.push_back(std::move(sphere));

    return scene.objects.back().get();
}

Object* ObjectFactory::CreatePlane(Scene& scene, Object* parent, point3D origin, double width, double height,
    Angle orientation, std::function<void(Object&, float)> onUpdate)
{
    vector<point3D> vertices = {
        {  1,  1, 0 },
        {  1, -1, 0 },
        { -1,  1, 0 },
        { -1, -1, 0 },
    };

    vector3D normal = { 0, 0, 1 };

    vector<point3D> backVertices;
    for (auto& vertex : vertices) {
        backVertices.push_back({ vertex - normal * Epsilon });
    }

    // Combine front + back vertices
    vertices.insert(vertices.end(), backVertices.begin(), backVertices.end());

    vector<Face> indices{
        // Front Face (CW)
        { 0, 3, 2 },
        { 0, 1, 3 },
        // Rear Face (CW)
        { 6, 5, 4 },
        { 6, 7, 5 }
    };

    // Build mesh object
    auto plane = std::make_unique<Mesh>(
        parent,
        origin,
        orientation,
        vector3D{ width, height, 1 },
        MeshBase(vertices, indices)
    );

    plane->onUpdate = onUpdate;

    scene.objects.push_back(std::move(plane));

    return scene.objects.back().get();
}

Object* ObjectFactory::CreateDirectionLight(Scene& scene, Object* parent, Angle orientation, double intensity, Colour colour, std::function<void(Object&, float)> onUpdate) {
    auto light = std::make_unique<Light>(parent, point3D{ 0.0, 0.0, 0.0 }, orientation, intensity, colour);

    light.get()->onUpdate = onUpdate;

    scene.objects.push_back(std::move(light));

    return scene.objects.back().get();
}

Object* ObjectFactory::CreateCamera(Scene& scene, Object* parent, point3D position, Angle orientation, dimensions viewPortSize, size_t downscaling, double fov, double nearClipPlane, double farClipPlane, std::function<void(Object&, float)> onUpdate) {
    auto camera = std::make_unique<Camera>(parent, position, orientation, viewPortSize, downscaling, fov, nearClipPlane, farClipPlane);

    camera.get()->onUpdate = onUpdate;

    scene.objects.push_back(std::move(camera));

    return scene.objects.back().get();
}
