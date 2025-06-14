#include <stdio.h> // for printing
#include <fixp.h>
#include <triad.h>
#include <vector.h>

Matrix3x3 triad(Vector3D B1, Vector3D g1, Vector3D B2, Vector3D g2){
	// Normalisér alle vektorerne
	int32_t nB1 = norm(B1);
	int32_t ng1 = norm(g1);
	int32_t nB2 = norm(B2);
	int32_t ng2 = norm(g2);

	B1.x = FIX16_DIV(B1.x, nB1);
	B1.y = FIX16_DIV(B1.y, nB1);
	B1.z = FIX16_DIV(B1.z, nB1);

	g1.x = FIX16_DIV(g1.x, ng1);
	g1.y = FIX16_DIV(g1.y, ng1);
	g1.z = FIX16_DIV(g1.z, ng1);

	B2.x = FIX16_DIV(B2.x, nB2);
	B2.y = FIX16_DIV(B2.y, nB2);
	B2.z = FIX16_DIV(B2.z, nB2);

	g2.x = FIX16_DIV(g2.x, ng2);
	g2.y = FIX16_DIV(g2.y, ng2);
	g2.z = FIX16_DIV(g2.z, ng2);

	Vector3D t1;
	Vector3D t2;
	Vector3D t3;

	Vector3D u1;
	Vector3D u2;
	Vector3D u3;

	t1 = B1;
	u1 = B2;

	Vector3D B1xg1 = cross(B1, g1);

	int32_t nB1g1 = norm(B1xg1);
	t2.x = FIX16_DIV(B1xg1.x, nB1g1);
	t2.y = FIX16_DIV(B1xg1.y, nB1g1);
	t2.z = FIX16_DIV(B1xg1.z, nB1g1);

	t3 = cross(t1, t2);

	Vector3D B2xg2 = cross(B2, g2);

	int32_t nB2g2 = norm(B2xg2);
	u2.x = FIX16_DIV(B2xg2.x, nB2g2);
	u2.y = FIX16_DIV(B2xg2.y, nB2g2);
	u2.z = FIX16_DIV(B2xg2.z, nB2g2);
	u3 = cross(u1, u2);

	Matrix3x3 U = create_matrix_from_vectors(u1, u2, u3);
	Matrix3x3 T = create_matrix_from_vectors(t1, t2, t3);

	Matrix3x3 C;
	Matrix3x3 Tt = transpose(T);

	C = MMult(U,Tt);

	return C;
}
