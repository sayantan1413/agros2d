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

#ifndef {{ID}}_WEAKFORM_H
#define {{ID}}_WEAKFORM_H

#include "util.h"
#include <weakform/weakform.h>
#include "hermes2d/plugin_interface.h"
#include "hermes2d/marker.h"

{{#VOLUME_MATRIX_SOURCE}}
template<typename Scalar>
class {{FUNCTION_NAME}} : public MatrixFormVolAgros<Scalar>
{
public:
    {{FUNCTION_NAME}}(unsigned int i, unsigned int j,
                      Material *materialSource,
                      Material *materialTarget);

    virtual Scalar value(int n, double *wt, Hermes::Hermes2D::Func<Scalar> *u_ext[], Hermes::Hermes2D::Func<double> *u,
                         Hermes::Hermes2D::Func<double> *v, Hermes::Hermes2D::Geom<double> *e, Hermes::Hermes2D::ExtData<Scalar> *ext) const;
    virtual Hermes::Ord ord(int n, double *wt, Hermes::Hermes2D::Func<Hermes::Ord> *u_ext[], Hermes::Hermes2D::Func<Hermes::Ord> *u,
                            Hermes::Hermes2D::Func<Hermes::Ord> *v, Hermes::Hermes2D::Geom<Hermes::Ord> *e, Hermes::Hermes2D::ExtData<Hermes::Ord> *ext) const;   	    
    {{FUNCTION_NAME}}<Scalar>* clone();
private:
    Material *m_materialSource;
    Material *m_materialTarget;	

    {{#VARIABLE_SOURCE}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_SOURCE}}	
    {{#VARIABLE_TARGET}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_TARGET}}
};
{{/VOLUME_MATRIX_SOURCE}}

{{#VOLUME_VECTOR_SOURCE}}
template<typename Scalar>
class {{FUNCTION_NAME}} : public VectorFormVolAgros<Scalar>
{
public:
    {{FUNCTION_NAME}}(unsigned int i, unsigned int j,
                      Material *materialSource,
                      Material *materialTarget
				
    );

    virtual Scalar value(int n, double *wt, Hermes::Hermes2D::Func<Scalar> *u_ext[], Hermes::Hermes2D::Func<double> *v,
                         Hermes::Hermes2D::Geom<double> *e, Hermes::Hermes2D::ExtData<Scalar> *ext) const;
    virtual Hermes::Ord ord(int n, double *wt, Hermes::Hermes2D::Func<Hermes::Ord> *u_ext[], Hermes::Hermes2D::Func<Hermes::Ord> *v,
                            Hermes::Hermes2D::Geom<Hermes::Ord> *e, Hermes::Hermes2D::ExtData<Hermes::Ord> *ext) const;
    {{FUNCTION_NAME}}<Scalar>* clone();	

private:		
    Material *m_materialSource;
    Material *m_materialTarget;	
    {{#VARIABLE_SOURCE}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_SOURCE}}	
    {{#VARIABLE_TARGET}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_TARGET}}

    unsigned int j;
};
{{/VOLUME_VECTOR_SOURCE}}  

{{#SURFACE_MATRIX_SOURCE}}
template<typename Scalar>
class {{FUNCTION_NAME}} : public MatrixFormSurfAgros<Scalar>
{
public:
    {{FUNCTION_NAME}}(unsigned int i, unsigned int j,
                      SceneBoundary *boundary);

    virtual Scalar value(int n, double *wt, Hermes::Hermes2D::Func<Scalar> *u_ext[], Hermes::Hermes2D::Func<double> *u, Hermes::Hermes2D::Func<double> *v,
                         Hermes::Hermes2D::Geom<double> *e, Hermes::Hermes2D::ExtData<Scalar> *ext) const;
    virtual Hermes::Ord ord(int n, double *wt, Hermes::Hermes2D::Func<Hermes::Ord> *u_ext[], Hermes::Hermes2D::Func<Hermes::Ord> *u, Hermes::Hermes2D::Func<Hermes::Ord> *v,
                            Hermes::Hermes2D::Geom<Hermes::Ord> *e, Hermes::Hermes2D::ExtData<Hermes::Ord> *ext) const;
    {{FUNCTION_NAME}}<Scalar>* clone(); 

private:
    Boundary *m_boundarySource;
    {{#VARIABLE_SOURCE}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_SOURCE}}
    {{#VARIABLE_TARGET}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_TARGET}}
	
};
{{/SURFACE_MATRIX_SOURCE}}

{{#SURFACE_VECTOR_SOURCE}}
template<typename Scalar>
class {{FUNCTION_NAME}} : public VectorFormSurfAgros<Scalar>
{
public:
    {{FUNCTION_NAME}}(unsigned int i, unsigned int j,
                      SceneBoundary *boundary);

    virtual Scalar value(int n, double *wt, Hermes::Hermes2D::Func<Scalar> *u_ext[], Hermes::Hermes2D::Func<double> *v,
                         Hermes::Hermes2D::Geom<double> *e, Hermes::Hermes2D::ExtData<Scalar> *ext) const;
    virtual Hermes::Ord ord(int n, double *wt, Hermes::Hermes2D::Func<Hermes::Ord> *u_ext[], Hermes::Hermes2D::Func<Hermes::Ord> *v,
                            Hermes::Hermes2D::Geom<Hermes::Ord> *e, Hermes::Hermes2D::ExtData<Hermes::Ord> *ext) const;
    {{FUNCTION_NAME}}<Scalar>* clone();

private:
    Boundary *m_boundarySource;
    {{#VARIABLE_SOURCE}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_SOURCE}}	
    {{#VARIABLE_TARGET}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_TARGET}}

    unsigned int j;
};
{{/SURFACE_VECTOR_SOURCE}}

{{#EXACT_SOURCE}}
template<typename Scalar>
class {{FUNCTION_NAME}} : public ExactSolutionScalarAgros<Scalar>
{
public:
    {{FUNCTION_NAME}}(Hermes::Hermes2D::Mesh *mesh, SceneBoundary *boundary);

    Scalar value(double x, double y) const;
    void derivatives (double x, double y, Scalar& dx, Scalar& dy) const;

    Hermes::Ord ord (Hermes::Ord x, Hermes::Ord y) const
    {
        return Hermes::Ord(Hermes::Ord::get_max_order());
    }

    private:
    Boundary *m_boundarySource;  
    {{#VARIABLE_SOURCE}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_SOURCE}}		
    {{#VARIABLE_TARGET}}
    mutable Value {{VARIABLE_SHORT}};{{/VARIABLE_TARGET}}
};
{{/EXACT_SOURCE}}

#endif