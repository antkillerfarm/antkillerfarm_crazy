#ifndef _MATRIX_H_
#define _MATRIX_H_

typedef struct {
	void *element;
	int8u row;
	int8u col;
} Matrix, *pMatrix;

/* callee function declaration */
#ifdef __cplusplus
extern "C"
{
#endif

    /* internal function */
    bool lu_decompose(int n, float64 (*src)[n], float64 (*lu)[n],  float64 *indx, int *d );
    void lu_back_subst(int32u n, float64 (*lu)[n],  float64 *indx, float64 *b);
    bool cholesky(int32u n, float64(*a)[n], float64 (*l)[n]);
    void ll_back_subst(int32u n, float64 (*lu)[n], float64 *b);

    void MatrixInit(pMatrix p_matrix, int8u row, int8u col, void *element);
    bool MatrixCpy(pMatrix p_src, pMatrix p_des);

        bool sw_MatrixAdd(pMatrix p_op1, pMatrix p_op2, pMatrix p_res);

        bool sw_MatrixSub(pMatrix p_op1, pMatrix p_op2, pMatrix p_res);

        bool sw_MatrixMul(pMatrix p_op1, pMatrix p_op2, pMatrix p_res);

        bool sw_MatrixInverse(pMatrix p_op, pMatrix p_res);

        bool sw_MatrixTrans(pMatrix p_op, pMatrix p_res);

        bool hw_MatrixAdd(pMatrix p_op1, pMatrix p_op2, pMatrix p_res);
        bool hw_MatrixMul(pMatrix p_op1, pMatrix p_op2, pMatrix p_res);

#ifdef __cplusplus
}
#endif


#endif // end of _MATRIX_H_
