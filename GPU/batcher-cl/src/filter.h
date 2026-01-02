#ifndef _FILTER_H_
#define _FILTER_H_

#include <stdint.h>

float filter_laplace_f[] = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
						   	 -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
							 -1.0f, -1.0f, 24.0f, -1.0f, -1.0f,
							 -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
							 -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};


#endif
