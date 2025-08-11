#pragma once
#include <cassert>

#include "math.h"
#include "Console.h"
#include "Object.h"

namespace graphics {
    struct zPixel {
        double z;
        Colour colour;

        bool operator ==(zPixel pixel) {
            return pixel.colour == colour;
        }

        operator Colour() {
            return colour;
        }
    };

    struct Pixels {
        vector<vector<Colour>> pixels;
        dimensions size;

        Pixels(vector<vector<Colour>> pixels) : pixels(pixels) {
            size = { pixels.size(), pixels[0].size() };
        }

        Pixels(dimensions size) : size(size) {
            pixels = vector<vector<Colour>>(size.x, vector<Colour>(size.y));
        }

        vector<Colour>& operator [](size_t index) {
            return pixels[index];
        }

        vector<Colour> operator [](size_t index) const {
            return pixels[index];
        }
    };

    struct ZBuffer {
        vector<vector<zPixel>> zBuffer;
        dimensions size;

        ZBuffer(vector<vector<zPixel>> zBuffer) : zBuffer(zBuffer) {
            size = { zBuffer.size(), zBuffer[0].size() };
        }

        ZBuffer(dimensions size) : size(size) {
            zBuffer = vector<vector<zPixel>>(size.x, vector<zPixel>(size.y, { 1 }));
        }

        vector<zPixel>& operator [](size_t index) {
            if (index >= size.x)
                throw std::invalid_argument("ZBuffer: index too large");

            return zBuffer[index];
        }

        operator Pixels() {
            Pixels pixels(size);

            for (size_t x = 0; x < size.x; x++) {
                for (size_t y = 0; y < size.y; y++) {
                    pixels[x][y] = zBuffer[x][y].colour;
                }
            }

            return pixels;
        }
    };

    struct Plane {
        point3D point;
        vector3D normal;

        bool operator ==(const Plane& plane) const {
            return point == plane.point || normal == plane.normal;
        }
    };

    inline vector<RenderMeshBase> TransformVertices(vector<RenderMeshBase> meshData, const Matrix& matrix) {
        for (RenderMeshBase& Mesh : meshData) {
            Mesh.TransformVertices(matrix);
        }

        return meshData;
    }

    inline vector<MeshBase> TransformNormals(vector<MeshBase> meshData, const Matrix& matrix) {
        for (MeshBase& Mesh : meshData) {
            Mesh.TransformNormals(matrix);
        }

        return meshData;
    }

    inline vector<RenderMeshBase> PerspectiveDivide(vector<RenderMeshBase> meshData) {
        for (RenderMeshBase& Mesh : meshData) {
            Mesh.PerspectiveDivide();
        }

        return meshData;
    }

    inline vector<MeshBase> ConvertToCartesian(vector<RenderMeshBase> meshData) {
        vector<MeshBase> meshDataCartesian;

        meshDataCartesian.reserve(meshData.size());

        for (RenderMeshBase mesh : meshData) {
            meshDataCartesian.push_back(static_cast<MeshBase>(mesh));
        }

        return meshDataCartesian;
    }

    inline static point4D intersectionByPlane(point4D point1, point4D point2, vector4D plane) {
        vector4D edgeVector = point2 - point1;

        double dist1 = dot(plane, point1);
        double dist2 = dot(plane, point2);

        double intersectionFactor = dist1 / (dist1 - dist2);

        point4D intersectionPoint = point1 + edgeVector * intersectionFactor;

        return intersectionPoint;
    }

    static bool clipByPlane(Face face, vector4D plane, RenderMeshBase& meshBase);

    vector<RenderMeshBase> ClipFaces(vector<RenderMeshBase> NDCData, Camera* camera);

    template<typename T1, typename T2, typename T3>
    inline static auto edgeFunction(const T1& a, const T2& b, const T3& c) {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    }

    static zPixel& interpolatePixel(const int& x, const int& y, const vector<point3D>& face, const vector3D& faceNormal, const vector3D& lightDirection);

    static void topTriangleRasterisation(vector<point3D> face, ZBuffer& zBuffer, const vector3D& faceNormal, const vector3D& lightDirection);

    static void bottomTriangleRasterisation(vector<point3D> face, ZBuffer& zBuffer, const vector3D& faceNormal, const vector3D& lightDirection);

    Pixels ScanLineRasterise(const vector<MeshBase>& ScreenData, const dimensions outputSize, const Light* light);

    void Downscale(Pixels& pixels, size_t scalar);

    inline static void pixelToChar(const Colour& backgroundColour, const Colour& foregroundColour, std::string& buffer) {
        buffer = consoleForegroundColourCode(foregroundColour) + consoleBackgroundColourCode(backgroundColour) + std::string(reinterpret_cast <const char*>(u8"▄"));
    }

    void PrintToConsoleBlock(const Pixels& pixels);

    inline static char pixelToChar(Colour pixel) {
        static const std::string gradient = " .:-=+*#";

        return gradient[pixel.r * 8];
    }

    void PrintToConsoleAscii(const Pixels& zBuffer);
};