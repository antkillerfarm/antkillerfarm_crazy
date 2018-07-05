#include "types.h"
#include "matrix.h"

#define MAE_MAX_RANK 16
#define MAE_MAX_INV_RANK 5
#define USER_MAX_RANK 48

bool sw_MatrixAdd(pMatrix p_op1, pMatrix p_op2, pMatrix p_res)
{
	int8u i, j;
	float64 (*op1_element)[p_op1->col];
	float64 (*op2_element)[p_op2->col];
	float64 (*res_element)[p_res->col];
	
	if ((p_op1->col != p_op2->col) || (p_op1->row != p_op2->row)) {
		return FALSE;
	}

	if ((p_res->col != p_op1->col) || (p_res->row != p_op1->row)) {
		return FALSE;
	}

	op1_element = p_op1->element;
	op2_element = p_op2->element;
	res_element = p_res->element;

	for (i=0; i<p_res->row; i++) {
		for (j=0; j<p_res->col; j++) {
			res_element[i][j] = op1_element[i][j] + op2_element[i][j];
		}
	}

	return TRUE;
}

bool sw_MatrixMul(pMatrix p_op1, pMatrix p_op2, pMatrix p_res)
{
	int8u i, j, k;
	float64 (*op1_element)[p_op1->col];
	float64 (*op2_element)[p_op2->col];
	float64 (*res_element)[p_res->col];
	float64 sum;
	
	if (p_op1->col != p_op2->row) {
		return FALSE;
	}

	if ((p_res->row != p_op1->row) || (p_res->col != p_op2->col)) {
		return FALSE;
	}

	op1_element = p_op1->element;
	op2_element = p_op2->element;
	res_element = p_res->element;

	for (i=0; i<p_res->row; i++) {
		for (j=0; j<p_res->col; j++) {
			sum = 0;
			for (k=0; k<p_op1->col; k++) {
				sum += op1_element[i][k] * op2_element[k][j];
			}
			
			res_element[i][j] = sum;
		}
	}

	return TRUE;
}


typedef struct {
	int8u top;
	int8u left;
} MatrixCoord, *pMatrixCoord;

typedef struct {
	int8u row;
	int8u col;
} MatrixSize, *pMatrixSize;

typedef struct {
	MatrixCoord matrix_coord;
	MatrixSize matrix_size;
} SubMatrix, *pSubMatrix;

typedef struct {
	SubMatrix sub_matrix[3][3];
	int8u sub_matrix_row;
	int8u sub_matrix_col;
} UnitedMatrix, *pUnitedMatrix;

void MatrixCpyEx(pMatrix p_src, pMatrix p_des, pMatrixCoord p_src_coord, pMatrixCoord p_des_coord, pMatrixSize p_size)
{
	float64 *p_element_src, *p_element_des,*p_element_src1, *p_element_des1;
	int16u index_src, index_des;
	int16u i, j;
	int16u row, col;

	p_element_src = p_src->element;
	p_element_des =  p_des->element;
	row = p_size->row;
	col = p_size->col;

	for (i = 0; i < row; i++)
	{
		index_src = (p_src_coord->top + i) * p_src->col + p_src_coord->left;
		index_des = (p_des_coord->top + i) * p_des->col + p_des_coord->left;
		p_element_src1 = p_element_src + index_src;
		p_element_des1 = p_element_des + index_des;
		for (j = 0; j < col; j++)
		{
			*p_element_des1 = *p_element_src1;
			p_element_src1++;
			p_element_des1++;
		}
	}
}

void DivideHugeMatrix(pMatrix p_op, pUnitedMatrix p_um)
{
	int8u row_mod, col_mod, sub_matrix_row, sub_matrix_col;
	int16u i, j;
	pSubMatrix p_sub_matrix;
	
	row_mod = p_op->row % MAE_MAX_RANK;
	sub_matrix_row = p_op->row / MAE_MAX_RANK;
	col_mod = p_op->col % MAE_MAX_RANK;
	sub_matrix_col = p_op->col / MAE_MAX_RANK;
	for (i = 0; i < sub_matrix_row; i++)
	{
		for (j = 0; j < sub_matrix_col; j++)
		{
			p_sub_matrix = &(p_um->sub_matrix[i][j]);
			p_sub_matrix->matrix_size.row = MAE_MAX_RANK;
			p_sub_matrix->matrix_size.col = MAE_MAX_RANK;
			p_sub_matrix->matrix_coord.top = i * MAE_MAX_RANK;
			p_sub_matrix->matrix_coord.left = j * MAE_MAX_RANK;
		}
	}
	p_um->sub_matrix_row = sub_matrix_row;
	p_um->sub_matrix_col = sub_matrix_col;
	if (row_mod != 0)
	{
		for (i = 0; i < sub_matrix_col; i++)
		{
			p_sub_matrix = &(p_um->sub_matrix[sub_matrix_row][i]);
			p_sub_matrix->matrix_size.row = row_mod;
			p_sub_matrix->matrix_size.col = MAE_MAX_RANK;
			p_sub_matrix->matrix_coord.top = sub_matrix_row * MAE_MAX_RANK;
			p_sub_matrix->matrix_coord.left = i * MAE_MAX_RANK;
		}
		p_um->sub_matrix_row++;
	}
	if (col_mod != 0)
	{
		for (i = 0; i < sub_matrix_row; i++)
		{
			p_sub_matrix = &(p_um->sub_matrix[i][sub_matrix_col]);
			p_sub_matrix->matrix_size.row = MAE_MAX_RANK;
			p_sub_matrix->matrix_size.col = col_mod;
			p_sub_matrix->matrix_coord.top = i * MAE_MAX_RANK;
			p_sub_matrix->matrix_coord.left = sub_matrix_col * MAE_MAX_RANK;
		}
		p_um->sub_matrix_col++;
	}
	if (row_mod != 0 && col_mod != 0)
	{
		p_sub_matrix = &(p_um->sub_matrix[sub_matrix_row][sub_matrix_col]);
		p_sub_matrix->matrix_size.row = row_mod;
		p_sub_matrix->matrix_size.col = col_mod;
		p_sub_matrix->matrix_coord.top = sub_matrix_row * MAE_MAX_RANK;
		p_sub_matrix->matrix_coord.left = sub_matrix_col * MAE_MAX_RANK;
	}
}

bool hw_HugeMatrixAdd(pMatrix p_op1, pMatrix p_op2, pMatrix p_res)
{
	UnitedMatrix united_matrix;
	pSubMatrix p_sub_matrix;
	int8u row, col;
	MatrixCoord matrix_coord_zero = {0, 0};
	int16u i, j;
	Matrix M_sub_op1;
	Matrix M_sub_op2;
	Matrix M_sub_res;

	DivideHugeMatrix(p_op1, &united_matrix);
	for (i = 0; i < united_matrix.sub_matrix_row; i++)
	{
		for (j = 0; j < united_matrix.sub_matrix_col; j++)
		{
			p_sub_matrix = &(united_matrix.sub_matrix[i][j]);
			row = p_sub_matrix->matrix_size.row;
			col = p_sub_matrix->matrix_size.col;
			float64 sub_op1[row][col];
			float64 sub_op2[row][col];
			float64 sub_res[row][col];
			MatrixInit(&M_sub_op1, row, col, sub_op1);
			MatrixInit(&M_sub_op2, row, col, sub_op2);
			MatrixInit(&M_sub_res, row, col, sub_res);

			MatrixCpyEx(p_op1, &M_sub_op1, &(p_sub_matrix->matrix_coord), &matrix_coord_zero, &(p_sub_matrix->matrix_size));
			MatrixCpyEx(p_op2, &M_sub_op2, &(p_sub_matrix->matrix_coord), &matrix_coord_zero, &(p_sub_matrix->matrix_size));
			sw_MatrixAdd(&M_sub_op1, &M_sub_op2, &M_sub_res);
			MatrixCpyEx(&M_sub_res, p_res, &matrix_coord_zero, &(p_sub_matrix->matrix_coord), &(p_sub_matrix->matrix_size));
		}
	}	
	return TRUE;
}

bool hw_MatrixAdd(pMatrix p_op1, pMatrix p_op2, pMatrix p_res)
{
	if ((p_op1->col != p_op2->col) || (p_op1->row != p_op2->row)) {
		return FALSE;
	}

	if ((p_res->col != p_op1->col) || (p_res->row != p_op1->row)) {
		return FALSE;
	}

	if (p_op1->row * p_op1->col <= MAE_MAX_RANK * MAE_MAX_RANK)
	{
		return sw_MatrixAdd(p_op1, p_op2, p_res);
	}
	else if (p_op1->row < USER_MAX_RANK && p_op1->col < USER_MAX_RANK)
	{
		return hw_HugeMatrixAdd(p_op1, p_op2, p_res);
	}
	else
	{
		return FALSE;
	}
}

#define SUM_FLAG_INIT 0
#define SUM_FLAG_RES 1
#define SUM_FLAG_SUM0 2
#define SUM_FLAG_SUM1 3

bool hw_HugeMatrixMul(pMatrix p_op1, pMatrix p_op2, pMatrix p_res)
{
	UnitedMatrix united_matrix_op1, united_matrix_op2, united_matrix_res;
	pSubMatrix p_sub_matrix_op1, p_sub_matrix_op2, p_sub_matrix_res;
	int8u row, col, sum_flag;
	MatrixCoord matrix_coord_zero = {0, 0};
	int16u i, j, k;
	Matrix M_sub_op1;
	Matrix M_sub_op2;
	Matrix M_sub_res;
	Matrix M_sub_sum0, M_sub_sum1;
	pMatrix p_sub_res;

	DivideHugeMatrix(p_op1, &united_matrix_op1);
	DivideHugeMatrix(p_op2, &united_matrix_op2);
	DivideHugeMatrix(p_res, &united_matrix_res);

	for (i = 0; i < united_matrix_res.sub_matrix_row; i++)
	{
		for (j = 0; j < united_matrix_res.sub_matrix_col; j++)
		{
			sum_flag = SUM_FLAG_INIT;
			p_sub_matrix_res = &(united_matrix_res.sub_matrix[i][j]);
			row = p_sub_matrix_res->matrix_size.row;
			col = p_sub_matrix_res->matrix_size.col;
			float64 sub_res[row][col];
			float64 sub_sum0[row][col];
			float64 sub_sum1[row][col];
			MatrixInit(&M_sub_res, row, col, sub_res);
			MatrixInit(&M_sub_sum0, row, col, sub_sum0);
			MatrixInit(&M_sub_sum1, row, col, sub_sum1);

			for (k = 0; k < united_matrix_op1.sub_matrix_col; k++)
			{
				p_sub_matrix_op1 = &(united_matrix_op1.sub_matrix[i][k]);
				row = p_sub_matrix_op1->matrix_size.row;
				col = p_sub_matrix_op1->matrix_size.col;
				float64 sub_op1[row][col];
				MatrixInit(&M_sub_op1, row, col, sub_op1);
				MatrixCpyEx(p_op1, &M_sub_op1, &(p_sub_matrix_op1->matrix_coord), &matrix_coord_zero, &(p_sub_matrix_op1->matrix_size));

				p_sub_matrix_op2 = &(united_matrix_op2.sub_matrix[k][j]);
				row = p_sub_matrix_op2->matrix_size.row;
				col = p_sub_matrix_op2->matrix_size.col;
				float64 sub_op2[row][col];
				MatrixInit(&M_sub_op2, row, col, sub_op2);
				MatrixCpyEx(p_op2, &M_sub_op2, &(p_sub_matrix_op2->matrix_coord), &matrix_coord_zero, &(p_sub_matrix_op2->matrix_size));

				if (sum_flag == SUM_FLAG_INIT)
				{
					sw_MatrixMul(&M_sub_op1, &M_sub_op2, &M_sub_res);
					sum_flag = SUM_FLAG_RES;
				}
				else if (sum_flag == SUM_FLAG_RES)
				{
					sw_MatrixMul(&M_sub_op1, &M_sub_op2, &M_sub_sum0);
					sw_MatrixAdd(&M_sub_res, &M_sub_sum0, &M_sub_sum1);
					sum_flag = SUM_FLAG_SUM1;
				}
				else if (sum_flag == SUM_FLAG_SUM0)
				{
					sw_MatrixMul(&M_sub_op1, &M_sub_op2, &M_sub_res);
					sw_MatrixAdd(&M_sub_res, &M_sub_sum0, &M_sub_sum1);
					sum_flag = SUM_FLAG_SUM1;
				}
				else // sum_flag == SUM_FLAG_SUM1
				{
					sw_MatrixMul(&M_sub_op1, &M_sub_op2, &M_sub_res);
					sw_MatrixAdd(&M_sub_res, &M_sub_sum1, &M_sub_sum0);
					sum_flag = SUM_FLAG_SUM0;
				}
			}
			if (sum_flag == SUM_FLAG_RES)
			{
				p_sub_res = &M_sub_res;
			}
			else if (sum_flag == SUM_FLAG_SUM0)
			{
				p_sub_res = &M_sub_sum0;
			}
			else // sum_flag == SUM_FLAG_SUM1
			{
				p_sub_res = &M_sub_sum1;
			}
			MatrixCpyEx(p_sub_res, p_res, &matrix_coord_zero, &(p_sub_matrix_res->matrix_coord), &(p_sub_matrix_res->matrix_size));
		}
	}	
	return TRUE;
}

bool hw_MatrixMul(pMatrix p_op1, pMatrix p_op2, pMatrix p_res)
{
	if (p_op1->col != p_op2->row)
	{
		return FALSE;
	}

	if ((p_res->row != p_op1->row) || (p_res->col != p_op2->col))
	{
		return FALSE;
	} 

	if ((p_op1->row * p_op1->col <= MAE_MAX_RANK * MAE_MAX_RANK) && 
		(p_op2->row * p_op2->col <= MAE_MAX_RANK * MAE_MAX_RANK) &&
		(p_res->row * p_res->col <= MAE_MAX_RANK * MAE_MAX_RANK))
	{
		return sw_MatrixMul(p_op1, p_op2, p_res);
	}
	else
	{
		return hw_HugeMatrixMul(p_op1, p_op2, p_res);
	}
}

void MatrixInit(pMatrix p_matrix, int8u row, int8u col, void *array)
{
	p_matrix->row = row;
	p_matrix->col = col;
	p_matrix->element = array;
}

bool hw_HugeMatrixInverse(pMatrix p_op,pMatrix p_res)
{
	return FALSE;
}

bool hw_MatrixInverse(pMatrix p_op,pMatrix p_res)
{
	if ((p_op->row != p_op->col) || (p_res->row != p_res->col) || (p_op->row != p_res->col)) {
		return FALSE;
	}

	if ((p_op->row <= MAE_MAX_INV_RANK))
	{
		return sw_MatrixInverse(p_op, p_res);
	}
	else
	{
		return hw_HugeMatrixInverse(p_op, p_res);
	}
}

bool sw_MatrixInverse(pMatrix p_op, pMatrix p_res)
{
	float64 Y[p_op->row][p_op->row];
	float64 indx[p_op->row];
	float64 B[p_op->row];
	float64 decomp[p_op->row][p_op->row];
	int d, row, col,i;
	//float64 trans[n][n];
	int32u n = p_op->row;
	float64 (*mat)[n];
	float64 (*inv)[n];
	Matrix ms_Y;

	if ((p_op->row != p_op->col) || (p_res->row != p_res->col) || (p_op->row != p_res->col)) {
		return FALSE;
	}
	
	ms_Y.row = p_op->row;
	ms_Y.col = p_op->col;
	ms_Y.element = Y;

	mat = p_op->element;
	inv = p_res->element;
	
	//initialize matrix Y, matrix Y is a unit matrix
	for (row=0; row<n; row++) {
		for (col=0; col<n; col++) {
			if (row == col) {
				Y[row][col] = 1.0;
			}
			else {
				Y[row][col] = 0.0;
			}
		}
	}

	//Matrix LU-Decompose operation
	if (lu_decompose(n, mat, decomp , indx, &d) != TRUE) {
		return FALSE;
	}

	//substitute process
	for (col=0; col<n; col++) {
		//copy_column(n, n, 1, Y, B, col, 0);
		for (i=0; i<n; i++) {
			B[i] = Y[i][col];
		}

		lu_back_subst(n, decomp, indx, B);

		for (i=0; i<n; i++) {
			Y[i][col] = B[i];
		}
	}

	//transepose matrix
	sw_MatrixTrans(&ms_Y, p_res);

	return TRUE;
}

bool lu_decompose(int n, float64 (*src)[n], float64 (*lu)[n],  float64 *indx, int *d )
{
    float64 dum; // from the book -- I don't know significance
    float64 sum;
    float64 maximum;
    float64 temp;
    float64 lu_decomp[n][n];
    //CMatrix lu_decomp(rows(),cols());

    float64 scale_vector[n];

    int row,col,k,col_max = 0; // counters

    *d = 1; // parity check

	// make a direct copy of the original matrix:
	//deepcopy(*this,lu_decomp);
	for (row=0; row<n; row++) {
		for (col=0; col<n; col++) {
			lu_decomp[row][col] = src[row][col];
		}
	}

	//CMatrix scale_vector = lu_decomp.scale(); // scale the matrix
	for (col=0; col<n; col++) {

        maximum = 0.0;

		for (row=0; row<n; row++) {

            temp = fabs(src[row][col]);

			if ( temp > maximum ) {
				maximum = temp;  // find max column magnitude in this row
			}
		}

		if (maximum == 0.0) {
			return FALSE;
		}

		scale_vector[col] = 1.0 / maximum; // save the scaling
	}

	// The loop over columns of Crout's method:
	for (row=0; row<n; row++) {
		if (row > 0) {
			// eqn 2.3.12 except for row=col:
			for (col=0; col<=row-1; col++) {
				sum = lu_decomp[row][col];
				if (col > 0) {
					for (k=0; k<=col-1; k++) {
						sum -= lu_decomp[row][k]*lu_decomp[k][col];
					}
					lu_decomp[row][col] = sum;
				}
			}
		}

		// Initialize for the search for the largest pivot element:
		maximum = 0;

		// i=j of eq 2.3.12 & i=j+1..N of 2.3.13:
		for (col=row; col<=n-1; col++) {
			sum = lu_decomp[row][col];
			if (row > 0) {
				for (k=0; k<=row-1; k++) {
					sum -= lu_decomp[k][col] * lu_decomp[row][k];
				}
				lu_decomp[row][col] = sum;
			}

			// figure of merit for pivot:
			dum = scale_vector[col] * fabs(sum);
			if (dum >= maximum) { // is it better than the best so far?
				col_max = col;
				maximum = dum;
			}
		}

		// Do we need to interchange rows?
		if (row != col_max) {
			//lu_decomp.switch_columns(col_max,row); // Yes, do so...
			float64 tmp_col[n];
			for (k=0; k<n; k++) {
				tmp_col[k] = lu_decomp[k][col_max];
			}
			for (k=0; k<n; k++) {
				lu_decomp[k][col_max] = lu_decomp[k][row];
			}
			for (k=0; k<n; k++) {
				lu_decomp[k][row] = tmp_col[k];
			}

			*d *= -1;  // ... and change the parity of d
			// also interchange the scale factor:
			dum = scale_vector[col_max];
			scale_vector[col_max] = scale_vector[row];
			scale_vector[row] = dum;
		}

		indx[row] = col_max;
		// Now, finally, divide by the pivot element:
		if (row != n-1) {
			if (lu_decomp[row][row] == 0) {
				lu_decomp[row][row] = 1e-20;
			}
			// If the pivot element is zero the matrix is
			// singular (at least to the precision of the
			// algorithm).  For some applications on singular
			// matrices, it is desirable to substitute tiny for zero
			dum = 1/lu_decomp[row][row];
			for (col=row+1; col<=n-1; col++) {
				lu_decomp[row][col] *= dum;
			}
		}
	}

	if (lu_decomp[n-1][n-1] == 0) {
		lu_decomp[n-1][n-1] = 1e-20;
	}

	//lu is the result matrix of LU-decompse
	for (row=0; row<n; row++) {
		for (col=0; col<n; col++) {
			lu[row][col] = lu_decomp[row][col];
		}
	}

	return TRUE;
}


void lu_back_subst(int32u n, float64 (*lu)[n],  float64 *indx, float64 *b)
{
	float64 sum;
	int32s row, col, ll;
	int32s ii = 0;

	for (col=0; col<n; col++) {
		ll= (int)indx[col];
		sum = b[ll];
		b[ll] = b[col];
		if (ii >= 0) {
			for (row=ii; row<=col-1; row++) {
				sum -= lu[row][col] * b[row];
			}
		}
		else if (sum != 0) {
			ii = col;
		}
		b[col] = sum;
	}

	for (col=n-1; col>=0; col--) {
		sum = b[col];
		if (col < n-1) {
			for (row=col+1; row<=n-1; row++) {
				sum -= lu[row][col] * b[row];
			}
		}
		// store a component of the soln vector X:
		b[col] = sum / lu[col][col];
	}
}

bool sw_MatrixTrans(pMatrix p_op, pMatrix p_res)
{
	int8u i, j;
	float64 (*op_element)[p_op->col];
	float64 (*res_element)[p_res->col];
	
	if ((p_res->col != p_op->row) || (p_res->row != p_op->col)) {
		return FALSE;
	}

	op_element = p_op->element;
	res_element = p_res->element;

	for (i=0; i<p_res->row; i++) {
		for (j=0; j<p_res->col; j++) {
			res_element[i][j] = op_element[j][i];
		}
	}

	return TRUE;
}

