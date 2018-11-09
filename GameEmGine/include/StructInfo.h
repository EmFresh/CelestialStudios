#pragma once
#include <GL/glew.h>
#include <string>
#include <cstdio>

struct Coord2D
{
	float x = 0, y = 0;

	float distance()
	{
		return sqrt(x * x + y * y);
	}

	static float distance(Coord2D v1, Coord2D v2)
	{
		Coord2D v3 = v2 - v1;
		return sqrt(v3.x*v3.x + v3.y*v3.y);
	}

	Coord2D normal()
	{
		return *this / distance();
	}

	float& operator[](int index)
	{
		float *error = nullptr;
		switch (index)
		{
		case 0:
			return static_cast<float&>(x);
		case 1:
			return static_cast<float&>(y);
		}
		return *error;
	}

	Coord2D operator+(Coord2D coord)
	{
		return { x + coord.x, y + coord.y };
	}
	Coord2D operator-(Coord2D coord)
	{
		return { x - coord.x, y - coord.y };
	}
	Coord2D operator/(Coord2D coord)
	{
		return { x / coord.x,y / coord.y };
	}
	Coord2D operator/(float coord)
	{
		return { x / coord,y / coord };
	}
	void operator-=(Coord2D coord)
	{
		x -= coord.x;
		y -= coord.y;
	}
	void operator+=(Coord2D coord)
	{
		x += coord.x;
		y += coord.y;
	}
	void operator/=(Coord2D coord)
	{
		x /= coord.x, y /= coord.y;
	}
	void operator/=(float coord)
	{
		x /= coord, y /= coord;
	}
};

struct Coord3D
{
	float x = 0.0f, y = 0.0f, z = 0.0f;

	Coord3D()
	{}

	static float distance(Coord3D v1, Coord3D v2)
	{
		Coord3D v3 = v2 - v1;
		return v3.distance();
	}

	float distance()
	{
		return sqrt(x * x + y * y + z * z);
	}

	Coord3D normal()
	{
		return *this / distance();
	}

	Coord3D(Coord2D coord)
	{
		x = coord.x;
		y = coord.y;
	}

	Coord3D(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Coord3D(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void set(Coord2D coord)
	{
		x = coord.x;
		y = coord.y;
	}

	void set(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void normalize()
	{
		float norm = sqrt(x * x + y * y + z * z);
		x *= norm;
		y *= norm;
		z *= norm;
	}

	float& operator[] (int index)
	{
		float* error = nullptr;
		switch (index)
		{
		case 0:
			return const_cast<float&>(x);
		case 1:
			return const_cast<float&>(y);
		case 2:
			return const_cast<float&>(z);
		}
		return *error;
	}

	Coord3D operator+(Coord3D coord)
	{
		return { x + coord.x, y + coord.y, z + coord.z };
	}

	Coord3D operator-(Coord3D coord)
	{
		return { x - coord.x, y - coord.y, z + coord.z };
	}

	Coord3D operator*(Coord3D coord)
	{
		return { x * coord.x, y * coord.y, z * coord.z };
	}

	Coord3D operator/(Coord3D coord)
	{
		return { x / coord.x,y / coord.y,z / coord.z };
	}
	Coord3D operator/(float coord)
	{
		return { x / coord,y / coord,z / coord };
	}


	void operator-=(Coord3D coord)
	{
		x -= coord.x;
		y -= coord.y;
		z -= coord.z;
	}

	void operator+=(Coord3D coord)
	{
		x += coord.x;
		y += coord.y;
		z += coord.z;
	}

	void operator*=(Coord3D coord)
	{
		x *= coord.x;
		y *= coord.y;
		z *= coord.z;
	}

	void operator*=(float coord)
	{
		x *= coord;
		y *= coord;
		z *= coord;
	}
};

struct Size2D
{
	float width = 0, height = 0;
	float& operator[](int index)
	{
		float *error = nullptr;
		switch (index)
		{
		case 0:
			return static_cast<float&>(width);
		case 1:
			return static_cast<float&>(height);
		}
		return *error;
	}
};

struct Size3D
{
	float width = 0, height = 0, depth = 0;

	Size3D()
	{}

	Size3D(Size2D size)
	{
		width = size.width;
		height = size.height;
	}

	Size3D(float w, float h, float d)
	{
		this->width = w;
		this->height = h;
		this->depth = d;
	}

	Size3D(float w, float h)
	{
		this->width = w;
		this->height = h;
	}

	void set(Size2D size)
	{
		width = size.width;
		height = size.height;
	}

	void set(float w, float h, float d)
	{
		this->width = w;
		this->height = h;
		this->depth = d;
	}

	void set(float w, float h)
	{
		this->width = w;
		this->height = h;
	}

	float& operator[] (int index)
	{
		float* error = nullptr;
		switch (index)
		{
		case 0:
			return const_cast<float&>(width);
		case 1:
			return const_cast<float&>(height);
		case 2:
			return const_cast<float&>(depth);
		}
		return *error;
	}
};

struct vboInfo3D
{
	Coord3D pos;
	Size3D size;
};

struct ColourRGBA
{
	GLubyte r, g, b, a;

	ColourRGBA() :r(255), g(255), b(255), a(255)
	{}

	ColourRGBA(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255) :r(r), g(g), b(b), a(a)
	{}

	void set(ColourRGBA rgba)
	{
		set(
			rgba.r,
			rgba.g,
			rgba.b,
			rgba.a);
	}

	void set(GLubyte r, GLubyte g, GLubyte b)
	{
		this[0][0] = r;
		this[0][1] = g;
		this[0][2] = b;
	}

	void set(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
	{
		this[0][0] = r;
		this[0][1] = g;
		this[0][2] = b;
		this[0][3] = a;
	}

	ColourRGBA operator*(ColourRGBA rgba)
	{
		return ColourRGBA{
			GLubyte((int)r * (int)rgba.r / 255),
			GLubyte((int)g * (int)rgba.g / 255),
			GLubyte((int)b * (int)rgba.b / 255),
			GLubyte((int)a * (int)rgba.a / 255) };
	}

	void operator*=(ColourRGBA rgba)
	{
		*this = *this * rgba;
	}

	GLubyte& operator[](int index)
	{
		GLubyte *error = nullptr;
		switch (index)
		{
		case 0:
			return static_cast<GLubyte&>(r);
		case 1:
			return static_cast<GLubyte&>(g);
		case 2:
			return static_cast<GLubyte&>(b);
		case 3:
			return static_cast<GLubyte&>(a);
		}
		return *error;
	}
};

struct UV
{
	float u = 0, v = 0, w = 0;
	void set(float u, float v, float w)
	{
		this->u = u;
		this->v = v;
		this->w = w;
	}

	void set(float u, float v)
	{
		this->u = u;
		this->v = v;
	}

	float& operator[](int index)
	{
		switch (index)
		{
		case 0:
			return static_cast<float&>(u);
		case 1:
			return static_cast<float&>(v);
		case 2:
			return static_cast<float&>(w);
		}
		float *error = nullptr;
		return *error;
	}
};

struct VboInfo2D
{
	VboInfo2D(Coord2D c = { 0,0 }, Size2D s = { 0,0 })
	{
		position = c;
		size = s;
	}

	Coord2D position;
	Size2D size;
protected:
	float _angle;
};

struct Vertex2D
{
	Coord2D coord;
	ColourRGBA	colour;
	UV uv;

	void setCoord2D(float x, float y)
	{
		coord.x = x;
		coord.y = y;
	}
	//uses the 0-255 representation instead of 0-1
	void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255)
	{
		colour.r = r;
		colour.g = g;
		colour.b = b;
		colour.a = a;
	}
	//sets uv
	void setUV(float u, float v)
	{
		uv.u = u;
		uv.v = v;
	}

	void print()
	{
		printf("Coord2D: (%f, %f)\n", coord.x, coord.y);
		printf("Colour : (%d, %d, %d, %d)\n", colour.r, colour.g, colour.b, colour.a);
		printf("UV     : (%f, %f)\n\n", uv.u, uv.v);
	}
};

struct Vertex3D
{
	Coord3D coord, norm;
	ColourRGBA	colour;
	UV uv;

	void setCoord(float x, float y, float z)
	{
		coord.x = x;
		coord.y = y;
		coord.z = z;
	}
	//uses the 0-255 representation instead of 0-1
	void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255)
	{
		colour.r = r;
		colour.g = g;
		colour.b = b;
		colour.a = a;
	}
	//sets uv
	void setUV(float u, float v)
	{
		uv.u = u;
		uv.v = v;
	}

	void setNorm(float x, float y, float z)
	{
		norm.x = x;
		norm.y = y;
		norm.z = z;
	}

	void print()
	{
		printf("Coord3D: (%f, %f, %f)\n", coord.x, coord.y, coord.z);
		printf("Colour : (%d, %d, %d, %d)\n", colour.r, colour.g, colour.b, colour.a);
		printf("UV     : (%f, %f)\n\n", uv.u, uv.v);
	}
};

struct WindowInfo
{
	std::string *title = new std::string;
	Size3D  *size = new Size3D;
	Coord2D *position = new Coord2D;
	int monitor;
	void print()
	{
		printf("Title    : %s\n\n", title->c_str());
		printf("Position : (%f, %f)\n", position->x, position->y);
		printf("Size     : (%.0f, %.0f, %.0f)\n", size->width, size->height, size->depth);
		printf("Monitor  : %d\n\n", monitor);
	}
};