// This file is part of Agros.
//
// Agros is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros.  If not, see <http://www.gnu.org/licenses/>.
//
//
// University of West Bohemia, Pilsen, Czech Republic
// Email: info@agros2d.org, home page: http://agros2d.org/

#ifndef BDF2_H
#define BDF2_H

#include "util.h"
#include "plugin_interface.h"

class Material;

// todo: zrychlit, odebrat asserty, uchovavat rovnou alpha/gamma, atd

class AGROS_LIBRARY_API BDF2Table
{
public:
    BDF2Table() : m_n(-100) {}

    // returns true if matrix unchanged
    bool setOrderAndPreviousSteps(int order, QList<double> previousStepsLengths);

    int n() const { return m_n;}
    int order() const { return m_n;}

    inline double matrixFormCoefficient() const {return m_alpha[0];}
    // double vectorFormCoefficient(Func<double> **ext, int component, int numComponents, int offsetPreviousTimeExt, int integrationPoint) const;

    static void test(bool varyLength = false);

protected:
    inline double* alpha() {return m_alpha; }

    double testCalcValue(double step, QList<double> values, double fVal);

    virtual void recalculate() = 0;

    int m_n;
    double th[10];
    double m_actualTimeStepLength;
    double m_alpha[10];
};

class BDF2ATable : public BDF2Table
{
    virtual void recalculate();
};

#endif // BDF2_H
