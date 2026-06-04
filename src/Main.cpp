#include "Scene.h"

int main() {
    InitEngine();

    Scene MyScene;

    std::function rotate = [](Object& self, float deltaTime) {
        self.rotate({ 1 * deltaTime, 0.0, 0.0 });
        };

    Object* camera = ObjectFactory::CreateCamera(MyScene, nullptr, point3D{ 0.0, 0.0, 0.0 }, Angle{ 0.0, 0.0, 0.0 }, dimensions{ 100, 100 }, 4);

    Camera* camera_ = dynamic_cast<Camera*>(camera);

    Object* light = ObjectFactory::CreateDirectionLight(MyScene, nullptr, Angle{ -0.5, -0.5, 0.0 });

    Object* MyCube = ObjectFactory::CreateCube(MyScene, nullptr, point3D{ 0, 0, 7 }, 2.0, Angle{ 0.0, 0.0, 0.0 }, rotate);

    // Define key handlers
    {
        const double moveSpeed = 4;

        MyScene.keyDownHandlers['A'] = [MyCube, moveSpeed](double duration) mutable {
            if (duration == 0) {
                MyCube->velocity += vector3D{ -1, 0, 0 } *moveSpeed;
            }
            };

        MyScene.keyUpHandlers['A'] = [MyCube, moveSpeed](double duration) mutable {
            MyCube->velocity -= vector3D{ -1, 0, 0 } *moveSpeed;
            };

        MyScene.keyDownHandlers['D'] = [MyCube, moveSpeed](double duration) mutable {
            if (duration == 0) {
                MyCube->velocity += vector3D{ 1, 0, 0 } * moveSpeed;
            }
            };

        MyScene.keyUpHandlers['D'] = [MyCube, moveSpeed](double duration) mutable {
            MyCube->velocity -= vector3D{ 1, 0, 0 } *moveSpeed;
            };

        MyScene.keyDownHandlers['W'] = [MyCube, moveSpeed](double duration) mutable {
            if (duration == 0) {
                MyCube->velocity += vector3D{ 0, 1, 0 } *moveSpeed;
            }
            };

        MyScene.keyUpHandlers['W'] = [MyCube, moveSpeed](double duration) mutable {
            MyCube->velocity -= vector3D{ 0, 1, 0 } *moveSpeed;
            };

        MyScene.keyDownHandlers['S'] = [MyCube, moveSpeed](double duration) mutable {
            if (duration == 0) {
                MyCube->velocity += vector3D{ 0, -1, 0 } *moveSpeed;
            }
            };

        MyScene.keyUpHandlers['S'] = [MyCube, moveSpeed](double duration) mutable {
            MyCube->velocity -= vector3D{ 0, -1, 0 } *moveSpeed;
            };

        MyScene.keyDownHandlers['I'] = [camera, rotate](double duration) mutable {
            camera->rotate({ 0.1, 0.0, 0.0 });
            };

        MyScene.keyDownHandlers['K'] = [camera, rotate](double duration) mutable {
            camera->rotate({ -0.1, 0.0, 0.0 });
            };
    }

    GameLoop(MyScene);
}