#include <algorithm>

#include "Graphics.h"

static bool graphics::clipByPlane(Face face, vector4D plane, RenderMeshBase& meshBase) {
    vector<point4D> faceVertices = { meshBase.vertices[face.indices[0]], meshBase.vertices[face.indices[1]], meshBase.vertices[face.indices[2]] };

    vector<point4D> clippedTriangle;

    point4D previousPoint = faceVertices.back();
    bool previousPointInside = dot(plane, previousPoint) >= -Epsilon;

    int amountOutside = 0;

    for (const point4D& currentPoint : faceVertices) {
        bool currentPointInside = dot(plane, currentPoint) >= -Epsilon;

        if (currentPointInside != previousPointInside) {
            auto newEnd = std::remove(meshBase.indices.begin(), meshBase.indices.end(), face);
            meshBase.indices.erase(newEnd, meshBase.indices.end());

            clippedTriangle.push_back(intersectionByPlane(previousPoint, currentPoint, plane));
        }

        if (currentPointInside) {
            clippedTriangle.push_back(currentPoint);
        }
        else {
            amountOutside++;
        }

        previousPoint = currentPoint;
        previousPointInside = currentPointInside;
    }

    if (amountOutside == 3) {
        auto newEnd = std::remove(meshBase.indices.begin(), meshBase.indices.end(), face);
        meshBase.indices.erase(newEnd, meshBase.indices.end());

        return true;
    }

    if (faceVertices == clippedTriangle) { return false; }

    if (clippedTriangle.size() == 3) {
        Face clippedFace;

        clippedFace.normal = face.normal;

        for (int i = 0; i < 3; i++) {
            auto it = std::find(meshBase.vertices.begin(), meshBase.vertices.end(), clippedTriangle[i]);

            if (it != meshBase.vertices.end()) {
                clippedFace.indices[i] = it - meshBase.vertices.begin();
                continue;
            }
            clippedFace.indices[i] = meshBase.vertices.size();
            meshBase.vertices.push_back(clippedTriangle[i]);
        }

        meshBase.indices.push_back(clippedFace);
    }
    else if (clippedTriangle.size() == 4) {
        vector<size_t> clippedTriangleIndices(4);

        for (int i = 0; i < 4; i++) {
            auto it = std::find(meshBase.vertices.begin(), meshBase.vertices.end(), clippedTriangle[i]);

            if (it != meshBase.vertices.end()) {
                clippedTriangleIndices[i] = it - meshBase.vertices.begin();
                continue;
            }
            clippedTriangleIndices[i] = meshBase.vertices.size();
            meshBase.vertices.push_back(clippedTriangle[i]);
        }

        Face clippedFace1 = { clippedTriangleIndices[0], clippedTriangleIndices[1], clippedTriangleIndices[3] };
        Face clippedFace2 = { clippedTriangleIndices[1], clippedTriangleIndices[2], clippedTriangleIndices[3] };

        clippedFace1.normal = face.normal;
        clippedFace2.normal = face.normal;

        meshBase.indices.push_back(clippedFace1);
        meshBase.indices.push_back(clippedFace2);
    }

    return true;
}

vector<RenderMeshBase> graphics::ClipFaces(vector<RenderMeshBase> clipData, Camera* camera) {

    for (RenderMeshBase& clipMesh : clipData) {

        // TODO: Implement Back Face Culling properly
        // Back Face Culling
        //clipMesh.indices.erase(std::remove_if(
        //    clipMesh.indices.begin(),
        //    clipMesh.indices.end(),
        //    [camera](Face face) {
        //        return dot(face.normal, camera->getDirection()) <= 0;
        //    }),
        //    clipMesh.indices.end());

        vector<vector4D> clippingPlanes = {
                                                      {  1,  0,  0,  1 },
                                                      { -1,  0,  0,  1 },
                                                      {  0,  1,  0,  1 },
                                                      {  0, -1,  0,  1 },
                                                      {  0,  0,  1,  1 },
                                                      {  0,  0, -1,  1 } 
                                                  };

        for (size_t j = 0; j < clipMesh.indices.size();) {
            bool wasClipped = false;
            for (const vector4D& clippingPlane : clippingPlanes) {
                wasClipped = clipByPlane(clipMesh.indices[j], clippingPlane, clipMesh);
                if (wasClipped) {
                    break;
                }
            }

            if (!wasClipped) {
                j++;
            }
        }
    }

    return clipData;
}

static auto edgeFunction(const point3D& a, const point3D& b, const point3D& c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

static graphics::zPixel& graphics::interpolatePixel(const int& x, const int& y, const vector<point3D>& face, const vector3D& faceNormal, const vector3D& lightDirection) {
    point3D pixelpoint = { x + 0.5, y - 0.5, 0 };

    double area  = graphics::edgeFunction(face[0], face[1], face[2]);
    double alpha = graphics::edgeFunction(face[1], face[2], pixelpoint) / area;
    double beta  = graphics::edgeFunction(face[2], face[0], pixelpoint) / area;
    double gamma = graphics::edgeFunction(face[0], face[1], pixelpoint) / area;

    double pixelZ = face[0].z * alpha + face[1].z * beta + face[2].z * gamma;

    double intensity = max(0.0, dot(faceNormal, lightDirection));

    zPixel pixel = { pixelZ, Colour{1.0, 1.0, 1.0} *intensity };

    return pixel;
}

static void graphics::topTriangleRasterisation(vector<point3D> face, ZBuffer& zBuffer, const vector3D& faceNormal, const vector3D& lightDirection) {
    if (isEqual( face[0].y - face[1].y, 0.0))
        return;

    double Gradient0 = (face[0].x - face[1].x) / (face[0].y - face[1].y);
    double Gradient1 = (face[0].x - face[2].x) / (face[0].y - face[2].y);

    double positiveGradient = (Gradient0 > Gradient1) ? Gradient0 : Gradient1;
    double negativeGradient = (Gradient0 < Gradient1) ? Gradient0 : Gradient1;


    double zGradient = (face[0].z - face[2].z) / (face[0].y - face[2].y);

    face[2] = { face[0].x - Gradient1 * (face[0].y - face[1].y), face[1].y, face[0].z - zGradient * (face[0].y - face[1].y) };

    for (int y = (int)std::ceil(face[0].y); y >= (int)std::ceil(face[1].y - Epsilon); y--) {
        for (int x = (int)std::round(face[0].x - positiveGradient * (face[0].y - y)); x < (int)std::round(face[0].x - negativeGradient * (face[0].y - y)); x++) {
            assert(x > 0 || x <= zBuffer.size.x || y > 0 || y <= zBuffer.size.y);

            zPixel& pixel = interpolatePixel(x, y, face, faceNormal, lightDirection);
            
            zBuffer[x][y] = (pixel.z < zBuffer[x][y].z) ? pixel : zBuffer[x][y];
        }
    }
}

static void graphics::bottomTriangleRasterisation(vector<point3D> face, ZBuffer& zBuffer, const vector3D& faceNormal, const vector3D& lightDirection) {
    if (isEqual(face[1].y - face[2].y, 0))
        return;

    double Gradient0 = (face[1].x - face[2].x) / (face[1].y - face[2].y);
    double Gradient1 = (face[0].x - face[2].x) / (face[0].y - face[2].y);

    double positiveGradient = (Gradient0 > Gradient1) ? Gradient0 : Gradient1;
    double negativeGradient = (Gradient0 < Gradient1) ? Gradient0 : Gradient1;


    double zGradient = (face[0].z - face[2].z) / (face[0].y - face[2].y);

    face[0] = { face[0].x - Gradient1 * (face[0].y - face[1].y), face[1].y, face[0].z - zGradient * (face[0].y - face[1].y) };

    for (int y = (int)std::floor(face[2].y); y <= (int)std::floor(face[1].y - Epsilon); y++) {
        for (int x = (int)std::round(face[2].x + negativeGradient * (y - face[2].y)); x < (int)std::round(face[2].x + positiveGradient * (y - face[2].y)); x++) {
            assert(x > 0 || x <= zBuffer.size.x || y > 0 || y <= zBuffer.size.y);

            zPixel& pixel = interpolatePixel(x, y, face, faceNormal, lightDirection);

            zBuffer[x][y] = (pixel.z < zBuffer[x][y].z) ? pixel : zBuffer[x][y];
        }
    }
}

graphics::Pixels graphics::ScanLineRasterise(const vector<MeshBase>& ScreenData, const dimensions outputSize, const Light* light) {
    ZBuffer zBuffer({ outputSize.x, outputSize.y });

    for (MeshBase mesh : ScreenData) {
        for (Face face : mesh.indices) {
            vector<point3D> vertices = { mesh.vertices[face.indices[0]], mesh.vertices[face.indices[1]], mesh.vertices[face.indices[2]] };

            std::sort(vertices.begin(), vertices.end(), [](const point3D& a, const point3D& b) {
                return a.y > b.y;
                });

            topTriangleRasterisation(vertices, zBuffer, face.normal, light->getDirection());
            bottomTriangleRasterisation(vertices, zBuffer, face.normal, light->getDirection());
        }
    }

    return zBuffer;
}

void graphics::Downscale(Pixels& pixels, size_t scalar) {
    Pixels pixelsDownscaled({ pixels.size.x / scalar, pixels.size.y / scalar });
    for (size_t x = 0; x < pixels.size.x / scalar; x++) {
        for (size_t y = 0; y < pixels.size.y / scalar; y++) {
            Colour averaged;

            for (size_t i = 0; i < scalar; i++) {
                for (size_t j = 0; j < scalar; j++) {
                    averaged = averaged + pixels[x * scalar + i][y * scalar + j] / (scalar * scalar);
                }
            }

            pixelsDownscaled[x][y] = averaged;
        }
    }

    pixels = pixelsDownscaled;
}