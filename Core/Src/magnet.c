/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h> // for printing
//#include <math.h> // for sine, cosine and power
#include <string.h> // for sizeof()
#include <time.h> // for benchmarking
#include <fixp.h>
#include <luts.h>

// placeholders for fixed point maths later
#define MULT(a,b) FIX16_MULT((a),(b))
#define POW(a,b)  FIX16_POW((a),(b))
#define DIV(a,b)  FIX16_DIV((a),(b))
#define PI 205887 // pi in fixed point 16.16 format
#define N_ITERS 100000 // benchmarking repetitions

// hard code constants
int Ns[104] = {1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,};
int Ms[104] = {0, 1, 0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

int32_t Gs[104] = {
0x8C4C999A, 0xF93FCCCD, 0xF2B67333, 0x14C29EB8, 0x05A70A3D, 0x0D150000, 0xE4A275C3, 0x0978AE14, 0x0234DC29, 0x0FEECF5C, 0x110223D7, 0x03D247AE, 0xFCB50F5C, 0x00524F5C, 0xF944F333, 0x0DF48A3D, 0x06AC6B85, 0xFD9A4F5C, 0xFE89F5C3, 0xFFF6F333, 0x0413D47B, 0x050930A4, 0x0454DC29, 0xF9BCFAE1, 0xFFDFCCCD, 0x002751EC, 0xFFC347AE, 0x084630A4, 0xF5BF4000, 0x00000000, 0x02A9EE14, 0x00706148, 0x002A999A, 0x0011AE14, 0xFFFF3852, 0x04CAAB85, 0x01BA68F6, 0xFDFC0A3D, 0xFEB8CA3D, 0xFE442E14, 0x0086F5C3, 0x00300F5C, 0xFFEC3333, 0xFFFB9C29, 0x01DACCCD, 0x04AD970A, 0x0145F333, 0xFD46F0A4, 0x01632B85, 0xFED42666, 0xFFE5E666, 0x00460F5C, 0xFFF4E3D7, 0xFFFB028F, 0xFE2AE3D7, 0xFA4C47AE, 0x01662E14, 0xFDFFA148, 0xFFC58F5C, 0x01117D71, 0x002951EC, 0x0028170A, 0x00225EB8, 0x0000CCCD, 0xFFFF599A, 0x03A2028F, 0xFCE723D7, 0xFCF6CF5C, 0x01EBF333, 0xFFE80CCD, 0x000FD70A, 0xFFBE1EB8, 0x0022B852, 0x0026B0A4, 0x0000E148, 0x0003428F, 0x000251EC, 0xFA5391EC, 0xFEF2E3D7, 0x009F07AE, 0x02484CCD, 0xFF9E9EB8, 0x012C9EB8, 0xFF97D99A, 0x00231EB8, 0xFFEE70A4, 0xFFF5C7AE, 0xFFFF0F5C, 0xFFFF70A4, 0xFFFFC51F, 0xFF02147B, 0xF9EAD70A, 0x01D0428F, 0x00804F5C, 0xFE76599A, 0x0388A3D7, 0xFF4B6148, 0x00BB0000, 0xFFC6E148, 0x00146B85, 0xFFFD28F6, 0x0004028F, 0x00000000, 0x00000F5C
};

int32_t GVs[104] = {
0x00034CCD, 0x0001999A, 0xFFEA6666, 0xFFF9970A, 0xFFFCE148, 0xFFFD4000, 0xFFF547AE, 0xFFFDAE14, 0xFFF947AE, 0xFFF430A4, 0x0015B333, 0xFFE0B0A4, 0x000968F6, 0xFFF1E666, 0xFFF4F852, 0x00071EB8, 0xFFEC051F, 0xFFFA599A, 0x00000000, 0xFFFFDC29, 0x0005C7AE, 0x0005AB85, 0x000A75C3, 0x0012EE14, 0xFFF6B852, 0xFFFED70A, 0x00007852, 0x00055C29, 0xFFFC7333, 0xFFF74F5C, 0x001687AE, 0x0008A3D7, 0x0003170A, 0xFFFF451F, 0x000051EC, 0x000507AE, 0x000D68F6, 0xFFE3F5C3, 0x000847AE, 0xFFF54F5C, 0x0002F852, 0x00036E14, 0xFFFE4000, 0x00004000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

int32_t Hs[104] = {
0x00000000, 0x1442199A, 0x00000000, 0xEF35BD71, 0xFE735C29, 0x00000000, 0xFD471EB8, 0x02382B85, 0xFE7BC000, 0x00000000, 0x05E48F5C, 0xFC748CCD, 0x00FA947B, 0xFF1F570A, 0x00000000, 0x01BD4CCD, 0x0529170A, 0xFD8D999A, 0xFFA8CF5C, 0x004A947B, 0x00000000, 0xFEB7170A, 0x03B7F333, 0x0288947B, 0xFEB20A3D, 0x003F0000, 0x001D6B85, 0x00000000, 0xF70CA8F6, 0xFD432666, 0x007EF852, 0x01285EB8, 0x005B6148, 0xFFC27D71, 0xFFFC4000, 0x00000000, 0x031DAB85, 0xFB4A3AE1, 0x016011EC, 0xFDC12B85, 0x00E5DEB8, 0x003D199A, 0xFFDAA666, 0xFFFEAE14, 0x00000000, 0xF6322666, 0x05AFE8F6, 0x040D4A3D, 0xFEA27852, 0xFEE50000, 0x009223D7, 0x001CA148, 0xFFEACF5C, 0x0002EB85, 0x00000000, 0x019D970A, 0x00000000, 0x02951EB8, 0x023CAB85, 0xFE4BE666, 0xFFCE6B85, 0xFFC5DEB8, 0x00400000, 0xFFFA28F6, 0xFFFB9C29, 0x00000000, 0x002EA3D7, 0x0213C28F, 0xFED8D47B, 0x026EBD71, 0x008E947B, 0xFFBE1EB8, 0xFF751C29, 0xFFEB851F, 0xFFF56B85, 0xFFFAD47B, 0xFFFF7AE1, 0x00000000, 0xFE9930A4, 0x00EE8A3D, 0x06570CCD, 0xFB0E051F, 0x00E9CF5C, 0x003E7D71, 0x00000000, 0x00000000, 0x0007A8F6, 0xFFF7828F, 0xFFFEE3D7, 0x00007333, 0x00000000, 0xF9EAD70A, 0x01358000, 0x0905A3D7, 0xFE76599A, 0xFD481EB8, 0xFFD2D99A, 0x00BB0000, 0x002AD70A, 0x0028D70A, 0x0008851F, 0xFFFDFD71, 0xFFFE947B, 0xFFFF8000
};

int32_t HVs[104] = {
0x00000000, 0xFFEACCCD, 0x00000000, 0xFFD8AE14, 0xFFF663D7, 0x00000000, 0x00112666, 0xFFF74A3D, 0xFFF9AE14, 0x00000000, 0x00096B85, 0x0003E8F6, 0x000AAB85, 0xFFFFC7AE, 0x00000000, 0xFFFCF333, 0x000B87AE, 0x000968F6, 0x00086E14, 0xFFFFA666, 0x00000000, 0xFFF2C51F, 0xFFE5199A, 0xFFFE028F, 0xFFFDD1EC, 0xFFFF87AE, 0x0001028F, 0x00000000, 0x0018D47B, 0x0008B0A4, 0x00020CCD, 0x0003B333, 0xFFFB0F5C, 0xFFFFC28F, 0x00002148, 0x00000000, 0xFFF2970A, 0x00059C29, 0x000C6E14, 0x000AB0A4, 0x00017AE1, 0xFFFDF0A4, 0x00010000, 0x00004000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

int32_t normalizeTheta(int32_t theta) {
    int32_t result;
    if ((theta > -1) && (theta<1)) {
        result = 1;
    }
    else {
    	if ((theta < ((180 << 16) + 1)) && (theta > ((179 << 16) + 65535))) {
            result = (179<<16)+65535;
        }
        else {
            result = theta;
        }
    }
    return result;
}

int32_t toRad(int32_t angledeg) {
    return DIV(MULT(angledeg,PI),180<<16);
}

void magnet(int32_t r, int32_t theta, int32_t phi, int32_t days, int32_t buffer[3]) {
	/*
	 * Input1: 16.16 distance from earth center,
	 * Input2: 16.16 latitude in degrees, where 0 is equator, 90 is north pole and -90 is south pole
	 * Input3: 16.16 longitude in degrees, ranging from -180 to 180, where 0 is the prime meridian
	 * Input4: 16.16 decimal days since 2000
	 * Input5: 3-array of int32_t to be changed to Br, Bt and Bp respectively, in Rasmus units
	 */
	printf("Theta input: ");
	printFix(theta);
	printf("\r\n");

	printf("Phi input: ");
	printFix(phi);
	printf("\r\n");

	printf("Normalized theta: ");
    theta = normalizeTheta(theta);
    printFix(theta);
    printf("\r\n");

    printf("Theta radians: ");
    theta = toRad(5898240-theta);
    printFix(theta);
    printf("\r\n");

    printf("Phi radians: ");
    phi = toRad(phi);
    printFix(phi);
    printf("\r\n");
    int32_t a = 417542963;

    int Nmodel = 13; // hard code
    int Nmax = Nmodel * (Nmodel + 3 ) / 2;

    int32_t g[Nmodel][Nmodel+1];
    int32_t h[Nmodel][Nmodel+1];
    memset(g, 0, sizeof(g));
    memset(h, 0, sizeof(h));


    int i;
    for (i = 0; i < Nmax; i++) { // 0 to 103
        g[Ns[i]-1][Ms[i]] = (Gs[i] + MULT(GVs[i], DIV(days, 23920640)) + 2 /* 1 << 1 */ ) >> 2; // 360 til 16.16
        h[Ns[i]-1][Ms[i]] = (Hs[i] + MULT(HVs[i], DIV(days, 23920640)) + 2 /* 1 << 1 */ ) >> 2;
    }

    int32_t Bt = 0;
    int32_t Br = 0;
    int32_t Bp = 0;

    int32_t P11 = 1<<16;
    int32_t P10 = 1<<16;
    int32_t dP11 = 0;
    int32_t dP10 = 0;
    int32_t P2;
    int32_t dP2;
    int32_t P20;
    int32_t dP20;
    int32_t K;

    int16_t m;
    int16_t n;

    for (m = 0; m <= Nmodel; m++) { // m fra 0 til og med 13
        for (n = 1; n <= Nmodel; n++) {
            if ( m <= n ) {
                if (m == n) {
                    P2 = MULT(sinrad(theta), P11);
                    dP2 = MULT(sinrad(theta), dP11) + MULT(cosrad(theta), P11);
                    P11 = P2;
                    P10 = P11;
                    P20 = 0;
                    dP11 = dP2;
                    dP10 = dP11;
                    dP20 = 0;
                } else {
                    if (n == 1) {
                        P2 = MULT(cosrad(theta), P10);
                        dP2 = MULT(cosrad(theta), dP10) - MULT(sinrad(theta), P10);
                        P20 = P10;
                        P10 = P2;
                        dP20 = dP10;
                        dP10 = dP2;
                    } else {
                    	int32_t nm1 = convert(n-1);           // 16.16
                    	int32_t mfix = convert(m);
                    	int32_t num  = FIX16_MULT(nm1,nm1) - FIX16_MULT(mfix,mfix);
                    	int32_t den  = FIX16_MULT(convert(2*n)-convert(1),
                    	                          convert(2*n)-convert(3));
                    	K = FIX16_DIV(num, den);
                    	P2 = MULT(cosrad(theta), P10) - MULT(K, P20);
                        dP2 = MULT(cosrad(theta), dP10) - MULT(sinrad(theta), P10) - MULT(K, dP20);
                        P20 = P10;
                        P10 = P2;
                        dP20 = dP10;
                        dP10 = dP2;
                    }
                }
                // Precompute repeated terms
                int32_t ar_pow = POW(DIV(a, r), (n + 2)<<16);
                int32_t mphi = MULT(m<<16, phi);
                int32_t cos_mphi = cosrad(mphi);
                int32_t sin_mphi = sinrad(mphi);

                int32_t gnm = g[n - 1][m];
                int32_t hnm = h[n - 1][m];

                int32_t gcos = MULT(gnm, cos_mphi);
                int32_t hsin = MULT(hnm, sin_mphi);
                int32_t gsin = MULT(gnm, sin_mphi);
                int32_t hcos = MULT(hnm, cos_mphi);

                // Magnetic field component updates
                Br = Br + MULT(MULT(ar_pow, (n + 1)<<16), MULT((gcos + hsin), P2));
                Bt = Bt + MULT(MULT(ar_pow, 1<<16), MULT((gcos + hsin), dP2));
                Bp = Bp + MULT(MULT(ar_pow, 1<<16), MULT(MULT(convert(m), (-gsin + hcos)), P2));
            }
        }
    }
    Bt = -Bt;
    Bp = -DIV(Bp, sinrad(theta));
    buffer[0] = Bt;
    buffer[1] = Bp;
    buffer[2] = Br;
}

/*
int32_t r = 417542963; // Earth's radius in fixed point 16.16 format
int32_t theta = 1966139; // 30 degrees in fixed point 16.16 format
int32_t phi = 1966080; // 30 degrees in fixed point 16.16 format
int32_t days = 478740480; // 15 years in fixed point 16.16 format (15 * 365.25 * 24 * 3600, converted to fixed point)
int32_t vector[3];
magnet(r, theta, phi, days, vector);
int i;
for (i=0; i<3; i++) {
	printFix(vector[i]);
*/
