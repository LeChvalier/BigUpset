
#include <iostream>

#include "Maths.h"

// Define which tests do you want
#define FLOAT_2
#define FLOAT_3

int main()
{
	std::cout << "BigUpSet Math :" << std::endl;
	using namespace Math;

#ifdef FLOAT_2
	{
		std::cout << std::endl << "Float2 :" << std::endl;

		// Definition and Display
		float2 A = { 2.f, 3.f };
		std::cout << "A = " << A << std::endl;
		float2 B = { 4.f, 5.f };
		std::cout << "B = " << B << std::endl;

		//	Opposite
		std::cout << "-A = " << -A << std::endl;
		
		//	Addition
		std::cout << "A + B = " << A + B << std::endl;

		//	Products
		std::cout << "2.f * A = " << 2.f * A << std::endl;
		std::cout << "A * B = " << A * B << std::endl;

		//	Magnitude
		std::cout << "A.Magnitude() = " << A.Magnitude() << std::endl;

		// Normalized
		std::cout << "Normalize(A) = " << A.Normalize() << std::endl;
	}
#endif

#ifdef FLOAT_3
	{
		std::cout << std::endl << "Float3 :" << std::endl;

		// Definition and Display

		float3 A = { 1.f, 2.f, 3.f };
		std::cout << "A = " << A << std::endl;
		float3 B = { 5.f, 6.f, 7.f };
		std::cout << "B = " << B << std::endl;

		//	Magnitude
		std::cout << "A.Magnitude() = " << A.Magnitude() << std::endl;

		//	Opposite
		std::cout << "-A = " << -A << std::endl;

		//	Addition
		std::cout << "A + B = " << A + B << std::endl;

		//	Products
		std::cout << "2.f * A = " << 2.f * A << std::endl;
		
		std::cout << "Dot(A, B) = " << A.Dot(B) << std::endl;
		std::cout << "Cross(A, B) = " << A.Cross(B) << std::endl;

		// Normalized
		std::cout << "Normalize(A) = " << A.Normalize() << std::endl;
	}
#endif

	return 1;
}