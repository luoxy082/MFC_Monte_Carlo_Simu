#ifndef __SPA_H__
#define __SPA_H__

//4599 == 511*9
#define column 4599
#define row 511

#ifdef __cplusplus
extern "C" {
#endif
	int spa(int max_iterations, double EbN0_dB, int *iterations);



#ifdef __cplusplus
}
#endif
#endif 