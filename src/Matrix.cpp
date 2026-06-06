#include "Math.h"

Matrix::Matrix(dimensions size) : size(size) {
    matrix = vector<vector<double>>(size.y, vector<double>(size.x, 0));
}

Matrix::Matrix(vector<vector<double>> matrix) : matrix(matrix) {
    size = dimensions(matrix.begin()->size(), matrix.size());
}

Matrix::Matrix(std::initializer_list<vector<double>> matrix) : matrix(matrix) {
    size = dimensions(matrix.begin()->size(), matrix.size());
}

// Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

Matrix Matrix::operator *(const Matrix& rightMatrix) const {
    const Matrix* leftMatrix = this;
    Matrix productMatrix = Matrix(dimensions(rightMatrix.size.x, leftMatrix->size.y));

    if (leftMatrix->size.x != rightMatrix.size.y) {
        throw std::invalid_argument("Matrix multiplication error: column count of the first matrix must equal row count of the second matrix.");
    }

    size_t sharedDimension = rightMatrix.size.y;

    for (size_t y = 0; y < leftMatrix->size.y; y++) {
        for (size_t x = 0; x < rightMatrix.size.x; x++) {
            for (size_t i = 0; i < sharedDimension; i++) {
                productMatrix[y][x] += leftMatrix->matrix[y][i] * rightMatrix.matrix[i][x];
            }
        }
    }

    return productMatrix;
}

Matrix Matrix::operator +(const Matrix& rightMatrix) const {
    const Matrix* leftMatrix = this;
    Matrix sumMatrix = Matrix(leftMatrix->size);

    if (leftMatrix->size != rightMatrix.size) {
        throw std::invalid_argument("Matrix addition error: both matrices must be the same size.");
    }

    for (size_t x = 0; x < sumMatrix.size.x; x++) {
        for (size_t y = 0; y < sumMatrix.size.y; y++) {
            sumMatrix[y][x] = leftMatrix->matrix[y][x] + rightMatrix.matrix[y][x];
        }
    }

    return sumMatrix;
}

Matrix Matrix::operator -(const Matrix& rightMatrix) const {
    const Matrix* leftMatrix = this;
    Matrix sumMatrix = Matrix(leftMatrix->size);

    if (leftMatrix->size != rightMatrix.size) {
        throw std::invalid_argument("Matrix addition error: both matrices must be the same size.");
    }

    for (size_t x = 0; x < sumMatrix.size.x; x++) {
        for (size_t y = 0; y < sumMatrix.size.y; y++) {
            sumMatrix[y][x] = leftMatrix->matrix[y][x] - rightMatrix.matrix[y][x];
        }
    }

    return sumMatrix;
}

Matrix Matrix::operator *(const double& scalar) const {
    const Matrix* baseMatrix = this;
    Matrix scaledMatrix = Matrix(baseMatrix->size);

    for (size_t x = 0; x < scaledMatrix.size.x; x++) {
        for (size_t y = 0; y < scaledMatrix.size.y; y++) {
            scaledMatrix[y][x] = baseMatrix->matrix[y][x] * scalar;
        }
    }

    return scaledMatrix;
}

Matrix::operator std::string() const {
    std::string output;

    for (size_t y = 0; y < size.y; y++) {
        for (size_t x = 0; x < size.x; x++) {
            output += std::to_string(matrix[y][x]) + ", ";
        }
        output += "\n";
    }

    return output;
}

Matrix::operator vector3D() const {
    if (size.x != 1 || size.y != 4) {
        throw std::invalid_argument("Cannot convert matrix to point3D: expected 1x4 column vector.");
    }

    double x = matrix[0][0];
    double y = matrix[1][0];
    double z = matrix[2][0];

    return vector3D{ x, y, z };
}

Matrix::operator point3D() const {
    if (size.x != 1 || size.y != 4) {
        throw std::invalid_argument("Cannot convert matrix to point3D: expected 1x4 column vector.");
    }

    double x = matrix[0][0];
    double y = matrix[1][0];
    double z = matrix[2][0];

    if (size.y == 4 && matrix[3][0] != 0.0f && matrix[3][0] != 1.0f) {
        double w = matrix[3][0];
        x /= w;
        y /= w;
        z /= w;
    }

    return point3D{ x, y, z };
}

Matrix::operator point4D() const {
    if (size.x != 1 || size.y != 4) {
        throw std::invalid_argument("Cannot convert matrix to point3D: expected 1x4 column vector.");
    }

    double x = matrix[0][0];
    double y = matrix[1][0];
    double z = matrix[2][0];
    double w = matrix[3][0];

    return point4D{ x, y, z, w };
}

Matrix::Matrix(const vector3D& vector) {
    std::vector<std::vector<double>> vectorForm = { {vector.x},
                                          {vector.y},
                                          {vector.z},
                                          {0} };

    *this = Matrix(vectorForm);
}

Matrix::Matrix(const point3D& point) {
    vector<vector<double>> vectorForm = { {point.x},
                                          {point.y},
                                          {point.z},
                                          {1} };

    *this = Matrix(vectorForm);
}

Matrix::Matrix(const point4D& point) {
    vector<vector<double>> vectorForm = { {point.x},
                                          {point.y},
                                          {point.z},
                                          {point.w} };

    *this = Matrix(vectorForm);
}

// Transform Matrices
////////////////////////////////////////////////////////////////////////////////////////////////////

Matrix RotateZ(double roll) {
    Matrix rollMatrix = { {cos(roll), -sin(roll), 0, 0},
                          {sin(roll), cos(roll),  0, 0},
                          {0,         0,          1, 0},
                          {0,         0,          0, 1} };

    return rollMatrix;
}

Matrix RotateX(double pitch) {
    Matrix pitchMatrix = { {1, 0,          0,           0},
                           {0, cos(pitch), -sin(pitch), 0},
                           {0, sin(pitch), cos(pitch),  0},
                           {0, 0,          0,           1} };

    return pitchMatrix;
}

Matrix RotateY(double yaw) {
    Matrix yawMatrix = { {cos(yaw),  0, sin(yaw), 0},
                         {0,         1, 0,        0},
                         {-sin(yaw), 0, cos(yaw), 0},
                         {0,         0, 0,        1} };

    return yawMatrix;
}

Matrix Rotate(Angle angle) {
    return RotateZ(angle.roll) * RotateY(angle.yaw) * RotateX(angle.pitch);
}

Matrix Rotate(Quaternion quaternion) {
    double w = quaternion.w;
    double x = quaternion.x;
    double y = quaternion.y;
    double z = quaternion.z;
    return { {1 - 2 * (y * y + z * z), 2 * (x * y - w * z),     2 * (x * z + w * y),     0},
             {2 * (x * y + w * z),     1 - 2 * (x * x + z * z), 2 * (y * z - w * x),     0},
             {2 * (x * z - w * y),     2 * (y * z + w * x),     1 - 2 * (x * x + y * y), 0},
             {0,                       0,                       0,                       1} };
}

Matrix InverseRotate(Angle angle) {
    return RotateX(-angle.pitch) * RotateY(-angle.yaw) * RotateZ(-angle.roll);
}

Matrix Translate(point3D point) {
    Matrix translationMatrix = { {1, 0, 0, point.x},
                                 {0, 1, 0, point.y},
                                 {0, 0, 1, point.z},
                                 {0, 0, 0, 1      } };

    return translationMatrix;
}

Matrix Scale(vector3D scale) {
    Matrix scaleMatrix = { {scale.x, 0,       0,       0},
                           {0,       scale.y, 0,       0},
                           {0,       0,       scale.z, 0},
                           {0,       0,       0,       1} };

    return scaleMatrix;
}

const Matrix identityMatrix = { {1, 0, 0, 0},
                                {0, 1, 0, 0},
                                {0, 0, 1, 0},
                                {0, 0, 0, 1} };
