#include "../include/Quat.h"

const float EPSILON = 0.0001f;

// Comparisons
bool Quat::operator==(const Quat& q) const {
	return (
		glm::epsilonEqual(x, q.x, EPSILON) &&
		glm::epsilonEqual(y, q.y, EPSILON) &&
		glm::epsilonEqual(z, q.z, EPSILON) &&
		glm::epsilonEqual(w, q.w, EPSILON)
	);
}

bool Quat::operator!=(const Quat& q) const {
	return !(
		glm::epsilonEqual(x, q.x, EPSILON) &&
		glm::epsilonEqual(y, q.y, EPSILON) &&
		glm::epsilonEqual(z, q.z, EPSILON) &&
		glm::epsilonEqual(w, q.w, EPSILON)
	);
}


// Addition and substraction
Quat Quat::operator+(const Quat& q) const {
	return Quat(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quat Quat::operator+=(const Quat& q) {
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

Quat Quat::operator-(const Quat& q) const {
	return Quat(x - q.x, y - q.y, z - q.z, w - q.w);
}

Quat Quat::operator-=(const Quat& q) {
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

Quat Quat::operator-() const {
	return Quat(-x, -y, -z, -w);
}


// Multiplication (scalar then quaternion)
Quat Quat::operator*(float scalar) const {
	return Quat(scalar * x, scalar * y, scalar * z, scalar * w);
}

Quat Quat::operator*=(float scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

// From: https://github.com/kromenak/gengine/blob/master/Source/Math/Quaternion.cpp
Quat Quat::operator*(Quat q) const {
	return Quat(
		w * q.x + x * q.w + y * q.z - z * q.y,
		w * q.y + y * q.w + z * q.x - x * q.z,
		w * q.z + z * q.w + x * q.y - y * q.x,
		w * q.w - x * q.x - y * q.y - z * q.z
	);
}

Quat Quat::operator*=(Quat q) {
	float newX = w * q.x + x * q.w + y * q.z - z * q.y;
	float newY = w * q.y + y * q.w + z * q.x - x * q.z;
	float newZ = w * q.z + z * q.w + x * q.y - y * q.x;
	float newW = w * q.w - x * q.x - y * q.y - z * q.z;
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
	return *this;
}


// Length
float Quat::getLength() const {
	return sqrt(x * x + y * y + z * z + w * w);
}

void Quat::normalize() {
	float len = this->getLength();
	if (glm::epsilonEqual(len, 0.0f, EPSILON)) {
		x = y = z = w = 0.0f;
	}
	else {
		x /= len;
		y /= len;
		z /= len;
		w /= len;
	}
}

// Conjugation
void Quat::conjugate() {
	x = -x;
	y = -y;
	z = -z;
}


// Rotation
glm::vec3 Quat::rotate(const glm::vec3& vec) const {
	float c1 = 2.0f * (x * vec.x + y * vec.y + z * vec.z);
	// This might be bad, I should check up on it
	float c2 = w * w - x * x - y * y - z * z;
	float c3 = 2 * w;

	return glm::vec3(
		c1 * x + c2 * vec.x + c3 * (y * vec.z - z * vec.y),
		c1 * y + c2 * vec.y + c3 * (z * vec.x - x * vec.z),
		c1 * z + c2 * vec.z + c3 * (x * vec.y - y * vec.x)
	);
}


// Conversions
// According to https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
glm::mat4 Quat::toMat() {
	// m00 m01 m02
	// m10 m11 m12
	// m20 m21 m22

	float m00 = 1 - 2.0f * y * y - 2.0f * z * z;
	float m01 = 2.0f * x * y + 2.0f * z * w;
	float m02 = 2.0f * x * z - 2.0f * y * w;

	float m10 = 2.0f * x * y - 2.0f * z * w;
	float m11 = 1 - 2.0f * x * x - 2.0f * z * z;
	float m12 = 2.0f * y * z + 2.0f * x * w;

	float m20 = 2.0f * x * z + 2.0f * y * w;
	float m21 = 2.0f * y * z - 2.0f * x * w;
	float m22 = 1 - 2.0 * x * x - 2.0f * y * y;

	return glm::mat4(
		m00, m01, m02, 0.0f,
		m10, m11, m12, 0.0f,
		m20, m21, m22, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

// Based on: https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js#L294
// Also on: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
void Quat::setFromMat(glm::mat4 mat) {
	float trace = mat[0][0] + mat[1][1] + mat[2][2];

	if (trace > 0.0f) {
		float scale = 0.5f / sqrt(1.0f + trace);

		w = 0.25f / scale;
		x = (mat[2][1] - mat[1][2]) * scale;
		y = (mat[0][2] - mat[2][0]) * scale;
		z = (mat[1][0] - mat[0][1]) * scale;
	} // elses are checking for the max value along the diagonal
	else if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
		float scale = 2.0f * sqrt(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);

		w = (mat[2][1] - mat[1][2]) / scale;
		x = 0.25f * scale;
		y = (mat[0][1] + mat[1][0]) / scale;
		z = (mat[0][2] + mat[2][0]) / scale;
	}
	else if (mat[1][1] > mat[2][2]) {
		float scale = 2.0f * sqrt(1.0f - mat[0][0] + mat[1][1] - mat[2][2]);

		w = (mat[0][2] - mat[2][0]) / scale;
		x = (mat[0][1] + mat[1][0]) / scale;
		y = 0.25f * scale;
		z = (mat[1][2] + mat[2][1]) / scale;
	}
	else {
		float scale = 2.0f * sqrt(1.0f - mat[0][0] - mat[1][1] + mat[2][2]);

		w = (mat[1][0] - mat[0][1]) / scale;
		x = (mat[0][2] + mat[2][0]) / scale;
		y = (mat[1][2] + mat[2][1]) / scale;
		z = 0.25f * scale;
	}

	this->normalize();
}

// https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js
// Got here from: https://www.andre-gaschler.com/rotationconverter/
glm::vec3 Quat::toEuler() {
	// Three.js version:
	glm::vec3 res;

	// Is clamp between -1, 1 needed?
	https://github.com/mrdoob/three.js/blob/dev/src/math/Euler.js#L105
	res.y = asinf(2.0f * (x * z + w * y));

	if (2.0f * (x * z + w * y) < 1.0f - EPSILON) {
		res.x = atan2f(-2.0f * (y * z - w * x), 1.0f - 2.0f * (x * x + y * y));
		res.z = atan2f(-2.0f * (x * y - w * z), 1.0f - 2.0f * (y * y + z * z));
	}
	else {
		res.x = atan2f(2.0f * (y * z + w * x), 1.0f - 2.0f * (x * x + z * z));
		res.z = 0;
	}

	return res;
}

// https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js
// Got here from: https://www.andre-gaschler.com/rotationconverter/
void Quat::setFromEuler(glm::vec3 eul) {
	float cosX = glm::cos(eul.x * 0.5f);
	float sinX = glm::sin(eul.x * 0.5f);
	float cosY = glm::cos(eul.y * 0.5f);
	float sinY = glm::sin(eul.y * 0.5f);
	float cosZ = glm::cos(eul.z * 0.5f);
	float sinZ = glm::sin(eul.z * 0.5f);

	x = sinX * cosY * cosZ + cosX * sinY * sinZ;
	y = cosX * sinY * cosZ - sinX * cosY * sinZ;
	z = cosX * cosY * sinZ + sinX * sinY * cosZ;
	w = cosX * cosY * cosZ - sinX * sinY * sinZ;

	this->normalize();
}

std::ostream& operator<<(std::ostream& os, const Quat& q) {
	os << "x: " << q.x << ", y: " << q.y << ", z: " << q.z << ", w: " << q.w;
	return os;
}