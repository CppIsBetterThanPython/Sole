#pragma once
#include <memory>
#include <chrono>
#include <thread>

#include "Graphics.h"

class Scene {
public:
    vector<std::unique_ptr<Object>> objects;

    const int FrameRate = 60;

    enum class keyStates { UP, DOWN };

    keyStates toKeyState(bool isDown) {
        return isDown ? keyStates::DOWN : keyStates::UP;
    }

    struct keyState {
        keyStates state;
        double duration;
    };

    std::unordered_map<int, std::function<void(double duration)>> keyUpHandlers;
    std::unordered_map<int, std::function<void(double duration)>> keyDownHandlers;

    std::unordered_map<int, keyState> previousKeyState;
    std::unordered_map<int, keyState> currentKeyState;

    void PollInput(double deltaTime) {
        for (auto& [key, _] : keyDownHandlers) {
            SHORT state = GetAsyncKeyState(key);
            bool isDown = (state & 0x8000) != false;
            bool wasDown = previousKeyState[key].state == keyStates::DOWN;

            if (wasDown != isDown) {
                previousKeyState[key].duration = currentKeyState[key].duration;
                currentKeyState[key].duration = 0;
                previousKeyState[key].state = toKeyState(isDown);
            }
            else {
                currentKeyState[key].duration += deltaTime;
            }

            if (isDown && keyDownHandlers.count(key)) {
                keyDownHandlers[key](currentKeyState[key].duration);
            }
            else if (wasDown && !isDown && keyUpHandlers.count(key)) {
                keyUpHandlers[key](previousKeyState[key].duration);
            }
        }
    }

    void Update(float deltaTime) {
        PollInput(deltaTime);
        for (const auto& object : objects) {
            object.get()->Update(deltaTime);
        }
    }

    void Render() {
        std::vector<Mesh*> meshObjects;
        vector<RenderMeshBase> meshData;
        Camera* camera{};
        Light* light{};

        for (const auto& object : objects) {
            if (auto camera_ = dynamic_cast<Camera*>(object.get())) {
                camera = camera_;
                continue;
            }
            if (auto light_ = dynamic_cast<Light*>(object.get())) {
                light = light_;
                continue;
            }
            if (auto mesh = dynamic_cast<Mesh*>(object.get())) {
                meshObjects.push_back(mesh);
                mesh->findFaceNormals();
                meshData.push_back(mesh->getWorldViewData());
            }
        }

        if (!camera) {
            throw std::runtime_error("No camera in scene!");
        }

        vector<RenderMeshBase> clipData = graphics::TransformVertices(meshData, camera->getPerspectiveMatrix() * camera->getViewMatrix());

        clipData = graphics::ClipFaces(clipData, camera);

        vector<RenderMeshBase> NDCData = graphics::PerspectiveDivide(clipData);

        vector<RenderMeshBase> ScreenData = graphics::TransformVertices(NDCData, camera->getScreenMatrix());

        vector<MeshBase> ScreenDataCartesian = graphics::ConvertToCartesian(ScreenData);

        graphics::Pixels pixels = graphics::ScanLineRasterise(ScreenDataCartesian, camera->viewPortSize * camera->downscaling, light);

        graphics::Downscale(pixels, camera->downscaling);

        graphics::PrintToConsoleBlock(pixels);
    }
};

namespace ObjectFactory {

    Object* CreateCube(Scene& scene, Object* parent, point3D origin, double sideLength,
        Angle orientation = { 0.0, 0.0, 0.0 }, std::function<void(Object&, float)> onUpdate = nullptr);

    Object* CreateSphere(Scene& scene, Object* parent, point3D origin, double radius, size_t stacks = 12, int slices = 24,
        Angle orientation = { 0.0, 0.0, 0.0 }, std::function<void(Object&, float)> onUpdate = nullptr);

    Object* CreatePlane(Scene& scene, Object* parent, point3D origin, double width, double height,
        Angle orientation = { 0.0, 0.0, 0.0 }, std::function<void(Object&, float)> onUpdate = nullptr);

    Object* CreateDirectionLight(Scene& scene, Object* parent, Angle orientation, double intensity = 1.0, Colour colour = { 255, 255, 255 }, std::function<void(Object&, float)> onUpdate = {});

    Object* CreateCamera(Scene& scene, Object* parent, point3D position, Angle orientation, dimensions viewPortSize, size_t downscaling = 1, double fov = 1.35, double nearClipPlane = 0.1, double farClipPlane = 1000.0, std::function<void(Object&, float)> onUpdate = {});
};

inline void GameLoop(Scene& firstScene) {
    static Scene& currentScene = firstScene;

    using clock = std::chrono::steady_clock;
    auto lastTime = clock::now();

    while (true) {
        auto now = clock::now();
        std::chrono::duration<float> elapsed = now - lastTime;
        float deltaTime = elapsed.count();
        lastTime = now;

        currentScene.Update(deltaTime);
        currentScene.Render();

        // 3. Control frame rate (optional, e.g., ~60 FPS)
        if (1000/currentScene.FrameRate - deltaTime >= 0)
            std::this_thread::sleep_for(std::chrono::milliseconds( static_cast<int>(1000 / currentScene.FrameRate - deltaTime) ));
    }
}

inline void InitEngine() {
    EnableVirtualTerminal();
    EnableUnicodeOutput();
}