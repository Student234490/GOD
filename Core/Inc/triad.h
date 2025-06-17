/*
 * triad.h
 *
 *  Created on: 14. jun. 2025
 *      Author: NoahRC
 */

#ifndef INC_TRIAD_H_
#define INC_TRIAD_H_
#include <stdint.h>
#include <vector.h>

void triad(Vector3D B1, Vector3D g1, Vector3D B2, Vector3D g2, Matrix3x3* result);
void rot2eulerZYX(const Matrix3x3* R, Vector3D* angles);


#endif /* INC_TRIAD_H_ */
