#ifndef _FILTER_H_
#define _FILTER_H_

#define FILTER_W 5
#define FILTER_H 5

#include <stdint.h>

float filter_laplace_f[] = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
						   	 -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
							 -1.0f, -1.0f, 24.0f, -1.0f, -1.0f,
							 -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
							 -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};


#endif
