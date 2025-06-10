// #include <stdint.h>
// #include <math.h>
#include <vector_and_matrix.h>

//////////////////Vector////////////////////////////////////////////

int32_t dot(Vector3D a, Vector3D b) {
    return Mult(a.x, b.x) + Mult(a.y, b.y) + Mult(a.z, b.z);
}

Vector3D cross(Vector3D a, Vector3D b){
	Vector3D c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.z;
	return c;
}

Vector3D add_vector(Vector3D a, Vector3D b){
	Vector3D vector;
	vector.x = a.x + b.x;
	vector.y = a.y + b.y;
	vector.z = a.z + b.z;
	return vector;
}

Vector3D subtract_vector(Vector3D a, Vector3D b){
	Vector3D vector;
	vector.x = a.x - b.x;
	vector.y = a.y - b.y;
	vector.z = a.z - b.z;
	return vector;
}

int32_t norm(Vector3D a){ //find norm
	sqrt(Mult(a.x, a.x) + Mult(a.y, a.y) + Mult(a.z, a.z));
}

int32_t cos_theta(Vector3D a, Vector3D b){ //cos_theta (later use??)
	return Div(dot(a,b), Mult(norm(a),norm(b)));
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

void print_matrix(Matrix3x3 a) {
    printf("(%d, %d, %d)\n", a.x.x, a.y.x, a.z.x);
    printf("(%d, %d, %d)\n", a.x.y, a.y.y, a.y.z);
    printf("(%d, %d, %d)\n", a.x.z, a.y.z, a.z.z);
}
