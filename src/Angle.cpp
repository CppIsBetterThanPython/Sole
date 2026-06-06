#include "Math.h"

Quaternion::Quaternion(Angle angle) {
    angle = angle.Negate();
    double cy = cos(angle.roll * 0.5);
    double sy = sin(angle.roll * 0.5);
    double cp = cos(angle.yaw * 0.5);
    double sp = sin(angle.yaw * 0.5);
    double cr = cos(angle.pitch * 0.5);
    double sr = sin(angle.pitch * 0.5);

    w = cr * cp * cy + sr * sp * sy;
    x = sr * cp * cy - cr * sp * sy;
    y = cr * sp * cy + sr * cp * sy;
    z = cr * cp * sy - sr * sp * cy;
}

vector3D Quaternion::Normal() const {
    vector3D normal = { 0, 0, 1 };
    Matrix rotationMatrix = Rotate(*this);
    return static_cast<vector3D>(rotationMatrix * normal);
}

// Quaternion multiplication (combining rotations)
Quaternion Quaternion::operator*(const Quaternion& q) const {
    return {
        w * q.w - x * q.x - y * q.y - z * q.z,
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w
    };
}

// Convert back to Euler angles
Quaternion::operator Angle() const {
    // roll (x-axis rotation)
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    double roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double pitch;
    double sinp = 2 * (w * y - z * x);
    if (std::abs(sinp) >= 1)
        pitch = std::copysign(M_PI / 2, sinp);  // use 90 degrees if out of range
    else
        pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    double yaw = std::atan2(siny_cosp, cosy_cosp);

    return { yaw, pitch, roll };
}
