#pragma once
#include <cmath>

#pragma region FLOAT2
struct float2
{
	// Values
	float x;
	float y;

	// Functions
	inline float Magnitude() const
	{
		return sqrtf(x * x + y * y); 
	};
	inline float2 Normalize() const
	{
		float m = Magnitude();
		return { x / m, y / m };
	}
};

// Operators
std::ostream& operator<<(std::ostream& os, const float2& A)
{
	os << "[" << A.x << ", " << A.y << "]";
	return os;
}
inline float2 operator-(const float2& A) { return { -A.x, -A.y }; }
inline float2 operator+(const float2& A, const float2& B) { return { A.x + B.x, A.y + B.y }; }
inline float operator*(const float2& A, const float2& B) { return A.x * B.x + A.y * B.y; }
inline float2 operator*(const float& A, const float2& B) { return { A * B.x, A * B.y }; }

#pragma endregion // Struct, Functions and Operators

#pragma region FLOAT3
struct float3
{
	// Values
	float x;
	float y;
	float z;

	// Functions
	inline float Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z);
	};

	inline float Dot(const float3& B) const
	{
		return { x * B.x + y * B.y + z * B.z };
	}

	inline float3 Cross(const float3& B) const
	{
		return { y * B.z - z * B.y, z * B.x - x * B.z, x * B.y - y * B.x }; 
	}

	inline float3 Normalize()
	{
		float m = Magnitude();
		return { x / m, y / m, z / m };
	}
};

// Operators
std::ostream& operator<<(std::ostream& os, const float3& a)
{
	os << "[" << a.x << ", " << a.y << ", " << a.z << "]";
	return os;
}
inline float3 operator-(const float3& A) { return { -A.x, -A.y, -A.z }; }
inline float3 operator-(const float3& A, const float3& B) { return { A.x - B.x, A.y - B.y, A.z - B.z }; }
inline float3 operator*(const float& A, const float3& B) { return { A * B.x, A * B.y, A * B.z }; }
inline float3 operator+(const float3& A, const float3& B) { return { A.x + B.x, A.y + B.y, A.z + B.z }; }

#pragma endregion // Struct, Functions and Operators

#pragma region FLOAT4
struct float4
{
	union
	{
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float e[4];
	};
};

#pragma endregion // Struct, Functions and Operators

#pragma region MAT4
struct mat4
{
	union
	{
		float e[16];
		float4 c[4];
	};
};

// Operators
inline mat4 operator*(const mat4& A, const mat4& B)
{
	mat4 Res = {};
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			for (int i = 0; i < 4; ++i)
				Res.c[c].e[r] += A.c[i].e[r] * B.c[c].e[i];
	return Res;
}
inline mat4& operator*=(mat4& A, const mat4& B)
{
	A = A * B;
	return A;
}

#pragma endregion // Struct, Functions and Operators

#pragma region GENERAL
namespace Math
{
	// Values
	static const float TAU = 6.283185307179586476925286f;
	static const float PI = 3.14159265359f;

	// Rad <=> Deg
	inline float ToRadians(float Degrees) { return Degrees * Math::PI / 180.f; }
	inline float ToDegrees(float Radians) { return Radians * 180.f / Math::PI; }

	template<typename T>
	inline T Min(T A, T B) { return A < B ? A : B; }

	template<typename T>
	inline T Max(T A, T B) { return A > B ? A : B; }

	template<typename T>
	inline T Clamp(T X, T MinValue, T MaxValue) { return Min(Max(X, MinValue), MaxValue); }

	template<typename T>
	inline T Lerp(T X, T Y, float A) { return (1.f - A) * X + A * Y; }
}

#pragma endregion // Other thing

namespace Mat4
{
	inline mat4 Identity()
	{
		return
		{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline mat4 Translate(const float3& T)
	{
		return
		{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			T.x, T.y, T.z, 1.f,
		};
	}

	inline mat4 Scale(const float3& S)
	{
		return
		{
			S.x, 0.f, 0.f, 0.f,
			0.f, S.y, 0.f, 0.f,
			0.f, 0.f, S.z, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline mat4 RotateX(const float& C, const float& S)
	{
		return
		{
			1.f, 0.f, 0.f, 0.f,
			0.f,   C,   S, 0.f,
			0.f,  -S,   C, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline mat4 RotateX(const float& AngleRadians)
	{
		float C = cosf(AngleRadians);
		float S = sinf(AngleRadians);
		return Mat4::RotateX(C, S);
	}

	inline mat4 RotateY(float C, float S)
	{
		return
		{
			  C, 0.f,   S, 0.f,
			0.f, 1.f, 0.f, 0.f,
			 -S, 0.f,   C, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline mat4 RotateY(float AngleRadians)
	{
		float C = cosf(AngleRadians);
		float S = sinf(AngleRadians);
		return Mat4::RotateY(C, S);
	}

	inline mat4 RotateZ(float C, float S)
	{
		return
		{
			  C,   S, 0.f, 0.f,
			 -S,   C, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline mat4 RotateZ(float AngleRadians)
	{
		float C = cosf(AngleRadians);
		float S = sinf(AngleRadians);
		return Mat4::RotateZ(C, S);
	}

	inline mat4 Transpose(mat4 M)
	{
		return {
			M.c[0].e[0], M.c[1].e[0], M.c[2].e[0], M.c[3].e[0],
			M.c[0].e[1], M.c[1].e[1], M.c[2].e[1], M.c[3].e[1],
			M.c[0].e[2], M.c[1].e[2], M.c[2].e[2], M.c[3].e[2],
			M.c[0].e[3], M.c[1].e[3], M.c[2].e[3], M.c[3].e[3]
		};
	}

	inline mat4 Inverse(const mat4& M)
	{
		mat4 R;

		float S[6];
		S[0] = M.c[0].e[0] * M.c[1].e[1] - M.c[1].e[0] * M.c[0].e[1];
		S[1] = M.c[0].e[0] * M.c[1].e[2] - M.c[1].e[0] * M.c[0].e[2];
		S[2] = M.c[0].e[0] * M.c[1].e[3] - M.c[1].e[0] * M.c[0].e[3];
		S[3] = M.c[0].e[1] * M.c[1].e[2] - M.c[1].e[1] * M.c[0].e[2];
		S[4] = M.c[0].e[1] * M.c[1].e[3] - M.c[1].e[1] * M.c[0].e[3];
		S[5] = M.c[0].e[2] * M.c[1].e[3] - M.c[1].e[2] * M.c[0].e[3];

		float C[6];
		C[0] = M.c[2].e[0] * M.c[3].e[1] - M.c[3].e[0] * M.c[2].e[1];
		C[1] = M.c[2].e[0] * M.c[3].e[2] - M.c[3].e[0] * M.c[2].e[2];
		C[2] = M.c[2].e[0] * M.c[3].e[3] - M.c[3].e[0] * M.c[2].e[3];
		C[3] = M.c[2].e[1] * M.c[3].e[2] - M.c[3].e[1] * M.c[2].e[2];
		C[4] = M.c[2].e[1] * M.c[3].e[3] - M.c[3].e[1] * M.c[2].e[3];
		C[5] = M.c[2].e[2] * M.c[3].e[3] - M.c[3].e[2] * M.c[2].e[3];

		// Assuming it is invertible
		float InvDet = 1.0f / (S[0] * C[5] - S[1] * C[4] + S[2] * C[3] + S[3] * C[2] - S[4] * C[1] + S[5] * C[0]);

		R.c[0].e[0] = +(M.c[1].e[1] * C[5] - M.c[1].e[2] * C[4] + M.c[1].e[3] * C[3]) * InvDet;
		R.c[0].e[1] = -(M.c[0].e[1] * C[5] - M.c[0].e[2] * C[4] + M.c[0].e[3] * C[3]) * InvDet;
		R.c[0].e[2] = +(M.c[3].e[1] * S[5] - M.c[3].e[2] * S[4] + M.c[3].e[3] * S[3]) * InvDet;
		R.c[0].e[3] = -(M.c[2].e[1] * S[5] - M.c[2].e[2] * S[4] + M.c[2].e[3] * S[3]) * InvDet;

		R.c[1].e[0] = -(M.c[1].e[0] * C[5] - M.c[1].e[2] * C[2] + M.c[1].e[3] * C[1]) * InvDet;
		R.c[1].e[1] = +(M.c[0].e[0] * C[5] - M.c[0].e[2] * C[2] + M.c[0].e[3] * C[1]) * InvDet;
		R.c[1].e[2] = -(M.c[3].e[0] * S[5] - M.c[3].e[2] * S[2] + M.c[3].e[3] * S[1]) * InvDet;
		R.c[1].e[3] = +(M.c[2].e[0] * S[5] - M.c[2].e[2] * S[2] + M.c[2].e[3] * S[1]) * InvDet;

		R.c[2].e[0] = +(M.c[1].e[0] * C[4] - M.c[1].e[1] * C[2] + M.c[1].e[3] * C[0]) * InvDet;
		R.c[2].e[1] = -(M.c[0].e[0] * C[4] - M.c[0].e[1] * C[2] + M.c[0].e[3] * C[0]) * InvDet;
		R.c[2].e[2] = +(M.c[3].e[0] * S[4] - M.c[3].e[1] * S[2] + M.c[3].e[3] * S[0]) * InvDet;
		R.c[2].e[3] = -(M.c[2].e[0] * S[4] - M.c[2].e[1] * S[2] + M.c[2].e[3] * S[0]) * InvDet;

		R.c[3].e[0] = -(M.c[1].e[0] * C[3] - M.c[1].e[1] * C[1] + M.c[1].e[2] * C[0]) * InvDet;
		R.c[3].e[1] = +(M.c[0].e[0] * C[3] - M.c[0].e[1] * C[1] + M.c[0].e[2] * C[0]) * InvDet;
		R.c[3].e[2] = -(M.c[3].e[0] * S[3] - M.c[3].e[1] * S[1] + M.c[3].e[2] * S[0]) * InvDet;
		R.c[3].e[3] = +(M.c[2].e[0] * S[3] - M.c[2].e[1] * S[1] + M.c[2].e[2] * S[0]) * InvDet;

		return R;
	}

	inline mat4 Frustum(float Left, float Right, float Bottom, float Top, float Near, float Far)
	{
		return
		{
			(Near * 2.f) / (Right - Left),   0.f,                              0.f,                               0.f,
			0.f,                             (Near * 2.f) / (Top - Bottom),  0.f,                               0.f,
			(Right + Left) / (Right - Left), (Top + Bottom) / (Top - Bottom), -(Far + Near) / (Far - Near),      -1.f,
			0.f,                             0.f,                             -(Far * Near * 2.f) / (Far - Near), 0.f
		};
	}

	inline mat4 Perspective(float FovY, float Aspect, float Near, float Far)
	{
		float Top = Near * tanf(FovY / 2.f);
		float Right = Top * Aspect;
		return Mat4::Frustum(-Right, Right, -Top, Top, Near, Far);
	}

	inline mat4 LookAt(float3 Eye, float3 At, float3 Up)
	{
		float3 ZAxis = (At - Eye).Normalize();
		float3 XAxis = ZAxis.Cross(Up).Normalize();
		float3 YAxis = XAxis.Cross(ZAxis);

		return {
			XAxis.x, YAxis.x, -ZAxis.x, 0.f,
			XAxis.y, YAxis.y, -ZAxis.y, 0.f,
			XAxis.z, YAxis.z, -ZAxis.z, 0.f,
			-XAxis.Dot(Eye), -YAxis.Dot(Eye), ZAxis.Dot(Eye), 1.f
		};
	}
}