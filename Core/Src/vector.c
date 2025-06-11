#include <stdint.h>
#include <math.h>
#include <vector.h>
#include <fixp.h>

//////////////////Vector////////////////////////////////////////////

int32_t dot(Vector3D a, Vector3D b) {
    return Mult(a.x, b.x) + Mult(a.y, b.y) + Mult(a.z, b.z);
}

Vector3D cross(Vector3D a, Vector3D b){
	Vector3D c;
	c.x = Mult(a.y,b.z) - Mult(a.z,b.y);
	c.y = Mult(a.z,b.x) - Mult(a.x,b.z);
	c.z = Mult(a.x,b.y) - Mult(a.y,b.z);
	return c;
}

Vector3D add_vector(Vector3D a, Vector3D b){
	Vector3D vector;
	vector.x = a.x + b.x;
	vector.y = a.y + b.y;
	vector.z = a.z + b.z;
	return vector;
}

Vector3D scale_vector(Vector3D a, int32_t k) {
	Vector3D vector;
	vector.x = Mult(a.x,k);
	vector.y = Mult(a.y,k);
	vector.z = Mult(a.y,k);
	return vector;
}

Vector3D VMult(Vector3D a, int32_t b){
	Vector3D c;
	c.x = Mult(a.x, b);
	c.y = Mult(a.y, b);
	c.z = Mult(a.z, b);
	return c;
}

Vector3D subtract_vector(Vector3D a, Vector3D b){
	return add_vector(a, scale_vector(b, convert(-1)));
}

int32_t norm(Vector3D a){ //find norm
	return sqrt(Mult(a.x, a.x) + Mult(a.y, a.y) + Mult(a.z, a.z));
}

int32_t cos_theta(Vector3D a, Vector3D b){ //cos_theta (later use??)
	return Div(dot(a,b), Mult(norm(a),norm(b)));
}

void printFixVector(Vector3D a) {
	printf("[ ");
	printFix(a.x);
	printf(", ");
	printFix(a.y);
	printf(", ");
	printFix(a.z);
	printf(" ]");
}

void printVector(Vector3D a) {
	printf("[ %i, %i, %i ]", (int)a.x, (int)a.y, (int)a.z);
}

//////////////////Matrix////////////////////////////////////////////

Matrix3x3 create_matrix(int32_t a, int32_t b, int32_t c,
                        int32_t d, int32_t e, int32_t f,
                        int32_t g, int32_t h, int32_t i) {
    Matrix3x3 matrix;
    matrix.x.x = a; matrix.x.y = d; matrix.x.z = g;
    matrix.y.x = b; matrix.y.y = e; matrix.y.z = h;
    matrix.z.x = c; matrix.z.y = f; matrix.z.z = i;
    return matrix;
}

Matrix3x3 create_matrix_from_vectors(Vector3D a, Vector3D b, Vector3D c) {
    Matrix3x3 matrix;
    matrix.x = a;
    matrix.y = b;
    matrix.z = c;
    return matrix;
}

Matrix3x3 transpose(Matrix3x3 a){
	Vector3D a_vect1;
	Vector3D a_vect2;
	Vector3D a_vect3;

	a_vect1.x = a.x.x;
	a_vect1.y = a.y.x;
	a_vect1.z = a.z.x;

	a_vect2.x = a.x.y;
	a_vect2.y = a.y.y;
	a_vect2.z = a.z.y;

	a_vect3.x = a.x.z;
	a_vect3.y = a.y.z;
	a_vect3.z = a.z.z;

	Matrix3x3 b; // b er brugt til at gemme data
	b.x = a_vect1;
	b.y = a_vect2;
	b.z = a_vect3;

	return b;
}

Matrix3x3 MMult(Matrix3x3 a, Matrix3x3 b){ //Matrix mult
	Vector3D b_vect1;
	Vector3D b_vect2;
	Vector3D b_vect3;
	b_vect1 = b.x;
	b_vect2 = b.y;
	b_vect3 = b.z;

	////a////
	a = transpose(a);
	Vector3D a_vect1;
	Vector3D a_vect2;
	Vector3D a_vect3;
	a_vect1 = a.x;
	a_vect2 = a.y;
	a_vect3 = a.z;

	//////calc
	Matrix3x3 svar;
	svar.x.x = dot(a_vect1, b_vect1);
	svar.y.x = dot(a_vect1, b_vect2);
	svar.z.x = dot(a_vect1, b_vect3);

	svar.x.y = dot(a_vect2, b_vect1);
	svar.y.y = dot(a_vect2, b_vect2);
	svar.z.y = dot(a_vect2, b_vect3);

	svar.x.z = dot(a_vect3, b_vect1);
	svar.y.z = dot(a_vect3, b_vect2);
	svar.z.z = dot(a_vect3, b_vect3);

	return svar;
}

Vector3D MVMult(Matrix3x3 a, Vector3D b){
	a = transpose(a);
	Vector3D a_vect1;
	Vector3D a_vect2;
	Vector3D a_vect3;
	a_vect1 = a.x;
	a_vect2 = a.y;
	a_vect3 = a.z;

	Vector3D c;
	c.x = dot(a_vect1, b);
	c.y = dot(a_vect2, b);
	c.z = dot(a_vect3, b);
	return c;
}

void print_matrix(Matrix3x3 a) {
    printf("(%d, %d, %d)\n", a.x.x, a.y.x, a.z.x);
    printf("(%d, %d, %d)\n", a.x.y, a.y.y, a.y.z);
    printf("(%d, %d, %d)\n", a.x.z, a.y.z, a.z.z);
}

void printFixMatrix(Matrix3x3 a) {
	printf("\r\n[ ");
	printFix(a.x.x);
	printf(", ");
	printFix(a.y.x);
	printf(", ");
	printFix(a.z.x);
	printf(" ] \r\n");

	printf("[ ");
	printFix(a.x.y);
	printf(", ");
	printFix(a.y.y);
	printf(", ");
	printFix(a.z.y);
	printf(" ] \r\n");

	printf("[ ");
	printFix(a.x.z);
	printf(", ");
	printFix(a.y.z);
	printf(", ");
	printFix(a.z.z);
	printf(" ] \r\n");

}

void print_matrix(Matrix3x3 a) {
    printf("(%ld, %ld, %ld)\n", a.x.x, a.y.x, a.z.x);
    printf("(%ld, %ld, %ld)\n", a.x.y, a.y.y, a.y.z);
    printf("(%ld, %ld, %ld)\n", a.x.z, a.y.z, a.z.z);
}
