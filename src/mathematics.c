#include <math.h>
#include "mathematics.h"

/**
    Private methods
    */
bool rectangle_wsp(long double fi, long double fi0);

/**
    Code
    */
inline int int_abs(int a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}

inline long double double_abs(long double a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}


inline long double double_min(long double a, long double b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

inline long double double_max(long double a, long double b){
    if(a > b){
        return a;
    }else{
        return b;
    }
}

inline short int short_min(short int a, short int b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

inline short int short_max(short int a, short int b){
    if(a > b){
        return a;
    }else{
        return b;
    }
}

inline int int_max(int a, int b){
    if(a > b){
        return a;
    }else{
        return b;
    }
}

int rzad(int a){
    int res = 1, lim = 10;
    a = int_abs(a);
    while(a >= lim){
        res += 1;
        lim *= 10;
    }
    return res;
}

inline int sign(long double a){
    if(a == 0){
        return 0;
    }else if(a > 0){
        return 1;
    }else{
        return -1;
    }
}

inline short short_sign(short a){
     if(a == 0){
        return 0;
    }else if(a > 0){
        return 1;
    }else{
        return -1;
    }
}

/**
    Ensures that  fi  angle is in [0, dwaPI)
    */

long double norm(long double fi){
	if(fi > 0){
		while(fi - dwaPI > eps){
			fi -= dwaPI;
		}
	}else
		while(fi < 0){
			fi += dwaPI;
		}
    if(double_abs(fi - dwaPI) < eps){
        fi = 0;
    }
	return fi;
}

long double square_equation(long double r0, long double fi){
	fi -= PI4;
	long double s = sin(fi),
          c = cos(fi);
	s = (s + sign(s * c) * c);
	if(s == 0){
		s = 1;
	}
	return double_abs(r0 / s);
}

/**
    Returns   true   if   the first factor (wsp1)   should be used,
              false  if   the second one   (wsp2).
    Used to say which pair of sides is pointed by angle  fi
    fi0 - angle between two closer verticies and center of the rectangle
    */

bool rectangle_wsp(long double fi, long double fi0){
    fi = norm(fi);
    if( ((fi >= PIpol) && (fi <= PIpol + fi0)) ||
        ((fi >= PI32)  && (fi <= PI32  + fi0)) ){
        return false;
    }else{
        return true;
    }
}

long double rectangle_equation(long double r0, long double fi, long double fi0, long double fi02, long double wsp1, long double wsp2){
    fi += fi02;
    long double res = sin(fi);

    if(rectangle_wsp(fi, fi0)){
        res -= wsp1 * cos(fi);
    }else{
        res -= wsp2 * cos(fi);
    }
    if(res != 0){
        res = double_abs(r0 / res);
    }

    return res;
}

long double vector_angle(long double x, long double y){
   if(x != 0){
		long double b;
		b = atan(double_abs(y / x));
		if(y == 0){
			if(x < 0){
				b = PI;
			}
		}
		else if(x < 0){
			if(y > 0){
				b = PI - b;
			}else{
				b += PI;
			}
		}
		else if(y < 0){
			b = dwaPI - b;
		}
        return b;
    }
    else{
		if(y == 0){
			return 0;
		}else if(y > 0){
			return PIpol;
		}else{
            return PI32;
		}
    }
}




void get_line_from_points(long double x1, long double y1, long double x2, long double y2, line *L){
    L->A = y1 - y2;
    L->B = x2 - x1;
    L->C = x1 * y2 - x2 * y1;
    L->sqrtAB = sqrt(L->A * L->A + L->B * L->B);
}

void get_line_from_point_and_vector(long double x, long double y, long double vx, long double vy, line *L){
    L->A = -vy;
    L->B = vx;
    L->C = x * vy - y * vx;
}

inline long double point_distance_from_line(long double x0, long double y0, line *L){
    if(L->sqrtAB == 0){
        return -1;
    }else{
        return double_abs((L->A * x0 + L->B * y0 + L->C) / L->sqrtAB);
    }
}

void common_point(const line* L1, const line* L2, long double *x, long double *y){
    *y = (L1->C * L2->A - L1->A * L2->C) / (L2->B * L1->A - L1->B * L2->A);
    *x = -(L1->C + L1->B * *y) / L1->A;
}

/**
    for vectors [x1, y1] and [x2, y2]
    */
inline long double vector_product(long double x1, long double y1, long double x2, long double y2){
    return x1 * y2 - x2 * y1;
}

bool vectors_on_two_sides(long double vector_pr1, long double vector_pr2){
    if(vector_pr1 == 0 || vector_pr2 == 0){
        return true;
    }else if((vector_pr1 > 0 && vector_pr2 < 0) ||
             (vector_pr1 < 0 && vector_pr2 > 0) ){
        return true;
    }else{
        return false;
    }
}

bool do_segments_intersect(const point *A1, const point *A2,
                           const point *B1, const point *B2){
    long double v_x = A2->x - A1->x,
           v_y = A2->y - A1->y,
           b_x = B1->x - A1->x,
           b_y = B1->y - A1->y;

    if(vectors_on_two_sides(vector_product(v_x, v_y, B2->x - A1->x, B2->y - A1->y), vector_product(v_x, v_y, b_x, b_y))){
        b_x = -b_x;
        b_y = -b_y;
        v_x = B2->x - B1->x;
        v_y = B2->y - B1->y;
        if(vectors_on_two_sides(vector_product(v_x, v_y, b_x, b_y), vector_product(v_x, v_y, A2->x - B1->x, A2->y - B1->y))){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

/**
    This function uses do_segments_intersect
    */
bool get_segment_intersection(const point *A1, const point *A2,
                              const point *B1, const point *B2,
                              point *I){
    if(do_segments_intersect(A1, A2, B1, B2)){
        long double dxA = A1->x - A2->x,
               dxB = B1->x - B2->x,
               dyA = A1->y - A2->y,
               dyB = B1->y - B2->y,
               denom = dxA * dyB - dyA * dxB,
               dxyA,
               dxyB;
        /**
            If segments cover each other
            function does nothing
            */
        if(double_abs(denom) < eps){
            return false;
        }else{
            dxyA = A1->x * A2->y - A1->y * A2->x;
            dxyB = B1->x * B2->y - B1->y * B2->x;

            I->x = (dxyA * dxB - dxA * dxyB) / denom;
            I->y = (dxyA * dyB - dyA * dxyB) / denom;
            return true;
        }
    }else{
        return false;
    }
}

long double coefficient_multiplier(long double v){
    v = double_abs(v) / MACH_SPEED;
    if(v < 1){
        return exp(v * 1.3862943611198906);
    }else if(v < 1.5){
        v -= 1;
        return -4 * v * v + 4.0;
    }else{
        return 0.538859 * exp(-v * 1.098612 + 3.295837) + 0.2;
    }
}
