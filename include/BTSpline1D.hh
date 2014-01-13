// @(#) $Id: BTSpline1D.hh,v 1.1 2005/08/20 21:45:11 tjrob Exp $  $Name:  $
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
// BTSpline1D.hh
//
// Created: Mark Fishler (5/00)
//

#ifndef BTSPLINE1D_H
#define BTSPLINE1D_H

// The BTSpline1D class represents a table-driven cubic spline in 1 dimension.
// The user supplies the table or the function to the constructor; thereafter,
// the spline is invoked as if it were a simple function of the input value x,
// as in
//
//     double answer = mySpline ( x );
//
// The spline requires (or pre-computes) values of gradient at each point in
// the table.  For cases where the gradient is known analytically, these may be
// supplied directly.  Normally, the spline would be fed just the function or
// values at the grid points; the gradients are pre-computed from the condition
// that the function has a continuous second derivative.

// A related class is BTSpline, for an N-dimensional cubic spline taking
// an array instead of a value for x.  The general BTSpline class is correct
// for 1 dimension, but involves a bit of extra overhead, and the inconvenience
// of requiring a pointer to the value x (rather than the value itself) because
// it will treat x[] as an array of dimension 1.

// See BTSpline.h.

#ifdef BTSpline_DESIGN

The BTSpline1D class is an object representing the transformation between some
value x and a value phi, based on tables of values and gradients (derivatives).
The values and gradients are computed (or supplied) at a set of points
(we call these node points) upon construction. These node points may be
irregularly spaced.

From then on, the user can invoke the () operator supplying the value of x;
the operator returns the value of the function phi, based on an interpolating
function which exactly matches the values and slopes at every node point.

A given instance of a BTSpline is constructed from:

(1) An integer extent of the array defining the node points.
(2) An array of N numbers representing the coordinates of each node points.
(3) The values and optionally the gradients of phi at those node points.

For convenience, the value and gradient information can be supplied in
any of several ways:

* A pointer to a function returning a double which takes a double x.  If this
is supplied, then the constructor will call the function however many times
to fill in the tables of values at the node points.  It will then compute
the gradients needed for continuity in the second derivative, using "natural"
boundary conditions (second derivative zero at both ends).

* A single table containing the value for each point.  Again, derivatives are
comuted using "natural" boundary conditions.

* Either a pointer to a function or a table, as above, followed by two
additional arguments which represent the values  of slope at the beginnng
and end boundaries;

* A table of values and a separate table of one gradients (derivative) per
point.

* A pointer to a function returning a void, which takes a double x and
two pointers to value and gradient.  If this is supplied, then the
constructor will call the function however many times to fill in the
tables of values and gradients at the node points.

The tables are captured in the class instance data, and may be discarded after
construction.

The only user-relevant method other than the constructor is
double operator() ( double x ) const;
that is, supplying a x for the desired sample point and receiving a double
value
of phi.  The value returned is a cubic function in each interval which will
match the values and the gradients at every node.

The class does NOT verify that x lies within the domain defined by the node
points; if it is outside, then the value obtained is still a valid
approximation, but one which will get worse as the distance from the nearest
node grows large.
#endif

class BTSpline1D {

    // The usage of the spline, and any functions provided, are based on doubles.
    // However, for space purposes, the class might be reconfigured to store (and
    // accept) tables in the float format.

    typedef double Data_t;                            // In the future we MAY want to template off
    // the type of table data; using Data_t for
    // this purpose will make this easy.

    public:

        // Invoke the spline to get a value:

        double operator() ( double x ) const;

        // (Deep) copy constructor

        BTSpline1D ( const BTSpline1D & s );
        BTSpline1D (  );

        // Constructors A -- the class accepts value info in two ways, and optionally
        //		       accepts boundary slope info (if that is not provided,
        //		       then the second derivative is set to 0 at boundaries).

#ifdef FUTURE

        BTSpline1D (
            const std::vector<Data_t> nodes,          // Locations in x of nodes
            const std::vector<Data_t> values          // Values being approximated
            );

        BTSpline1D (
            const std::vector<Data_t> nodes,          // Locations in x of nodes
            const std::vector<Data_t> values,         // Values being approximated
            Data_t slope0, Data_t slopeN
            );
#endif                                            // FUTURE

        BTSpline1D (
            int extent,                               // Number of node points
            const Data_t* nodes,                      // Locations in x of nodes
            const Data_t* values                      // Values being approximated
            );

        BTSpline1D (
            int extent,                               // Number of node points
            const Data_t* nodes,                      // Locations in x of nodes
            double values_function (double x)         // Function being approximated
            );

        BTSpline1D (
            int extent,                               // Number of node points
            const Data_t* nodes,                      // Locations in x of nodes
            const Data_t* values,                     // Values being approximated
            Data_t slope0, Data_t slopeN
            );

        BTSpline1D (
            int extent,                               // Number of node points
            const Data_t* nodes,                      // Locations in x of nodes
            double values_function (double x),        // Function being approximated
            Data_t slope0, Data_t slopeN
            );

        // Constructors B -- the class accepts value and gradient info in three ways:

#ifdef FUTURE

        BTSpline1D (
            const std::vector<Data_t> nodes,          // Locations in x of nodes
            const std::vector<Data_t> values,         // Values at each node point
            const std::vector<Data_t> gradients       // derivatives at each node point
            );

        // And some appropriate form for passing a values/grads function.
#endif                                            // FUTURE

        BTSpline1D (
            int extent,                               // Number of node points
            const Data_t* nodes,                      // Locations in x of nodes
            const Data_t* values,                     // values at each node point
            const Data_t* gradients                   // derivatives at each node point
        // nodes, values, gradients are arrays
        // of extent Data_ts.
            );

        BTSpline1D (
            int extent,                               // Number of node points
            const Data_t* nodes,                      // Locations in x of nodes
            void values_grads_function (
            double x,
            double* val,
            double* grads)
        // Function filling value and
        // gradient for input x
            );

        ~BTSpline1D();

    protected:

        // member data:

        int     extent_;
        Data_t*  nodePoints_;
        Data_t*  distances_;                          // distances separating nodes
        Data_t*  values_;
        Data_t*  grads_;
        Data_t*  secondDerivs_;

        bool    naturalBoundaryConditions;
        Data_t  slope0_;
        Data_t  slopeN_;

        // internal methods:

        void captureGrid( int extent, const Data_t* nodes );
        void captureValues( int extent, const Data_t* values );
        void fillValues( int extent, double values_function(double x));
        void computeSecondDerivs();

};                                                    // BTSpline
#endif                                                // BTSPLINE1D_H
