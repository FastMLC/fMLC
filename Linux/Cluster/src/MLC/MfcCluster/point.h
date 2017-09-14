#pragma once
#pragma once
class Point {
private:
	double xval, yval, zval;
public:
	// Constructor uses default arguments to allow calling with zero, one,
	// or two values.
	Point(double x = 0.0, double y = 0.0, double z = 0.0) {
		xval = x;
		yval = y;
		zval = z;
	}

	// Extractors.
	double x() { return xval; }
	double y() { return yval; }
	double z() { return zval; }
};