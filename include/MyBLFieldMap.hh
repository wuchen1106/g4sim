// AK20110616 Modified from G4beamline v2.03 BLFieldMap.hh
//
/*
This source file is part of G4beamline, http://g4beamline.muonsinc.com
Copyright (C) 2003,2004,2005,2006 by Tom Roberts, all rights reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

http://www.gnu.org/copyleft/gpl.html
*/
#ifndef MYBLFIELDMAP_HH
#define MYBLFIELDMAP_HH

#include "myglobals.hh"

/**	class MyBLFieldMap implements a general field map, both B and E.
 *
 *	Initially this class simply reads an input file to define the map.
 *	Eventually it will also generate a map automatically from a BLCoil,
 *	and from an arbitrary collection of magnets (to improve tracking
 *	efficiency for a large number of overlapping solenoids).
 *
 *	Field components are interpolated in the map; the values used are:
 *	  Bx = (interpolated value)*normB*(element_current/current_param)
 *	  Ex = (interpolated value)*normE*(element_gradient/gradient_param)
 *	  [other components are similar]
 *	where element_current and element_gradient are from the element
 *	definition command in the input file, and normB, normE, curren_param,
 *	and gradient_param come from the input file. The presence of both norm
 *	and current/gradient in the input file is to accommodate diverse
 *	sources of input files and units.
 *
 *	Outside the map the fields are zero, so if the map is truncated you
 *	should ensure that no particles are tracked outside the map in that
 *	region. This happens, for instance, for a beam solenoid truncated
 *	in radius to the inside of the coil -- particles which enter the
 *	coil will see a zero field, so the coil should be set to kill them;
 *	particles outside the map along z but inside the bore will also see
 *	a zero field, but that is usually OK as long as the map extends far
 *	enough along Z to include the nonzero field region.
 *
 *
 *	Input File format:
 *	Blank lines, and lines beginning with # or * are comments. Lines
 *	beginning with * are printed to stdout. Units are mm for coordinates,
 *	Tesla for B, and MegaVolts/meter for E; use normB and normE if the
 *	data points use different units.
 *
 *	The input file starts with a set of commands to define the parameters
 *	of the map, followed by blocks of lines containing the values of the
 *	field components. The field component names depend on the type of map
 *	(grid: Bx,By,Bz,Ex,Ey,Ez; cylinder: Br,Bz,Er,Ez).
 *	Each command has a specific list of arguments to define parameters
 *	of the map.
 *
 *	BEWARE: the parsing is not exhaustive. For instance, invalid arguments
 *	are silently ignored (which means you must verify the spelling and
 *	capitalization of argument names). Correct inputs will yield correct
 *	results, but invalid inputs may not be detected and may yield
 *	seemingly-correct but unintended results.
 *
 *	The first command is usually a param command, which has the following
 *	arguments:
 *		maxline	The maximum number of characters per line (default=1023)
 *		current	The current corresponding to this map (default=1.0)
 *		gradient The gradient corresponding to the map (default=1.0)
 *		normE	A normalization factor for E components (default=1.0)
 *		normB	A normalization factor for B components (default=1.0)
 *
 *	Two types of maps are implemented: grid and cylinder.
 *
 *	grid maps are a 3-D grid, with each block of data being a single
 *	X-Y plane; within a block the lines are Y and the columns of each line
 *	are values along X.
 *	The grid command has the following arguments:
 *		X0	The X value for the first value in each line
 *		Y0	The Y value for the first line in each block
 *		Z0	The Z value for the first block of each field component
 *		nX	The number of columns per line
 *		nY	The number of lines per block
 *		nZ	The number of blocks per field component
 *		dX	The X increment between values in each line
 *		dY	The Y increment between lines
 *		dZ	The Z increment between blocks
 *	     tolerance	The tolerance for pointwise data (default=0.01 mm)
 *	After the grid command, the following optional commands can be given:
 *		extendX flip=...
 *		extendY flip=...
 *		extendZ flip=...
 *	These commands permit a half-map to be extended to the full map
 *	around X=0, Y=0, or Z=0 respectively. The optional flip argument is a
 *	comma-separated list of field components whose signs will be inverted
 *	for negative values of the coordinate. For example, "extendZ flip=Bx,Ex"
 *	means the map from Z=0 to Z=(nZ-1)*dZ is extended symmetrically around
 *	Z=0 to negative Z values, flipping the signs of Bx and Ex when Z<0.
 *	This could be followed by "extendX flip=Bx,Ex", and the field flips
 *	will be the products of both commands.
 *
 *	cylinder maps are a 2-D map with rotational symmetry around the Z axis.
 *	Each field component has a single block with lines being Z and the
 *	columns being R.
 *	The cylinder command has the following arguments:
 *		R0	The R value for the first line in each block
 *		Z0	The Z value for the first line in each block
 *		nR	The number of columns per line
 *		nZ	The number of lines per block
 *		dR	The R increment between colums
 *		dZ	The Z increment between lines
 *	     tolerance	The tolerance for pointwise data (default=0.01 mm)
 *	After the cylinder command, the following optional commands can be
 *	given:
 *		extendZ flip=...
 *	This command behaves the same as for the grid map.
 *
 *	After the commands, each block consists of a line containing the
 *	name of the field component, followed by the lines of the block.
 *	The values within a line can be separated by whitespace or a ','
 *	followed by optional whitespace.
 *	Field components that are not given are set to 0.0 everywhere.
 *	Missing values will be considered to be 0.0.
 *	For grid maps the first block is for Z=Z0, and successive blocks
 *	increment Z by dZ; the first line in a block is for Y=Y0 and the
 *	first column in each line is for X=X0.
 *	For cylinder maps, the first line in each block is for Z=Z0 and the
 *	first column in each line is for R=0.
 *
 *	Instead of the blocked input format, a pointwise data format can be
 *	used. This is introduced by a line containing the command "data",
 *	followed by the individual points of the map, one per line.
 *	for a grid field, each line contains values for
 *	X,Y,Z,Bx,By,Bz,Ex,Ey,Ez separated by either a comma and optional
 *	whitespece or by whitespace. for a cylinder field each line contains
 *	values for R,Z,Br,Bz,Er,Ez. The order of the points does not matter;
 *	omitted grid points will be 0.0, and for duplicates the last entry
 *	wins. If there is no E field, the Ex,Ey,Ez or Er,Ez entries should
 *	be omitted on every line. NOTE: every line's X,Y,Z or R,Z must be
 *	on a grid point as specified by the arguments to the grid or cylinder
 *	commands, to within the tolerance specified; if not, an error message
 *	is printed and the input line is ignored.
 *
 *	For time=dependent fields, the "time" command is used:
 *		time [period=12]
 *	period, if given, is in nanoseconds, and causes the interval [0,period)
 *	to be extended forever (before and after the values given). Because of
 *	the interpolation used, at least two points beyond the interval
 *	boundaries should be provided; there need not be a point at either
 *	boundary (but usually there are).
 *	Following the time command are lines containing 2 or 3 doubles:
 *		t B E
 *	where t is the time (nanoseconds), and B and E are factors for the
 *	fields. If E is omitted, the value for B is used. These values will
 *	be interpolated in time with a cubic spline that can handle either
 *	uniform or non-uniform spacing of points along t.
 *	The time command can come either before or after the cylinder or grid
 *	commands, but not within either of their sequences.
 *	Note a cubic spline is used to interpolate between points, and that
 *	can cause over/under-shoot near an abrupt change. Combined with period=
 *	this gives an excellent representation of sinewave/cosinewave.
 *
 *	Note that time dependence can currently only be specified via the
 *	time command in an input file (i.e. not programmable method exists).
 *
 *
 *	Example block input file:
 *	* this is an example MyBLFieldMap input file, suitable for a solenoid
 *	# grid interval is 1 cm.
 *	# The region of validity is -390<=Z<=390 and 0<=R<=90
 *	param normB=1.0 current=1.0
 *	cylinder R0=0.0 Z0=0.0 nR=10 nZ=40 dR=10.0 dZ=10.0
 *	extendZ flip=Br
 *	Bz
 *	... 40 lines of 10 values, Z=0 thru Z=390
 *	Br
 *	... 40 lines of 10 values, Z=0 thru Z=390
 *	--EOF--
 *
 *	Example pointwise input file:
 *	* this is an example MyBLFieldMap input file, suitable for a solenoid
 *	# grid interval is 1 cm.
 *	# The region of validity is -390<=Z<=390 and 0<=R<=90
 *	param normB=1.0 current=1.0
 *	cylinder R0=0.0 Z0=0.0 nR=10 nZ=40 dR=10.0 dZ=10.0
 *	extendZ flip=Br
 *	data
 *	... 400 lines of 4 values, giving R,Z,Br,Bz
 *	--EOF--
 *
 **/
class MyBLFieldMap {
    public:
        /// default constructor.
        MyBLFieldMap();

        /// destructor.
        virtual ~MyBLFieldMap();

        /// readFile() reads a file to initialize the map.
        /// Returns true if OK, false on error.
        bool readFile(G4String filename);

        /// writeFile() writes the map to a file.
        /// Returns true if OK, false on error.
        bool writeFile(G4String filename, G4String comment="");

        /// getFieldValue() gets the map's field to the field[] array.
        /// local[] must be LOCAL coordinates; local[3] (time) is ignored
        /// unless there is a time dependence specified.
        /// (elements using MyBLFieldMap perform the global->local conversion)
        void getFieldValue(const G4double local[4], G4double field[6],
            G4double current=1.0, G4double gradient=1.0);

        /// getBoundingPoint() returns the i-th bounding point of the map.
        void getBoundingPoint(int i, G4double point[4]);

        /// hasB() returns true if this map has a nonzero B field.
        bool hasB();

        /// hasE() returns true if this map has a nonzero E field.
        bool hasE();

        /// createGridMap() will create a 3-d map from a pre-defined field.
        /// returns false if error.
        bool createGridMap(G4double X0, G4double Y0, G4double Z0, G4double dX,
            G4double dY, G4double dZ, int nX, int nY, int nZ,
            class G4ElectroMagneticField *field);

        /// createCylinderMap() will create a 2-d map from a pre-defined
        /// cylindrically-symmetric field. NOTE: cylindrical symmetry is
        /// ASSUMED; the field in the Y=0,X>=0 half-plane is used (Bx->Br...).
        /// returns false if error.
        bool createCylinderMap(G4double R0, G4double Z0, G4double dR,  G4double dZ, int nR,
            int nZ, class G4ElectroMagneticField *field);

        /// createTimeDependence() will apply the time dependence given.
        /// n is the # elements in the arrays; t[] is the time value for
        /// each point, b[] is the factor for B, and e[] is the factor for E.
        /// returns false if error.
        bool createTimeDependence(int n, G4double t[], G4double b[],
            G4double e[]=0, G4double period=-1.0);

        /// getTimeFactor() returns the time factors for B and E at time t.
        /// returns false if error.
        bool getTimeFactor(G4double t, G4double *b, G4double *e);
    private:
        G4int maxline;
        G4double current;
        G4double gradient;
        G4double normB;
        G4double normE;
        class FieldMapImpl *impl;
        class TimeImpl *time;
        friend class FieldMapPlacement;
};
#endif                                                // MYBLFIELDMAP_HH
