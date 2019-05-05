#include "spa.h"
#include <stdio.h>
#include <math.h>
#include "Gaussian.h"
#include <time.h>
#include <memory.h>

int H[row][column] = { 0 };
double y[column] = { 0 };
double y_noised[column] = { 0 };
double Lj[column] = { 0 };
double Lj2i[row][column] = { 0 };
double Li2j[row][column] = { 0 };
double Lj_total[column] = { 0 };
int vj[column] = { 0 };

int init_H_matrix(void)
{
	FILE *pf_H = NULL;
	errno_t ret;

	ret = fopen_s(&pf_H, "H_out.f", "r");
	if (ret != 0)
	{
		//Trace(_T("H_out.f open fail"));
		fclose(pf_H);

		return 1;
	}

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			H[i][j] = fgetc(pf_H) - '0';
		}
		fgetc(pf_H);
	}

	fclose(pf_H);

	return 0;
}

int init_y_matrix()
{
	FILE *pf_y = NULL;
	errno_t ret;
	int x[column] = { 0 };

	ret = fopen_s(&pf_y, "y_out.f", "r");
	if (ret != 0)
	{
		//printf("y_out.f open fail\n");
		fclose(pf_y);

		return 1;
	}

	for (int j = 0; j < column; j++)
	{
		x[j] = fgetc(pf_y) - '0';
		y[j] = pow(-1, x[j]);
	}

	fclose(pf_y);

	return 0;
}

int y_matrix_add_noise(double EbN0_dB)
{
	double E = 1;
	double n = column;
	double f = row;
	double R = (n - f) / (n);
	double EbN0 = pow(10, EbN0_dB / 10);
	double noise_variance = E / (2 * R * EbN0);
	double noise;

	//for (int j = 0; j < column; j++)
	for (int j = 0; j < column; j++)
	{
		noise = sqrt(noise_variance) * Gaussian();
		y_noised[j] = y[j] + noise;
	}

	return 0;
}

int y_matrix_create_Lj(double EbN0_dB)
{
	double E = 1;
	double n = column;
	double f = row;
	double R = (n - f) / (n);
	//double EbN0 = pow(10, EbN0_dB / 10);
	double Eb = E / R;
	double N0 = Eb * pow(10, -EbN0_dB / 10);

	for (int j = 0; j < column; j++)
	{
		Lj[j] = 4 * sqrt(E) * y_noised[j] / N0;
	}

	return 0;
}

int init_Lj2i_matrix(void)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			if (1 == H[i][j])
			{
				Lj2i[i][j] = Lj[j];
			}
		}
	}

	return 0;
}

//int init_Li2j_matrix(void)
int CN_update(void)
{
	double product = 1;
	int index_column;

	for (int i = 0; i < row; i++)
	{
		product = 1;
		index_column = 0;
		while (index_column < column)
		{
			if (0.0 != Lj2i[i][index_column])
			{
				product = product * tanh(Lj2i[i][index_column] * 0.5);
			}
			index_column++;
		}

		for (int j = 0; j < column; j++)
		{
			if (0.0 != Lj2i[i][j])
			{
				Li2j[i][j] = 2 * atanh(product / tanh(Lj2i[i][j] * 0.5));
			}
		}
	}

	return 0;
}

int VN_update(void)
{
	double sum = 0;
	int index_row;

	for (int j = 0; j < column; j++)
	{
		sum = 0;
		index_row = 0;
		while (index_row < row)
		{
			if (0.0 != Li2j[index_row][j])
			{
				sum = sum + Li2j[index_row][j];
			}
			index_row++;
		}

		for (int i = 0; i < row; i++)
		{
			if (0.0 != Li2j[i][j])
			{
				Lj2i[i][j] = Lj[j] + sum - Li2j[i][j];
			}
		}
	}

	return 0;
}

int LLR_update(void)
{
	double sum = 0;

	for (int j = 0; j < column; j++)
	{
		sum = 0;
		for (int i = 0; i < row; i++)
		{
			sum = sum + Li2j[i][j];
		}
		Lj_total[j] = Lj[j] + sum;
	}

	return 0;
}

int vj_update(void)
{
	for (int j = 0; j < column; j++)
	{
		if (Lj_total[j] < 0)
		{
			vj[j] = 1;
		}
		else
		{
			vj[j] = 0;
		}
	}

	return 0;
}

int cycle_update(void)
{
	CN_update();
	VN_update();
	LLR_update();
	vj_update();

	return 0;
}

int check_vj(void)
{
	int sum;

	for (int i = 0; i < row; i++)
	{
		sum = 0;
		for (int j = 0; j < column; j++)
		{
			sum = sum + vj[j] * H[i][j];
		}

		if (0 != (sum % 2))
		{
			return 1;
		}
	}

	return 0;
}

int spa(int max_iterations, double EbN0_dB, int *iterations)
{
	int ret;
	int i;

	memset(Lj, 0, column);
	memset(Lj2i, 0, row * column);
	memset(Li2j, 0, row * column);
	memset(Lj_total, 0, column);
	memset(vj, 0, column);

	//ret = init_H_matrix();
	//if (0 != ret)
	//{
	//	return ret;
	//}
	//ret = init_y_matrix();
	//if (0 != ret)
	//{
	//	return ret;
	//}

	y_matrix_add_noise(EbN0_dB);
	y_matrix_create_Lj(EbN0_dB);
	init_Lj2i_matrix();

	for (i = 0; i < max_iterations; i++)
	{
		cycle_update();
		if (0 == check_vj())
		{
			break;
		}
	}
	*iterations = i;

	return 0;
}