// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#include "util.h"
#include <cmath>
#include <iostream>

#include "firesafety.h"
#include "hermes2d.h"

using namespace std;

const double FireSafety::SIGMA = 5.67e-8;

FireProperty::FireProperty(double width, double height, double pv, FireCurve fireCurve, double i0, double eps, double increase) :
            m_width(width), m_height(height), m_pv(pv), m_fireCurve(fireCurve), m_i0(i0), m_eps(eps), m_increase(increase)
{
    m_fs = new FireSafety(this);
}

FireProperty::~FireProperty()
{
    delete m_fs;
}

double FireProperty::fireCurveValue()
{
    if (m_fireCurve == FireCurve_ISO)
        return (20 + 345 * log10(8 * m_pv + 1) + 273);
    else
        assert(0);
}


FireSafety::FireSafety(FireProperty fp) : m_fp(fp)
{    
}

double FireSafety::critical_intensity(double position, double d)
{
    // krivka pozaru
    double T = m_fp.fireCurve(); // Teplota dle krivky pozaru
    double I = m_fp.emisivity() * SIGMA * pow(T, 4); // Salava slozka

    double l = m_fp.width() / 2   +  position;    // pozice horizontalni
    double h = m_fp.height() / 2;     // pozice vertikalni ve 2D vzdy polovina vysky
    double X = h / d;
    double Y = l / d;


    // if (d == 0) return 0;
    double phi_1 = 1/(2 * M_PI) * (X / sqrt(1 + X * X) * atan( Y / sqrt(1 + X * X)) +
                                   ( Y /  sqrt(1 + Y * Y) * atan( X / sqrt(1 + Y * Y)))) * 2;

    l =  m_fp.width() / 2 -  position;    // pozice horizontalni

    X = h / d;
    Y = l / d;


    double phi_2 = 1/(2 * M_PI) * (X / sqrt(1 + X * X) * atan( Y / sqrt(1 + X * X)) +
                                   ( Y /  sqrt(1 + Y * Y) * atan( X / sqrt(1 + Y * Y)))) * 2;

    double I0 = I * (phi_2 + phi_1);
    return  (I0 - m_fp.i0());
}


double FireSafety::newton(double position, double estimate = 10)
{
    double x = 1e-6;
    double dx = 1e-5;
    double distance = estimate;
    bool found = false ;

    {
        double x = estimate;
        double dx = 1e-5;
        double distance = estimate;

        double I0 = 10;
        int j = 0;
        while ((abs(I0) > 1e-4) && (j < 1000))
        {
            j++;
            I0 = critical_intensity(position, x);
            double dI0 = (critical_intensity(position, x + dx) - I0) / dx;
            x = x - I0 / dI0;

        }

        if (j < 1000)
        {
            if (abs(distance - x) > 1e-10)
            {
                return x;
            }
        }
    }
    return -1;
}

QList<EnvelopePoint> FireSafety::calculateArea()
{
    int N = 100;
    EnvelopePoint point;

    double max_step = m_fp.width() / (N-1);
    double min_step = m_fp.width() / (N-1) / 1000;
    double step = m_fp.width() / (N-1);
    double estimate = m_fp.width() / 4;
    int i = 0;
    double position = 0;

    while (step > min_step)
    {
        i++;
        if (m_envelope.length() > 0)
            estimate = m_envelope.last().distance;

        point.distance = newton(position, estimate);

        if (point.distance != -1)
        {
            point.position = position;
            m_envelope.append(point);
            position += step;
        }
        else
        {
            position -= step;
            step = step / 2;
        }
    }


    step = - step * 2;

    while ((position - m_fp.width() / 2) > 2 * abs(step))
    {
        estimate = m_envelope.last().distance / 2;
        position += step;
        point.distance = newton(position, estimate);

        if (point.distance != -1)
        {
            point.position = position;
            m_envelope.append(point);
            if (abs(step)  <  m_fp.width() / (N-1))
            {
                step = step * 2;
            }
            else
            {
                step = - m_fp.width() / (N-1);
            }
            position += step;
        }
        else
        {
            position -= step;
            step = step / 2;
        }
    }

    // add end point
    point.position = m_fp.width() / 2;
    point.distance = 0;
    m_envelope.append(point);

    sortEnvelope();
    return m_envelope;
}

bool FireSafety::compare(const EnvelopePoint &s1, const EnvelopePoint &s2)
{
    return s1.distance < s2.distance;
}


void FireSafety::sortEnvelope()
{
    qSort(m_envelope.begin(), m_envelope.end(), FireSafety::compare);
}
