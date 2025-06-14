#include <stdio.h> // for printing
//#include <math.h> // for sine, cosine and power
#include <string.h> // for sizeof()
#include <time.h> // for benchmarking
#include <fixp.h>
#include <luts.h>
#include <triad.h>
#include <vector.h>

Matrix3x3 triad(Vector3D B1, Vector3D g1, Vector3D B2, Vector3D g2){
	// Normalisér alle vektorerne
	B1.x = FIX16_DIV(B1.x, norm(B1));
	B1.y = FIX16_DIV(B1.y, norm(B1));
	B1.z = FIX16_DIV(B1.z, norm(B1));

	g1.x = FIX16_DIV(g1.x, norm(g1));
	g1.y = FIX16_DIV(g1.y, norm(g1));
	g1.z = FIX16_DIV(g1.z, norm(g1));

	B2.x = FIX16_DIV(B2.x, norm(B2));
	B2.y = FIX16_DIV(B2.y, norm(B2));
	B2.z = FIX16_DIV(B2.z, norm(B2));

	g2.x = FIX16_DIV(g2.x, norm(g2));
	g2.y = FIX16_DIV(g1.y, norm(g2));
	g2.z = FIX16_DIV(g1.z, norm(g2));

	Vector3D t1;
	Vector3D t2;
	Vector3D t3;

	Vector3D u1;
	Vector3D u2;
	Vector3D u3;

	t1 = B1;
	u1 = B2;

	Vector3D B1xg1 = cross(B1, g1);

	t2.x = FIX16_DIV(B1xg1.x, norm(B1xg1));
	t2.y = FIX16_DIV(B1xg1.y, norm(B1xg1));
	t2.z = FIX16_DIV(B1xg1.z, norm(B1xg1));
	t3 = cross(t1, t2);

	Vector3D B2xg2 = cross(B2, g2);

	u2.x = FIX16_DIV(B2xg2.x, norm(B2xg2));
	u2.y = FIX16_DIV(B2xg2.y, norm(B2xg2));
	u2.z = FIX16_DIV(B2xg2.z, norm(B2xg2));
	u3 = cross(u1, u2);

	Matrix3x3 U = create_matrix_from_vectors(u1, u2, u3);
	Matrix3x3 T = create_matrix_from_vectors(t1,t2,t3);

	Matrix3x3 C;
	Matrix3x3 Tt = transpose(T);

	C = MMult(U,Tt);

	return C;
}
