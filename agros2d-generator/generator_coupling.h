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

#ifndef GENERATOR_COUPLING_H
#define GENERATOR_COUPLING_H

#include "generator.h"
struct FormInfo;

class Agros2DGeneratorCoupling : public Agros2DGeneratorBase
{

public:
    Agros2DGeneratorCoupling(const QString &couplingId);

    // todo: copied from module
    void prepareWeakFormsOutput();
    void deleteWeakFormOutput();


    void generatePluginProjectFile();
    void generatePluginFilterFiles();
    void generatePluginLocalPointFiles();
    void generatePluginInterfaceFiles();
    void generatePluginWeakFormFiles();
    QMap<QString, QString>  sourceVaribales() const {return m_sourceVariables;}
    QMap<QString, QString>  targetVaribales() const {return m_targetVariables;}


private:

    void generateWeakForms(ctemplate::TemplateDictionary &output);

    std::auto_ptr<XMLModule::module> coupling_xsd;
    XMLModule::coupling *m_coupling;

    std::auto_ptr<XMLModule::module> m_source_module_xsd;
    XMLModule::field *m_sourceModule;

    std::auto_ptr<XMLModule::module> m_target_module_xsd;
    XMLModule::field *m_targetModule;

    // dictionary for variables used in weakforms
    QMap<QString, QString> m_sourceVariables;
    QMap<QString, QString> m_targetVariables;


    void generatePluginWeakFormSourceFiles();
    void generatePluginWeakFormHeaderFiles();

    template <typename WeakForm>
    void generateForm(FormInfo formInfo, LinearityType linearityType, ctemplate::TemplateDictionary &output, WeakForm weakform, QString weakFormType);

    QMap<QString, int> quantityOrdering;
    QMap<QString, bool> quantityIsNonlinear;
    QMap<QString, int> functionOrdering;

    QMap<QString, int> sourceQuantityOrdering;
    QMap<QString, bool> sourceQuantityIsNonlinear;
    QMap<QString, int> sourceFunctionOrdering;

    ctemplate::TemplateDictionary* m_output;

};

#endif // GENERATOR_COUPLING_H
