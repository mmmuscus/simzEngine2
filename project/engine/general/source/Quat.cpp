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

	if (glm::epsilonEqual(len, 1.0f, EPSILON))
		return;

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

Quat Quat::normalized() {
	Quat ret = *this;
	ret.normalize();
	return ret;
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
glm::mat4 Quat::toMat(Quat q) {
	// m00 m01 m02
	// m10 m11 m12
	// m20 m21 m22

	float xx2 = 2.0f * q.x * q.x;
	float yy2 = 2.0f * q.y * q.y;
	float zz2 = 2.0f * q.z * q.z;

	float xy2 = 2.0f * q.x * q.y;
	float xz2 = 2.0f * q.x * q.z;
	float xw2 = 2.0f * q.x * q.w;
	float yz2 = 2.0f * q.y * q.z;
	float yw2 = 2.0f * q.y * q.w;
	float zw2 = 2.0f * q.z * q.w;

	float m00 = 1.0f - yy2 - zz2;
	float m01 = xy2 + zw2;
	float m02 = xz2 - yw2;

	float m10 = xy2 - zw2;
	float m11 = 1.0f - xx2 - zz2;
	float m12 = yz2 + xw2;

	float m20 = xz2 + yw2;
	float m21 = yz2 - xw2;
	float m22 = 1.0f - xx2 - yy2;

	return glm::mat4(
		m00, m01, m02, 0.0f,
		m10, m11, m12, 0.0f,
		m20, m21, m22, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

// Based on: https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js#L294
// Also on: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
Quat Quat::fromMat(glm::mat4 mat) {
	Quat ret;

	float trace = mat[0][0] + mat[1][1] + mat[2][2];

	if (trace > 0.0f) {
		float scale = 0.5f / sqrt(1.0f + trace);

		ret.w = 0.25f / scale;
		ret.x = (mat[2][1] - mat[1][2]) * scale;
		ret.y = (mat[0][2] - mat[2][0]) * scale;
		ret.z = (mat[1][0] - mat[0][1]) * scale;
	} // elses are checking for the max value along the diagonal
	else if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
		float scale = 2.0f * sqrt(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);

		ret.w = (mat[2][1] - mat[1][2]) / scale;
		ret.x = 0.25f * scale;
		ret.y = (mat[0][1] + mat[1][0]) / scale;
		ret.z = (mat[0][2] + mat[2][0]) / scale;
	}
	else if (mat[1][1] > mat[2][2]) {
		float scale = 2.0f * sqrt(1.0f - mat[0][0] + mat[1][1] - mat[2][2]);

		ret.w = (mat[0][2] - mat[2][0]) / scale;
		ret.x = (mat[0][1] + mat[1][0]) / scale;
		ret.y = 0.25f * scale;
		ret.z = (mat[1][2] + mat[2][1]) / scale;
	}
	else {
		float scale = 2.0f * sqrt(1.0f - mat[0][0] - mat[1][1] + mat[2][2]);

		ret.w = (mat[1][0] - mat[0][1]) / scale;
		ret.x = (mat[0][2] + mat[2][0]) / scale;
		ret.y = (mat[1][2] + mat[2][1]) / scale;
		ret.z = 0.25f * scale;
	}

	ret.normalize();

	return ret;
}

// https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js
// Got here from: https://www.andre-gaschler.com/rotationconverter/
glm::vec3 Quat::toEuler(Quat q) {
	// Three.js version:
	glm::vec3 ret;

	float testValue = q.x * q.z + q.w * q.y;

	// Is clamp between -1, 1 needed? 
	// https://github.com/mrdoob/three.js/blob/dev/src/math/Euler.js#L105
	ret.y = asinf(2.0f * testValue);

	if (2.0f * testValue < 1.0f - EPSILON) {
		float yy = q.y * q.y;

		ret.x = atan2f(-2.0f * (q.y * q.z - q.w * q.x), 1.0f - 2.0f * (q.x * q.x + yy));
		ret.z = atan2f(-2.0f * (q.x * q.y - q.w * q.z), 1.0f - 2.0f * (yy + q.z * q.z));
	}
	else {
		ret.x = atan2f(2.0f * (q.y * q.z + q.w * q.x), 1.0f - 2.0f * (q.x * q.x + q.z * q.z));
		ret.z = 0;
	}

	return ret;
}

// https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js
// Got here from: https://www.andre-gaschler.com/rotationconverter/
Quat Quat::fromEuler(glm::vec3 eul) {
	Quat ret;

	float cosX = glm::cos(eul.x * 0.5f);
	float sinX = glm::sin(eul.x * 0.5f);
	float cosY = glm::cos(eul.y * 0.5f);
	float sinY = glm::sin(eul.y * 0.5f);
	float cosZ = glm::cos(eul.z * 0.5f);
	float sinZ = glm::sin(eul.z * 0.5f);

	ret.x = sinX * cosY * cosZ + cosX * sinY * sinZ;
	ret.y = cosX * sinY * cosZ - sinX * cosY * sinZ;
	ret.z = cosX * cosY * sinZ + sinX * sinY * cosZ;
	ret.w = cosX * cosY * cosZ - sinX * sinY * sinZ;

	ret.normalize();

	return ret;
}

std::ostream& operator<<(std::ostream& os, const Quat& q) {
	os << "x: " << q.x << ", y: " << q.y << ", z: " << q.z << ", w: " << q.w;
	return os;
}