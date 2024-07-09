#ifndef QUAT_H_
#define QUAT_H_

// I decided to implement quaternions myself to understand them better, instead of using
// glm::quat. For understanding and implementation I used the following ressources:
// Game Engine Architecture Book
// 3B1B quaternion intuition & explanation: https://eater.net/quaternions
// G-Engine Quaternion post: http://clarkkromenaker.com/post/gengine-09-quaternions/

// Class will be used for rotations, so _theoretically_ only unit quaternions will be 
// needed. Because of this the class will focuns on these.

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

class Quat {
public:
	// [x, y, z, w] = [qv, qs]
	// Complex vector
	float x;
	float y;
	float z;
	// Real scalar
	float w;

private:


public:
	// Constructors
	Quat() : x(0), y(0), z(0), w(1) {}
	Quat(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Quat(glm::vec3 v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {}
	Quat(Quat& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}

	// Comparisons
	bool operator==(const Quat& q) const;
	bool operator!=(const Quat& q) const;

	// Addition and substraction
	Quat operator+(const Quat& q) const;
	Quat operator+=(const Quat& q);
	Quat operator-(const Quat& q) const;
	Quat operator-=(const Quat& q);
	Quat operator-() const;

	// Multiplication (scalar then quaternion)
	Quat operator*(float scalar) const;
	Quat operator*=(float scalar);
	Quat operator*(Quat q) const;
	Quat operator*=(Quat q);

	// Length
	float getLength() const;
	void normalize();

	// Conjugation
	// With unit quaternions the inverse of the quaternion is equal to its conjugate
	void conjugate();

	// Rotation
	// Rotates the vector with the quaternion (twice the angle and same axis)
	glm::vec3 rotate(const glm::vec3& vec) const;

};

std::ostream& operator<<(std::ostream& os, const Quat& q);

#endif QUAT_H_