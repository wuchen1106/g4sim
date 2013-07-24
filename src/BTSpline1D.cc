// @(#) $Id: BTSpline1D.cc,v 1.1 2005/08/20 21:45:11 tjrob Exp $  $Name:  $
//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of     *
// * FERMILAB.                                                        *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// BTSpline1D.cc
//
// Created: Mark Fishler (5/00)
//

#ifdef BTSPLINE_MATHEMATICS

A 1-dimensional cubic spline is defined as a function which is in each of
some set of intervals is a cubic polynomials in the input coordinate which
matches the values and derivatives of some function being approximated.
These values and derivatives are in tables which remain const and characterise
the spline.

The first step is to find where in the table you are: Once you localize to an
interval, you work with the fractional distance dx from the lower end of the
interval to the actual point being evaluated.

The spline in 1 dimension consists of adding 4 terms based on x and the value
at each end or the derivative at each end.  If instead of gradients only values
are supplied, instead the spline uses second derivatives at the endpoints.
#endif

#include "BTSpline1D.hh"
#include <iostream>
#include <cstdlib>

BTSpline1D::BTSpline1D (const BTSpline1D &s): grads_(0), secondDerivs_(0) {
    extent_ = s.extent_;
    int i;
    nodePoints_ = new Data_t[extent_];
    distances_ = new Data_t[extent_];
    values_ = new Data_t[extent_];
    for (i=0; i<extent_; ++i) {
        nodePoints_[i] = s.nodePoints_[i];
    }
    for (i=0; i<extent_; ++i) {
        distances_[i] = s.distances_[i];
    }
    for (i=0; i<extent_; ++i) {
        values_[i] = s.values_[i];
    }
    if (s.grads_) {
        grads_ = new Data_t[extent_];
        for (i=0; i<extent_; ++i) {
            grads_[i] = s.grads_[i];
        }
    }
    if (s.secondDerivs_) {
        secondDerivs_ = new Data_t[extent_];
        for (i=0; i<extent_; ++i) {
            secondDerivs_[i] = s.secondDerivs_[i];
        }
    }

}


BTSpline1D::BTSpline1D(): extent_(0), nodePoints_(0),
values_(0), grads_(0), secondDerivs_(0){}

void BTSpline1D::captureGrid ( int extent, const Data_t* nodes ) {

    // Operations common to all the constructors of BTSpline1D:  Capture the
    // shape and nodes of the grid, and compute differences.

    int i;

    // Capture N_, extents, and nodes

    extent_ = extent;

    nodePoints_ = new Data_t[extent];
    for ( i = 0; i < extent_; i++ ) {
        nodePoints_[i] = nodes[i];
    }

    // Compute h_ -- the differences between consecutive grid points

    distances_ = new Data_t[extent];
    for ( i = 0; i < extent_-1; i++ ) {
        distances_[i] = nodePoints_[i+1] - nodePoints_[i];
        if ( distances_[i] <= 0 ) {
            std::cout <<
                "Spline1D initialized with nodePoints not monotonic increasing\n";
            std::abort();
        }
    }

}                                                     /* captureGrid() */


void BTSpline1D::captureValues ( int extent, const Data_t* values ) {
    values_ = new Data_t[extent_];
    const Data_t* valp  = values;
    Data_t* vp = values_;
    for ( int i=0; i < extent_; i++ ) {
        *vp++ = *valp++;
    }
}                                                     // captureValues


void BTSpline1D::fillValues ( int extent, double values_function(double x)) {
    values_ = new Data_t[extent_];
    double x;
    Data_t* vp = values_;
    for ( int i = 0; i < extent_; i++ ) {
        x = nodePoints_[i];
        *vp = values_function ( x );
        vp++;
    }
}                                                     // fillValues


void BTSpline1D::computeSecondDerivs () {

    secondDerivs_ = new Data_t[extent_];

    // Based on Numerical Recipes page 114.

#ifdef MATH

    For 1 <= j <= N-2, we have equations matching first derivatives at the
        node points.  These are

        distance[j-1]/6              * y[j-1]
        + (distance[j]+distance[j-1])/3 * y[j]
        +  distance[j]/6                * y[j+1]
        = dyRight/dxRight - dyLeft/dxLeft

        We also have, for "natural" boundary conditions, y2[0] = y2[N-1] = 0.
            (Or for specified slopes, a different relation, reflected in the elses.)

    So our equations are

                M Y2 = S

                where Y2 are the desired second derivatives,

                S is a vector {0, (dyRight/dxRight - dyLeft/dxLeft) from 1...N-2, 0}
    or for slope boundaries,
            S[0]   = 3 * ( (y[1]-y[0])/(x[1]-x[0]) - v )         / (x[1]-x[0])      = S(v)
            S[N-1] = 3 * ( w - (y[N-1]-y[N-2])/(x[N-1]-x[N-2]) ) / (x[N-1]-x[N-2])  = S(w)

            and M is a matrix of the form

            1      0 or .5
            d0/6  (d0+d1)/3  d1/6
            d1/6   (d1+d2)/3  d2/6
            d2/6   (d2+d3)/3  d3/6
            ...
            dN-3/6 (dN-3+DN-2)/3 DN-2/3
            0 or .5       1

            Step 1 in solving this tridiagonal system is to eliminate the lower part.
            That only affects the first rank above the diagonal; we will call the
        affected rank u.  (u is a vector with indices ranging from 0 to N-2 (not N-1)).
        By inspection of M, u[0] starts out as 0 for our natural condition.

            We will also hold S in the place where we want y2 to end up.  S[0] starts out
            as 0 as well, for the natural condition.

                We will not form u in advance, nor form the below-diagonal, because they are
                simple expressions in terns of the distances.  For each row, we get a new
                pivot from the original diagonal element modified by the u above it when
                eliminating the below-diagonal term, and a new y2 similarly, and then we divide
                thru by the pivot.
#endif

    // Decompose the tridiagonal matrix representing the conditions of
    // continuous second derivative.

                double* u = new double[extent_];

    Data_t* y = values_;                              // aliases just to shorten the expressions.
    Data_t* y2 = secondDerivs_;
    int N = extent_;

    if (naturalBoundaryConditions) {
        y2[0] = u[0] = 0;
    }
    else {
        y2[0] = 3 * ( (y[1]-y[0])/distances_[0] - slope0_ ) / distances_[0];
    }

    double dxLeft;
    double dyLeft;
    double dxRight;
    double dyRight;
    double pivot;
    for ( int i=1; i < N-1; ++i ) {
        dxLeft  = distances_[i-1];
        dxRight = distances_[i];
        dyLeft  = y[i]  - y[i-1];
        dyRight = y[i+1]- y[i];
        pivot   = (dxLeft+dxRight)/3.0 - dxLeft * u[i-1] / 6.0;
        y2[i]   = dyRight/dxRight - dyLeft/dxLeft - dxLeft * y2[i-1] / 6.0;
        u[i]    = dxRight / (6.0 * pivot);
        y2[i]   = y2[i] / pivot;
    }

    double lower;                                     // The element one to the left of the bottom right of M
    if (naturalBoundaryConditions) {
        y2[N-1] = 0;
        lower = 0;
    }
    else {
        y2[N-1] =
            3 * ( slopeN_ - (y[N-1]-y[N-2])/distances_[N-2] ) / distances_[N-2];
        lower = .5;
    }
    pivot    = 1 - lower*u[N-2];
    y2[N-1] -= lower * y2[N-2];
    y2[N-1]  = y2[N-1] / pivot;

    // Backsolve that decomposition to get the second dervatives.

    for ( int k=N-2; k >= 0; --k) {
        y2[k] -= u[k] * y2[k+1];
    }

    // secondDerivs_ is already y2, and we no longer need u

    delete[] u;

    return;
}


// -------------------------
//
// Spline setup for type A definition
// (user supplies only function values;
// continuity of second derivative is imposed)
//
// --------------------------

BTSpline1D::BTSpline1D (
int extent,                                           // Number of node points
const Data_t* nodes,                                  // Locations in x of nodes
const Data_t* values                                  // Values being approximated
) : grads_(0), naturalBoundaryConditions (true) {

    captureGrid ( extent, nodes );
    captureValues ( extent, values );
    computeSecondDerivs ();

}                                                     // BTSpline1D


BTSpline1D::BTSpline1D (
int extent,                                           // Number of node points
const Data_t* nodes,                                  // Locations in x of nodes
double values_function (double x)                     // Function being approximated
) : grads_(0), naturalBoundaryConditions (true) {

    captureGrid ( extent, nodes );
    fillValues (extent, values_function);
    computeSecondDerivs ();

}                                                     // BTSpline1D


BTSpline1D::BTSpline1D (
int extent,                                           // Number of node points
const Data_t* nodes,                                  // Locations in x of nodes
const Data_t* values,                                 // Values being approximated
Data_t slope0, Data_t slopeN
) : grads_(0), naturalBoundaryConditions (false),
slope0_ (slope0),  slopeN_ (slopeN) {

    captureGrid ( extent, nodes );
    captureValues ( extent, values );
    computeSecondDerivs ();

}                                                     // BTSpline1D


BTSpline1D::BTSpline1D (
int extent,                                           // Number of node points
const Data_t* nodes,                                  // Locations in x of nodes
double values_function (double x),                    // Function being approximated
Data_t slope0, Data_t slopeN
) : grads_(0), naturalBoundaryConditions (false),
slope0_ (slope0),  slopeN_ (slopeN) {

    captureGrid ( extent, nodes );
    fillValues (extent, values_function);
    computeSecondDerivs ();

}                                                     // BTSpline1D


// -------------------------
//
// Spline setup for type B definition (user supplies gradients)
//
// --------------------------

BTSpline1D::BTSpline1D (
int extent,                                           // Number of node points
const Data_t* nodes,                                  // Locations in x of nodes
const Data_t* values,                                 // values at each node point
const Data_t* gradients                               // derivatives at each node point
// nodes, values, gradients are arrays
// of extent Data_ts.
) : secondDerivs_(0) {

    captureGrid ( extent, nodes );

    // Capture values and grads by copying the arrays.
    //-| This could be sped up by the use of memcopy,
    //-| but is a one-shot deal so I opt for maximal clarity.

    const Data_t* valp  = values;
    const Data_t* gradp = gradients;
    Data_t* vp = values_;
    Data_t* gp = grads_;
    for ( int i=0; i < extent_; i++ ) {
        *vp++ = *valp++;
        *gp++ = *gradp++;
    }

}                                                     // BTSpline1D - separate tables for values, grads


BTSpline1D::BTSpline1D (
int extent,                                           // Number of node points
const Data_t* nodes,                                  // Locations in x of nodes
void values_grads_function (
double x,
double* val,
double* grads)                                        // Function filling values and
// gradient for input x
) : secondDerivs_(0) {

    captureGrid ( extent, nodes );

    // Fill values and grads arrays by calling the supplied function across all
    // the nodes.

    float x;
    Data_t* vp = values_;
    Data_t* gp = grads_;
    double vpTemp;
    double gpTemp;
    for ( int i = 0; i < extent_; i++ ) {
        x = nodePoints_[i];
        values_grads_function ( x, &vpTemp, &gpTemp );
        *vp++ = vpTemp;
        *gp++ = gpTemp;
    }

}                                                     // BTSpline1D - function returning value and grad supplied


BTSpline1D::~BTSpline1D () {

    delete[] nodePoints_;
    delete[] distances_;
    delete[] values_;
    if (grads_) delete[] grads_;
    if (secondDerivs_) delete[] secondDerivs_;

}                                                     // Destructor


double BTSpline1D::operator() ( double x ) const {

    // Find the interval that x lies within.  This has a lower coordinate
    // of origin.  Also compute the f0, f1, g0 and g1 functions.
    // (See mathematics comment at end of this file):
    //-| This step will point to the exactly matching point if there is one,
    //-| and to the first or next-to-last point if out of range.

    double f0;
    double f1;
    double g0;
    double g1;

    int a = 0;                                        // Highest node value known NOT to exceed x.
    //   Will end up as highest node which
    //   DOES not exceed x.
    int b = extent_ - 1;                              // Lowest node value which must exceed xd,
    //   assuming x is not outside the range.

    while (b != (a+1) ) {
        int c = (a+b+1)>>1;
        if (x > nodePoints_[c]) {
            a = c;
        }
        else {
            b = c;
        }
    }

    // Now use either the f0,f1, g0, g1 algorithm if gradients are knowen,
    // or the method in Numerical Recipes if second derivs have been computed:

    double spline;

    double dx;
    double oneMinusDx;

    dx = (x - nodePoints_[a])/distances_[a];
    oneMinusDx  = 1 - dx;

    if ( grads_ ) {

        double oneMinusX2;
        double x2;

        x2 = dx * dx;
        oneMinusX2 = oneMinusDx * oneMinusDx;

        f0 = (2. * dx + 1.) * oneMinusX2;
        f1 = (3. - 2. * dx) * x2;
        g0 =  distances_[a] * dx * oneMinusX2;
        g1 = -distances_[a] * oneMinusDx * x2;

        // Sum these F and G elements times the corresponding value (for F) and
        // gradient in each direction (for G) to get the answer.

        spline = f0 * values_[a] + f1 * values_[a+1]
            + g0 * grads_[a]  + g1 * grads_[a+1] ;

    }
    else {
        double spacingFactor = distances_[a] * distances_[a] / 6.0;
        double leftCubic  = (dx*dx*dx - dx) * secondDerivs_[b];
        double rightCubic =
            (oneMinusDx*oneMinusDx*oneMinusDx-oneMinusDx) * secondDerivs_[a];
        spline =   dx * values_[b] + oneMinusDx * values_[a]
            + (leftCubic + rightCubic) * spacingFactor;
    }

    return spline;

}                                                     /* operator() */


#ifdef BTSPLINE_MATHEMATICS

The spline in 1 dimensions consists of adding 4 terms, each of
which is a cubic polynomials in dx times a value or gradient component
of the function phi at one of ends of the interval.

These polynomials are, for the non-gradient terms,

f0(x) = (2*x +1) * (1-x)**2
f1(x) = (3 - 2*x) * x**2

f0 refers to the lower end and f1 to the upper end; notice that these
are linear with some correction which near 1 varies slowly for f1, and near
0 varies slowly for f0.

For the gradient terms, the polynomials contain a factor of the size h of
the interval:

g0(x) =   h * x * (1-x)**2
g1(x) = - h * (1-x) * x**2

These are found in about 15 operations a given value of dx.

Adding these four gives an interpolation which matches both the function and
its first derivation at both ends of the interval.
#endif
