#include "matrix.h"

/* TODO: 

   -- Implement error checking with set jumps and long jumps

   -- Need to read chapter 5 and apply all basic optimizations

   -- implement parallelized routines. This is critical for matrix computations.  

   -- CREATE COMPANION LATEX FILE ANALYZING RUN-TIMES OF ALL ALGOS 

   -- NEED TO GO THROUGH AND MAKE SURE OUTPUT IS PASSED AS A POINTER ARGUMENT

   -- one bug to be aware about is having a matrix of different dimension than 
      that is stated. make sure to avoid this. 
   
*/

/* This file will encode various matrix structs/operations, in 2D, which will ideally 
   be implemented using fast algorithms. The runtimes for each will be listed in the 
   comments. */

/* Allocates necessary heap memory. actual matrix memory is garbage. */
struct matrix* init_matrix(size_t rows, size_t cols)
{
  /* error checking */
  if (rows == 0)
    {
      fprintf(stderr, "Error: invalid number of rows");
      exit(0);
    }
  
  else if (cols == 0)
    {
      fprintf(stderr, "Error: invalid number of cols");
      exit(0);
    }

  struct matrix* A = malloc(sizeof(struct matrix));

  /* checks to see if the allocation worked */
  if (A == NULL)
    {
      fprintf(stderr, "Error: struct could not be allocated.");
      exit(0);
    } 

  set_rows(A, rows);
  set_cols(A, cols);

  /* need to allocate the memory for the matrix */
  A->data = (fp*) malloc(rows * cols * sizeof(fp));

  /* again...need to check to see if the allocation worked */
  if (A->data == NULL)
    {
      fprintf(stderr, "Error: data array could not be allocated.");
      exit(0);
    }  

  A->is_initialized = true;

  return A;
}

/* If allocated, frees both the data and the struct. */
void free_matrix(struct matrix* A)
{
  /* If matrix is not initialized, do nothing */
  if (!A->is_initialized)
    return;


  /* first free the data */
  free(A->data);

  /* then the struct itself */
  free(A);

  /* Then let future calls of free/init know it is freed */
  A->is_initialized = false;

}


/* Resizes the matrix according to the rows and columns given. Attempts
   to save as many elements as many elements as possible */
void resize_matrix(struct matrix* B, size_t rows, size_t cols)
{
  free_matrix(B);   

  //TODOTODOTODOTODOTODOTODOR NEED TO FIX THIS AND ALL RESET REFERENCES

  /* Rather than returning a function handle, this  */
  B; 
}

/* performs a deep copy of the given matrix and stores it in the input data structure.

   WARNING: This function will clear any existing memory/dimensionality of B if
            if it exists.
 */
void deep_copy(const struct matrix* A, struct matrix* B)
{
  is_valid(A);
  
  size_t i,j;

  reset_matrix(B, get_rows(A), get_cols(A));

  for(i = 1; i <= get_rows(B); i++)
    {
      for(j = 1; j <= get_cols(B); j++)
	{
	  set_elem(B, i, j, get_elem(A, i, j));
	}
    } 
}

/* Given two matrices in the order A and then B, checks to see if AB makes sense.
   Since this is a check function...code is so small makes sense to make it
   inline. */
inline bool are_conformable(const struct matrix* A, const struct matrix* B)
{
  return (get_cols(A) == get_rows(B));
} 

/* for addition or subtraction...or a host of other purposes */
inline bool same_dim(const struct matrix* A, const struct matrix* B)
{
  return (A->cols == B->cols && A->rows == B->rows);
}

/* naive implementation. When B is passed in, it is cleared.*/
void naive_transpose(const struct matrix* A, struct matrix* B)
{
  is_valid(A);
  reset_matrix(B, get_rows(A), get_cols(A));

  size_t i,j;

  /* we swap indices. note this suffers from poor
     locality but it seems as though every 'easy' approach will tbh */
  for (i = 1; i <= get_rows(A); i++)
    {
      for (j = 1; j <= get_cols(A); j++)
	{
	  set_elem(B, j, i, get_elem(A, i, j));	
	}
    }
}

/* Returns the sum of two matrices. Notice the sensitivity to row-major
   ordering of the matrix in main memory/caches */
void add_mats(const struct matrix* A, const struct matrix* B, struct matrix* sum)
{

  /* First *need* to check to see if A and B are of same dim...
     and sensible. This involves error-catching.  */
  is_valid(A);
  is_valid(B);


  if (!same_dim(A,B))
    {
      fprintf(stderr, "Error: matrices to add are NOT the same dimension");
      exit(0);
    }

  size_t i,j;
  size_t m = get_rows(A);
  size_t n = get_cols(A);

  /* Clear out/init destination for sum */
  free_matrix(sum);
  sum = init_matrix(m,n);

  /* initialize components of the return value */
  set_rows(sum, m);
  set_cols(sum, n);
  
  /* row major order */
  for (i = 1; i <= m; i++)
    {
      /* Unrolling the column operations is all that makes sense...otherwise
         locality in the cache will be disrupted. */
      for (j = 1; j <= n; j++)
	{
	  set_elem(sum, i, j, add_fp(get_elem(A, i, j), get_elem(B, i, j)));
	}
    }
}

/* returns AB via O(n^3) naive matrix multiplication time */
void naive_mat_mult(const struct matrix* A, const struct matrix* B, struct matrix* product)
{
  /* valid input error checking */
  is_valid(A);
  is_valid(B);

  if (!are_conformable(A,B))
    {
      fprintf(stderr, "Error: matrices to multiply are NOT comformable");
      exit(0);
    }
  /* end error checking */

  /* Clear out product and init */
  reset_matrix(product, get_rows(A), get_cols(B));
  size_t i,j,k;

  fp inner_sum = 0;

  /* bulk of function */
  for (i = 1; i <= get_rows(product); i++)
    {
      for(j = 1; j <= get_cols(product); j++)
	{
	  for (k = 1; k <= get_cols(A); k++)
	    {
	      inner_sum += mult_fp(get_elem(A,i,k), get_elem(B,k,j));
	    }

	  set_elem(product, i, j, inner_sum);
	  inner_sum = 0;
	}
    }
}

/* Given two matrices A and B , returns their row-wise concatenation A|B */
void row_concat(const struct matrix* A, const struct matrix* B, struct matrix* result)
{
  is_valid(A);
  is_valid(B);

  if (get_rows(A) != get_rows(B))
    {
      fprintf(stderr, "Error: matrices to concat are NOT correct dim");
      exit(0);
    }

  
  reset_matrix(result, get_rows(A), get_cols(A) + get_cols(B)); 
  size_t i,j;

  /* FIX THIS...LOCALITY COULD BE BETTER */
  for (i = 1; i <= get_rows(A); i++)
    {
    for (j = 1; j <= get_cols(A); j++)
      {
	set_elem(result, i, j, get_elem(A, i, j));
      }
    }

  for (i = 1; i <= get_rows(B); i++)
    {
      for(j = get_cols(A)+1; j <= get_cols(A) + get_cols(B); j++)
	{
	  set_elem(result, i, j, get_elem(B, i, j - get_cols(A)));
	}
    }
}

/* returns the input matrix but with the ith row multiplied by s.

   WARNING: This actually ALTERS the input, there's no additional 
   pointer reference passed in 
 */
inline void row_mult(struct matrix* inp, size_t i, fp s)
{
  is_valid(inp);

  if (i >= inp->rows)
    {
      fprintf(stderr, "Error: index out of bounds");
      exit(0);
    }

  size_t k;
  
  for (k = 1; k <= get_cols(inp); inp++)
    {
      set_elem(inp, i, k, get_elem(inp, i, k) * s);
    }
}


/* adds s*row_i to row_j 

   WARNING: This actually ALTERS the input, there's no additional 
   pointer reference passed in 
*/
inline void row_add_mult(struct matrix* inp, size_t i, size_t j, fp s)
{
  is_valid(inp);

  if (i >= get_rows(inp) || j >= get_rows(inp))
    {
      fprintf(stderr, "Error: index out of bounds");
      exit(0);
    }

  size_t k;

  for (k = 1; k <= get_cols(inp); inp++)
    {
      set_elem(inp, j, k, s*get_elem(inp, i, k) + get_elem(inp, j, k));
    }
}

/* switches row i with row j

   
   WARNING: This actually ALTERS the input, there's no additional 
   pointer reference passed in 
*/
inline void switch_row(struct matrix* inp, size_t i, size_t j)
{
  if (i >= inp->rows || j >= inp->rows)
    {
      fprintf(stderr, "Error: index out of bounds");
      exit(0);
    }

  size_t k;
  fp temp;

  for (k = 1; k <= get_cols(inp); k++)
    {
      temp = get_elem(inp, j, k);
      set_elem(inp, j, k, get_elem(inp, i, k));
      set_elem(inp, i, k, temp);
    }
}

/* Locates the first non-zero element in a column. 
   If none is found...returns -1. Note there *could* be an error here if
   the matrix column dimension is MAX_UNSIGNED_INT + 1 (make sure not to 
   allocate such enormous matrices) */
inline size_t 
locate_nonzero_col(const struct matrix* inp, size_t col, size_t starting_row)
 {

   is_valid(inp);
   size_t i = starting_row;

   while(i <= get_rows(inp))
     {
       // could have gotten rid of boolean operation but want clearer code 
       if (get_elem(inp, i, col) != 0)
	 return i;
       else
	 i++;
     }
 
   return -1;
 }

/* Given a matrix, returns its reduced row-echelon form using gauss-jordan elimination */
struct matrix Gauss_Jordan_Reduce(struct matrix mat);

/* Given a matrix, returns its rank */
int rank(struct matrix mat);

/* Given a matrix, returns solutions to the corresponding homogenous system. */
struct matrix solve_homog(struct matrix mat);

/* Given a matrix M and an appropriate vector b (encoded as a matrix), returns the solution 
   to Mx = b using a rudimentary method.  */
struct matrix basic_solve(struct matrix M, struct matrix b);

/* Computes the determinant of M */
fp det(struct matrix M);

/* The following implementations are optimized for the target machine:  */

/* Multi-threaded optimized transpose routine.

   -- Inner most caches of core i7 is 32 kB. With this in mind, we operate upon 
      16 kB of source matrix and 16 kB of the destination matrix at once

   -- There are four cores total, so we limit the computation to 4 threads maximum

 */
void transpose_i7(const struct matrix* A, struct matrix* B)
{
  /* error input checking. Need to make sure A and B are the correct sizes. */
  is_valid(A);

  size_t s_rows = get_rows(A);
  size_t s_cols = get_cols(A);
  size_t net_size = s_rows * s_cols;

  /* s and d are the indices for which "block" we are in for the source
     and destination matrices in the iteration */
  size_t s;
  size_t d;

  
  size_t i;
  size_t j;

  size_t min_s;
  size_t min_d;

  /* For each element i in the source matrix, we can reverse engineer the 
     corresponding row and col in the destination matrix. These variables
     will hold the results of this reverse engineering on each iteration. */
  size_t dest_row;
  size_t dest_col;
   
  /* We create this array then operate on it independently of 
     the accessor functions. We need to do this to optimize 
     Cache memory. This is one of the few instances in which 
     we ignore the safety measures, but in general
     attempt to avoid it. */
  reset_matrix(B, s_cols, s_rows);
  const fp* source = A->data;
  
  /* Loop over blocks for source array */
  for (s = 1; s <= net_size; s += (L1D_SIZE/2))
    {
      min_s = min(net_size, s + L1D_SIZE/2);

      /* Loop over blocks for destination array */
      for (d = 1; d <= net_size; d+= (L1D_SIZE/2))
	{
	  min_d = min(net_size, d + L1D_SIZE/2);

	  for (i = s; i <= min_s; i++)
	    {
	      dest_col = i / s_cols + 1;
	      dest_row = (i-1) % s_cols +1;

	      /* Since we are iterating over blocks we *do* need
	         to compute the absolute index j inside the d 
	         range. */
	      j = dest_col + (dest_row-1) * s_rows;

	      /* We make sure to limit our editing to the data that is
	         already cached, attempting to minimize thrashing. */
	      if (j <= min_d)
		{
		  set_elem(B, dest_row, dest_col, get_elem(A, dest_col, dest_row));
		}

	    }
	}
    }
}


/* Multiplies two matrices together using the Strassen algorithm.
   Optimized for the x1 carbon machine */
void strass_mat_mult_i7(struct matrix* result, const struct matrix* A, const struct matrix* B)
{
 
  
}

/* on going issues with C:

   --does not support function overloading on arguments
   --does not support operator overloading
   --no objects/classes (difficult to organize larger projects)
*/
