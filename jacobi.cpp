#include "stdafx.h"

#include  <stdlib.h>
#include  <stdio.h>
#include  <assert.h>
#include  <memory.h>
#include  <math.h> 
#include "jacobi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau); \
              a[k][l]=h+s*(g-h*tau);

/* Computes all eigenvalues and eigenvectors of a real symmetric
 * matrix a[1..n][1..n]. On output, elements of a above the diagonal
 * are destroyed. d[1..n] returns the eigenvalues of a. v[1..n][1..n]
 * is a matrix whose columns contain, on output, the normalized
 * eigenvectors of a. nrot returns the number of Jacobi rotations that
 * were required.  
 */

double  * vectorFuncTion(int  a, int  size)
{
    double  * vec;
    int  mem_size;

    mem_size = sizeof( double ) * (size + 1);

    vec = (double *)malloc( mem_size );
    assert( vec != NULL );

    memset( vec, 0, mem_size );

    return  vec;
}


void   free_vector( double  * z, int  a, int  b )
{
    (void)a;
    (void)b;

    free( z );
}


double  ** create_matrix( int  n )
{
    double  ** vec;

    vec = (double **)malloc( sizeof( double ** ) * (n + 1) );
    assert( vec != NULL );

    vec[ 0 ] = NULL;
    for  ( int  ind = 1; ind <= n; ind++ )
		vec[ind] = vectorFuncTion(1, n);

    return  vec;
}


void   free_matrix( double ** vec, int  n )
{

    for  ( int  ind = 1; ind <= n; ind++ )
        free( vec[ ind ] );
    free( vec );
}


void jacobi(double **a, int n, double d[], double **v, int *nrot) 
{ 
    int j,iq,ip,i; 
    double tresh,theta,tau,t,sm,s,h,g,c,*b,*z; 

    b=vectorFuncTion(1,n); 
	z = vectorFuncTion(1, n);

    /* Initialize to the identity matrix. */            
    for (ip=1;ip<=n;ip++) 
	{       
        for (iq=1;iq<=n;iq++) 
            v[ip][iq]=0.0;
        v[ip][ip]=1.0; 
    } 

    /* Initialize b and d to the diagonal of a. */
    for (ip=1;ip<=n;ip++) 
	{
        b[ip]=d[ip]=a[ip][ip]; 
        z[ip]=0.0; 
        /* z - This vector will accumulate terms of the form tapq as
           in equa- tion (11.1.14). */
    } 

    *nrot=0; 

    for (i=1;i<=50;i++) 
	{
        sm=0.0; 
        /* Sum off-diagonal elements. */
        for (ip=1;ip<=n-1;ip++) {
            for (iq=ip+1;iq<=n;iq++) 
                sm += fabs(a[ip][iq]); 
        } 

        /*The normal return, which relies on quadratic convergence to
          machine underflow.*/
        if (sm == 0.0)
		{
             free_vector(z,1,n);
             free_vector(b,1,n); 
             return; 
        } 

        if (i < 4) 
            tresh=0.2*sm/(n*n); /*...on the  rst three sweeps. */ 
        else 
            tresh=0.0; /*...thereafter. */
        
        for  ( ip = 1;ip <= n-1; ip++) 
		{
            for (iq=ip+1;iq<=n;iq++)
			{
                g=100.0*fabs(a[ip][iq]); 
                /* After four sweeps, skip the rotation if the off
                   diagonal element is small.*/

                if  ( ( i > 4 )  
                      &&  ((double)(fabs(d[ip])+g) == (double)fabs(d[ip]))
                      && ((double)(fabs(d[iq])+g) == (double)fabs(d[iq])))
                    a[ip][iq]=0.0;
                else 
                    if (fabs(a[ip][iq]) > tresh) 
					{
                        h=d[iq]-d[ip]; 
                        if ((double)(fabs(h)+g) == (double)fabs(h))
                            t = (a[ip][iq])/h; /* t = 1=(2 ) */
                        else {
                            theta=0.5*h/(a[ip][iq]); /* Equation (11.1.10).*/
                            t=1.0/(fabs(theta)+sqrt(1.0+theta*theta)); 
                            if (theta < 0.0) 
                                t = -t; 
                        } 
                        c=1.0/sqrt(1+t*t); 
                        s=t*c; 
                        tau=s/(1.0+c);
                        h=t*a[ip][iq];
                        z[ip] -= h;
                        z[iq] += h;
                        d[ip] -= h;
                        d[iq] += h; 
                        a[ip][iq]=0.0; 
                        for (j=1;j<=ip-1;j++) 
						{
                            /* Case of rotations 1   j <p.*/
                            ROTATE(a,j,ip,j,iq);
                        } 
                        for (j=ip+1;j<=iq-1;j++) 
						{
                            /* Case of rotations p <j<q. */
                            ROTATE(a,ip,j,j,iq) ;
                        }
                        for (j=iq+1;j<=n;j++)
						{
                            /* Case of rotations q <j   n. */
                            ROTATE(a,ip,j,iq,j);
                        } 
                        for (j=1;j<=n;j++) {
                            ROTATE(v,j,ip,j,iq); 
                        } 
                        ++(*nrot); 
                    } 
            } 
        } 
        for (ip=1;ip<=n;ip++)
		{
            b[ip] += z[ip]; 
            d[ip]=b[ip]; 
            /*Update d with the sum of tapq,*/
            z[ip]=0.0;/* and reinitialize z. */
        }
    } 
    fflush( stdout );
    fprintf( stderr, "Too many iterations in routine jacobi\n"); 
    fflush( stderr );
    exit( -1 );
}



/* Given the eigenvalues d[1..n] and eigenvectors v[1..n][1..n] as
 * output from jacobi ( x 11.1) or tqli ( x 11.3), this routine
 * sorts the eigenvalues into descending order, and rearranges the
 * columns of v correspondingly. The method is straight insertion.
\*/

void eigsrt(double d[], double **v, int n) 
{
    int   k,j,i;
    double   p;

    for  ( i = 1; i < n; i++ )
	{
        p = d[k = i];
        
        for (j=i+1;j<=n;j++) 
            if (d[j] >= p) 
                p=d[k=j];
        if (k != i)
		{ 
            d[k]=d[i];
            d[i]=p;
            for (j=1;j<=n;j++)
			{ 
                p=v[j][i]; 
                v[j][i]=v[j][k];     
				v[j][k]=p; 
            } 
        } 
    } 
}


void  Jacobi_EigenSymmetric3( double  mat[3][3],  double  eval[3],  double  evec[3][3] )
{
    double  ** p_mat = create_matrix( 3 );
    double  ** p_out_vec = create_matrix( 3 );
    double  eigen_vals[ 10 ];
    int  rot = 0;

    for  ( int  ind = 0; ind < 3; ind++ )
        for  ( int  jnd = 0; jnd < 3; jnd++ )
            p_mat[ ind + 1 ][ jnd + 1 ] = mat[ ind ][jnd ];
    
        
    jacobi( p_mat, 3, eigen_vals, p_out_vec, &rot );
    eigsrt( eigen_vals, p_out_vec, 3 );

    eval[ 0 ] = eigen_vals[ 1 ];
    eval[ 1 ] = eigen_vals[ 2 ];
    eval[ 2 ] = eigen_vals[ 3 ];

    for  ( int  ind = 0; ind < 3; ind++ )
        for  ( int  jnd = 0; jnd < 3; jnd++ )
            evec[ ind ][ jnd ] = p_out_vec[ jnd + 1 ][ ind + 1 ];

    free_matrix( p_mat, 3 );
    free_matrix( p_out_vec, 3 );
}
