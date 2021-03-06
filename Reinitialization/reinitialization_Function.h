#include "reinitialization_class.h"


///////////////////////////////////////////////////////////////////////////////////
//
//			sign function
//
//////////////////////////////////////////////////////////////////////////////////
double sign_func(double a)
{
	if (abs(a)<DBL_EPSILON)
	{
		return a=0;
	}
	else
	{
		return a/abs(a);
	}
}


///////////////////////////////////////////////////////////////////////////////////
//
//			MINMOD function
//
//////////////////////////////////////////////////////////////////////////////////
double minmod(double a, double b)
{
	if (a*b<0)
	{
		return 0;
	}
	else if (abs(a)>=abs(b))
	{
		return b;
	}
	else
	{
		return a;
	}
}

///////////////////////////////////////////////////////////////////////////////////
//
//			(a)+, (a)- function
//
//////////////////////////////////////////////////////////////////////////////////
double plus_function(double a)
{
	return max(a,0.0);
}


double minus_function(double a)
{
	return min(a,0.0);
}



///////////////////////////////////////////////////////////////////////////////////
//
//			Godunov Hamiltonian
//
//////////////////////////////////////////////////////////////////////////////////
double HG(double a, double b, double c, double d, double signal)
{
	double temp;
	double result;

	if (signal>=0)
	{
		temp = max(minus_function(a)*minus_function(a),plus_function(b)*plus_function(b))+max(minus_function(c)*minus_function(c),plus_function(d)*plus_function(d));
		result = sqrt(temp);
	}
	else
	{
		temp = max(plus_function(a)*plus_function(a),minus_function(b)*minus_function(b))+max(plus_function(c)*plus_function(c),minus_function(d)*minus_function(d));
		result = sqrt(temp);
	}

	return result;
}



///////////////////////////////////////////////////////////////////////////////////
//
//			second derivation function
//
//////////////////////////////////////////////////////////////////////////////////
double derivation2x(int i,int j, double** A, double delta , int max_num)
{
	double result;

	if (i>0 && i<(max_num-1))
	{
		result = A[i-1][j]-2.0*A[i][j]+A[i+1][j];
	}
	else if (i==0)
	{
		result = A[i][j]-2.0*A[i+1][j]+A[i+2][j];
	}
	else if (i==(max_num-1))
	{
		result = A[i][j]-2.0*A[i-1][j]+A[i-2][j];
	}
	else
	{
		result = 0;
	}

	result = result/(delta*delta);
	return result;

}


double derivation2y(int i,int j, double** A, double delta , int max_num)
{
	double result;

	if (j>0 && j<(max_num-1))
	{
		result = A[i][j-1]-2.0*A[i][j]+A[i][j+1];
	}
	else if (j==0)
	{
		result = A[i][j]-2.0*A[i][j+1]+A[i][j+2];
	}
	else if (j==(max_num-1))
	{
		result = A[i][j]-2.0*A[i][j-1]+A[i][j-2];
	}
	else
	{
		result = 0;
	}

	result = result/(delta*delta);
	return result;



}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///       computing Dx+,Dy+.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// first derivation plus function 
// (difference value, delta plus)
tuple<double,double> derivation1x_plus(int i,int j, double** A, double delta, int max_num)
{
	double result;
	double Aij;

	// calculate D+xPSI_ij
	Aij=A[i][j];
	if (i<max_num-1)
	{
		result = (A[i+1][j] -A[i][j])/delta - delta/2.0*minmod(derivation2x(i,j,A,delta, max_num),derivation2x(i+1,j,A,delta,max_num));
	}
	else
	{
		result = (A[i][j] -A[i-1][j])/delta - delta/2.0*minmod(derivation2x(i,j,A,delta,max_num),derivation2x(i+1,j,A,delta,max_num));

	}
	return make_tuple(result, delta);
}


tuple<double,double> derivation1y_plus(int i,int j, double** A, double delta,  int max_num)
{
	double result;
	double Aij;

	// calculate D+xPSI_ij
	Aij=A[i][j];
	if (j<max_num-1)
	{
		result = (A[i][j+1]-A[i][j])/delta - delta/2.0*minmod(derivation2y(i,j,A,delta,max_num),derivation2y(i,j+1,A,delta,max_num));
	}
	else
	{
		result = (A[i][j]-A[i][j-1])/delta - delta/2.0*minmod(derivation2y(i,j,A,delta,max_num),derivation2y(i,j+1,A,delta,max_num));

	}
	return make_tuple(result, delta);

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///      computing Dx-,Dy-.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// first derivation minus function
// (difference value, delta minus)
tuple<double,double> derivation1x_minus(int i,int j, double** A, double delta, int max_num)
{
	double result;
	double Aij;

	// calculate D+xPSI_ij
	Aij=A[i][j];
	if (i>0)
	{
		result = (A[i][j]-A[i-1][j])/delta + delta/2.0*minmod(derivation2x(i,j,A,delta,max_num),derivation2x(i-1,j,A,delta,max_num));
	}
	else
	{
		result = (A[i+1][j]-A[i][j])/delta +delta/2.0*minmod(derivation2x(i,j,A,delta,max_num),derivation2x(i-1,j,A,delta,max_num));
	}
	return make_tuple(result, delta);
}

tuple<double,double> derivation1y_minus(int i,int j, double** A, double delta,  int max_num)
{
	double result;
	double Aij;

	// calculate D+xPSI_ij
	Aij=A[i][j];
	if (j>0)
	{
		result = (A[i][j]-A[i][j-1])/delta + delta/2.0*minmod(derivation2y(i,j,A,delta,max_num),derivation2y(i,j-1,A,delta,max_num));
	}
	else
	{
		result = (A[i][j+1]-A[i][j])/delta + delta/2.0*minmod(derivation2y(i,j,A,delta,max_num),derivation2y(i,j-1,A,delta,max_num));
	}

	return make_tuple(result, delta);
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///       Near interface, computing Dx+,Dy+.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// first derivation plus function near interface
// (difference value, delta plus)
tuple<double,double> derivation1x_plus_subcell(int i,int j, double** A, double delta, int max_num)
{
	double delta_plus;
	double temp1;
	double temp2;
	double psi0_xxyy;
	double discriminant;
	double derivation;

	if (i>0 && i<(max_num-2))
	{
		temp1 = A[i-1][j]-2.0*A[i][j]+A[i+1][j];
		temp2 = A[i][j]-2.0*A[i+1][j]+A[i+2][j];
		psi0_xxyy = minmod(temp1,temp2);
	}
	else
	{
		psi0_xxyy = 0;
	}

	// Calculate deltaX+
	if (abs(psi0_xxyy) > 10.0*DBL_EPSILON) // almost linear case
	{

		if (i<max_num-1)
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i+1][j])*(psi0_xxyy/2.0-A[i][j]-A[i+1][j]) - 4.0*A[i][j]*A[i+1][j] ;
			discriminant = abs(discriminant);
			delta_plus = delta * (1.0/2.0 + (A[i][j]-A[i+1][j]-sign_func(A[i][j]-A[i+1][j])*sqrt(discriminant))/psi0_xxyy);
		}
		else
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i][j])*(psi0_xxyy/2.0-A[i][j]-A[i][j]) - 4.0*A[i][j]*A[i][j] ;
			discriminant = abs(discriminant);
			delta_plus = delta * (1.0/2.0);
		}
	}
	else // non linear case
	{
		if (i<max_num-1)
		{
			delta_plus = delta * A[i][j]/(A[i][j]-A[i+1][j]);

		}
		else
		{
			delta_plus = delta * (1.0/2.0);
		}
	}

	// calculate D+xPSI_ij
	derivation = -A[i][j]/delta_plus - delta_plus/2.0*minmod(derivation2x(i,j,A,delta,max_num),derivation2x(i+1,j,A,delta,max_num));
	return make_tuple(derivation, delta_plus);
}


tuple<double,double> derivation1y_plus_subcell(int i,int j, double** A, double delta, int max_num)
{
	double delta_plus;
	double temp1;
	double temp2;
	double psi0_xxyy;
	double discriminant;
	double derivation;

	if (j>0 && j<max_num-2)
	{
		temp1 = A[i][j-1]-2.0*A[i][j]+A[i][j+1];
		temp2 = A[i][j]-2.0*A[i][j+1]+A[i][j+2];
		psi0_xxyy = minmod(temp1, temp2);
	}
	else
	{
		psi0_xxyy = 0;
	}

	// Calculate deltaX+
	if (abs(psi0_xxyy) > 10.0*DBL_EPSILON) // almost linear case
	{

		if (j<max_num-1)
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i][j+1])*(psi0_xxyy/2.0-A[i][j]-A[i][j+1]) - 4.0*A[i][j]*A[i][j+1] ;
			discriminant = abs(discriminant);
			delta_plus = delta * (1.0/2.0 + (A[i][j]-A[i][j+1]-sign_func(A[i][j]-A[i][j+1])*sqrt(discriminant))/psi0_xxyy);
		}
		else
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i][j])*(psi0_xxyy/2.0-A[i][j]-A[i][j]) - 4.0*A[i][j]*A[i][j] ;
			discriminant = abs(discriminant);
			delta_plus = delta * (1.0/2.0);
		}
	}
	else // non linear case
	{
		if (j<max_num-1)
		{
			delta_plus = delta * A[i][j]/(A[i][j]-A[i][j+1]);
		}
		else
		{
			delta_plus = delta *1.0/2.0;
		}
	}

	// calculate D+xPSI_ij
	derivation = -A[i][j]/delta_plus - delta_plus/2.0*minmod(derivation2y(i,j,A,delta,max_num),derivation2y(i,j+1,A,delta,max_num));
	return make_tuple(derivation, delta_plus);
}







////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///       Near interface, computing Dx-,Dy-.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// first derivation minus function
// (difference value, delta minus)
tuple<double,double> derivation1x_minus_subcell(int i,int j, double** A, double delta, int max_num)
{
	double delta_minus;
	double temp1;
	double temp2;
	double psi0_xxyy;
	double discriminant;
	double derivation;

	if (i>1 && i<max_num-1)
	{
		temp1 = A[i-1][j]-2.0*A[i][j]+A[i+1][j];
		temp2 = A[i][j]-2.0*A[i-1][j]+A[i-2][j];
		psi0_xxyy = minmod(temp1,temp2);
	}
	else
	{
		psi0_xxyy = 0;
	}

	// Calculate deltaX+
	if (abs(psi0_xxyy) > 10.0*DBL_EPSILON) // almost linear case
	{

		if (i>0)
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i-1][j])*(psi0_xxyy/2.0-A[i][j]-A[i-1][j]) - 4.0*A[i][j]*A[i-1][j] ;
			discriminant = abs(discriminant);
			delta_minus = delta * (1.0/2.0 + (A[i][j]-A[i-1][j]-sign_func(A[i][j]-A[i-1][j])*sqrt(discriminant))/psi0_xxyy);
		}
		else
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i][j])*(psi0_xxyy/2.0-A[i][j]-A[i][j]) - 4.0*A[i][j]*A[i][j] ;
			discriminant = abs(discriminant);
			delta_minus = delta * (1.0/2.0 );
		}
	}
	else // non linear case
	{
		if (i>0)
		{
			delta_minus = delta * A[i][j]/(A[i][j]-A[i-1][j]);
		}
		else
		{
			delta_minus = delta * 1.0/2.0;
		}
	}

	// calculate D+xPSI_ij

	derivation = A[i][j]/delta_minus + delta_minus/2.0*minmod(derivation2x(i,j,A,delta,max_num),derivation2x(i-1,j,A,delta,max_num));
	return make_tuple(derivation, delta_minus);
}


tuple<double,double> derivation1y_minus_subcell(int i,int j, double** A, double delta, int max_num)
{
	double delta_minus;
	double temp1;
	double temp2;
	double psi0_xxyy;
	double discriminant;
	double derivation;
	if (j>1 && j<max_num-1)
	{
		temp1 = A[i][j-1]-2.0*A[i][j]+A[i][j+1];
		temp2 = A[i][j]-2.0*A[i][j-1]+A[i][j-2];
		psi0_xxyy = minmod(temp1, temp2);
	}
	else
	{
		psi0_xxyy = 0;
	}

	// Calculate deltaX+
	if (abs(psi0_xxyy) > 10.0*DBL_EPSILON) // almost linear case
	{

		if (j>0)
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i][j-1])*(psi0_xxyy/2.0-A[i][j]-A[i][j-1]) - 4.0*A[i][j]*A[i][j-1] ;
			discriminant = abs(discriminant);
			delta_minus = delta * (1.0/2.0 + (A[i][j]-A[i][j-1]-sign_func(A[i][j]-A[i][j-1])*sqrt(discriminant))/psi0_xxyy);
		}
		else
		{
			discriminant = (psi0_xxyy/2.0-A[i][j]-A[i][j])*(psi0_xxyy/2.0-A[i][j]-A[i][j]) - 4.0*A[i][j]*A[i][j] ;
			discriminant = abs(discriminant);
			delta_minus = delta * (1.0/2.0);
		}
	}
	else // non linear case
	{
		if (j>0)
		{
			delta_minus = delta * A[i][j]/(A[i][j]-A[i][j-1]);
		}
		else
		{
			delta_minus = delta * 1.0/2.0;
		}
	}

	// calculate D+xPSI_ij

	derivation = A[i][j]/delta_minus - delta_minus/2.0*minmod(derivation2y(i,j,A,delta,max_num),derivation2y(i,j-1,A,delta,max_num));

	return make_tuple(derivation, delta_minus);

}


