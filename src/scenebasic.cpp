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

#include "scenebasic.h"
#include "scene.h"

SceneBasic::SceneBasic()
{
    isSelected = false;
    isHighlighted = false;
}

QVariant SceneBasic::variant()
{
    QVariant v;
    v.setValue(this);
    return v;
}

// *************************************************************************************************************************************

SceneNode::SceneNode(const Point &point) : SceneBasic()
{
    this->point = point;
}
double SceneNode::distance(const Point &point)
{
    return (this->point - point).magnitude();
}

int SceneNode::showDialog(QWidget *parent, bool isNew)
{
    DSceneNode *dialog = new DSceneNode(this, parent, isNew);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneEdge::SceneEdge(SceneNode *nodeStart, SceneNode *nodeEnd, SceneEdgeMarker *marker, double angle) : SceneBasic()
{
    this->nodeStart = nodeStart;
    this->nodeEnd = nodeEnd;
    this->marker = marker;
    this->angle = angle;
}

Point SceneEdge::center()
{
    double distance = (nodeEnd->point - nodeStart->point).magnitude();
    Point t = (nodeEnd->point - nodeStart->point) / distance;
    double R = distance / (2.0*sin(angle/180.0*M_PI / 2.0));

    Point p = Point(distance/2.0, sqrt(sqr(R) - sqr(distance)/4.0));
    Point center = nodeStart->point + Point(p.x*t.x - p.y*t.y, p.x*t.y + p.y*t.x);

    return Point(center.x, center.y);
}

double SceneEdge::radius()
{
    return (center() - nodeStart->point).magnitude();
}

double SceneEdge::distance(const Point &point)
{
    if (angle == 0)
    {
        double t = ((point.x-nodeStart->point.x)*(nodeEnd->point.x-nodeStart->point.x) + (point.y-nodeStart->point.y)*(nodeEnd->point.y-nodeStart->point.y)) /
                   ((nodeEnd->point.x-nodeStart->point.x)*(nodeEnd->point.x-nodeStart->point.x) + (nodeEnd->point.y-nodeStart->point.y)*(nodeEnd->point.y-nodeStart->point.y));

        if (t > 1.0) t = 1.0;
        if (t < 0.0) t = 0.0;

        double x = nodeStart->point.x + t*(nodeEnd->point.x-nodeStart->point.x);
        double y = nodeStart->point.y + t*(nodeEnd->point.y-nodeStart->point.y);

        return sqrt(sqr(point.x-x) + sqr(point.y-y));
    }
    else
    {
        Point c = center();
        double R = radius();
        double distance = (point - c).magnitude();

        // point and radius are similar        
        if (distance < EPS_ZERO) return R;

        Point t = (point - c) / distance;
        double l = ((point - c) - t * R).magnitude();
        double z = (t.angle() - (nodeStart->point - c).angle())/M_PI*180.0;
        if (z < 0) z = z + 360.0; // interval (0, 360)
        if ((z > 0) && (z < angle)) return l;

        double a = (point - nodeStart->point).magnitude();
        double b = (point - nodeEnd->point).magnitude();

        return qMin(a, b);
    }
}

int SceneEdge::showDialog(QWidget *parent, bool isNew)
{
    DSceneEdge *dialog = new DSceneEdge(this, parent, isNew);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneLabel::SceneLabel(const Point &point, SceneLabelMarker *marker, double area) : SceneBasic()
{
    this->point = point;
    this->marker = marker;
    this->area = area;
}

double SceneLabel::distance(const Point &point)
{
    return (this->point - point).magnitude();
}

int SceneLabel::showDialog(QWidget *parent, bool isNew)
{
    DSceneLabel *dialog = new DSceneLabel(this, parent, isNew);
    return dialog->exec();
}

// *************************************************************************************************************************************

DSceneBasic::DSceneBasic(QWidget *parent, bool isNew) : QDialog(parent)
{
    this->isNew = isNew;
    layout = new QVBoxLayout();
}

DSceneBasic::~DSceneBasic()
{
    delete layout;
}

void DSceneBasic::createControls()
{
    // dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(doReject()));

    layout->addLayout(createContent());
    layout->addStretch();
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void DSceneBasic::doAccept()
{
    if (save())
        accept();
}

void DSceneBasic::doReject()
{
    reject();
}

// *************************************************************************************************************************************

DSceneNode::DSceneNode(SceneNode *node, QWidget *parent, bool isNew) : DSceneBasic(parent, isNew)
{
    m_object = node;

    setWindowIcon(icon("scene-node"));
    setWindowTitle(tr("Node"));

    createControls();

    load();

    resize(sizeHint());
    setMinimumSize(sizeHint());
    setMaximumSize(sizeHint());
}    

DSceneNode::~DSceneNode()
{
    delete txtPointX;
    delete txtPointY;
}

QLayout* DSceneNode::createContent()
{
    txtPointX = new SLineEditValue();
    txtPointY = new SLineEditValue();

    QFormLayout *layout = new QFormLayout();
    layout->addRow(Util::scene()->problemInfo()->labelX() + " (m):", txtPointX);
    layout->addRow(Util::scene()->problemInfo()->labelY() + " (m):", txtPointY);

    return layout;
}

bool DSceneNode::load()
{
    SceneNode *sceneNode = dynamic_cast<SceneNode *>(m_object);

    txtPointX->setNumber(sceneNode->point.x);
    txtPointY->setNumber(sceneNode->point.y);

    return true;
}

bool DSceneNode::save()
{
    if (!txtPointX->evaluate(false)) return false;
    if (!txtPointY->evaluate(false)) return false;

    SceneNode *sceneNode = dynamic_cast<SceneNode *>(m_object);

    Point point(txtPointX->number(), txtPointY->number());

    // check if node doesn't exists
    if (Util::scene()->getNode(point) && ((sceneNode->point != point) || isNew))
    {
        QMessageBox::warning(this, "Node", "Node already exists.");
        return false;
    }

    if (!isNew)
    {
        if (sceneNode->point != point)
        {
            Util::scene()->undoStack()->push(new SceneNodeCommandEdit(sceneNode->point, point));
        }
    }

    sceneNode->point = point;

    return true;
}

// *************************************************************************************************************************************

DSceneEdge::DSceneEdge(SceneEdge *edge, QWidget *parent, bool isNew) : DSceneBasic(parent, isNew)
{
    m_object = edge;

    setWindowIcon(icon("scene-edge"));
    setWindowTitle(tr("Edge"));

    createControls();

    load();

    resize(sizeHint());
    setMinimumSize(sizeHint());
    setMaximumSize(sizeHint());
}

DSceneEdge::~DSceneEdge()
{
    delete cmbNodeStart;
    delete cmbNodeEnd;
    delete cmbMarker;
    delete btnMarker;
    delete txtAngle;
}

QLayout* DSceneEdge::createContent()
{
    cmbNodeStart = new QComboBox();
    cmbNodeEnd = new QComboBox();
    cmbMarker = new QComboBox();
    connect(cmbMarker, SIGNAL(currentIndexChanged(int)), this, SLOT(doMarkerChanged(int)));
    btnMarker = new QPushButton("...");
    btnMarker->setMaximumWidth(25);
    connect(btnMarker, SIGNAL(clicked()), this, SLOT(doMarkerClicked()));
    txtAngle = new SLineEditValue();

    QHBoxLayout *layoutMarker = new QHBoxLayout();
    layoutMarker->addWidget(cmbMarker);
    layoutMarker->addWidget(btnMarker);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("Start point:"), cmbNodeStart);
    layout->addRow(tr("End point:"), cmbNodeEnd);
    layout->addRow(tr("Boundary condition:"), layoutMarker);
    layout->addRow(tr("Angle (deg.):"), txtAngle);

    fillComboBox();

    return layout;
}

void DSceneEdge::fillComboBox()
{
    // start and end nodes
    cmbNodeStart->clear();
    cmbNodeEnd->clear();
    for (int i = 0; i<Util::scene()->nodes.count(); i++)
    {
        cmbNodeStart->addItem(QString("[%1; %2]").arg(Util::scene()->nodes[i]->point.x, 0, 'e', 2).arg(Util::scene()->nodes[i]->point.y, 0, 'e', 2),
                              Util::scene()->nodes[i]->variant());
        cmbNodeEnd->addItem(QString("[%1; %2]").arg(Util::scene()->nodes[i]->point.x, 0, 'e', 2).arg(Util::scene()->nodes[i]->point.y, 0, 'e', 2),
                            Util::scene()->nodes[i]->variant());
    }

    // markers
    cmbMarker->clear();
    for (int i = 0; i<Util::scene()->edgeMarkers.count(); i++)
    {
        cmbMarker->addItem(Util::scene()->edgeMarkers[i]->name, Util::scene()->edgeMarkers[i]->variant());
    }
}

bool DSceneEdge::load()
{
    SceneEdge *sceneEdge = dynamic_cast<SceneEdge *>(m_object);

    cmbNodeStart->setCurrentIndex(cmbNodeStart->findData(sceneEdge->nodeStart->variant()));
    cmbNodeEnd->setCurrentIndex(cmbNodeEnd->findData(sceneEdge->nodeEnd->variant()));
    cmbMarker->setCurrentIndex(cmbMarker->findData(sceneEdge->marker->variant()));
    txtAngle->setNumber(sceneEdge->angle);

    return true;
}

bool DSceneEdge::save()
{
    if (!txtAngle->evaluate(false)) return false;

    SceneEdge *sceneEdge = dynamic_cast<SceneEdge *>(m_object);

    SceneNode *nodeStart = dynamic_cast<SceneNode *>(cmbNodeStart->itemData(cmbNodeStart->currentIndex()).value<SceneBasic *>());
    SceneNode *nodeEnd = dynamic_cast<SceneNode *>(cmbNodeEnd->itemData(cmbNodeEnd->currentIndex()).value<SceneBasic *>());

    // check if edge doesn't exists
    SceneEdge *edgeCheck = Util::scene()->getEdge(nodeStart->point, nodeEnd->point, txtAngle->number());
    if ((edgeCheck) && ((sceneEdge != edgeCheck) || isNew))
    {
        QMessageBox::warning(this, "Edge", "Edge already exists.");
        return false;
    }

    if (nodeStart == nodeEnd)
    {
        QMessageBox::warning(this, "Edge", "Start and end node are same.");
        return false;
    }

    if (!isNew)
    {
        if ((sceneEdge->nodeStart != nodeStart) || (sceneEdge->nodeEnd != nodeEnd) || (sceneEdge->angle != txtAngle->number()))
        {
            Util::scene()->undoStack()->push(new SceneEdgeCommandEdit(sceneEdge->nodeStart->point, sceneEdge->nodeEnd->point, nodeStart->point, nodeEnd->point, sceneEdge->angle, txtAngle->number()));
        }
    }

    sceneEdge->nodeStart = nodeStart;
    sceneEdge->nodeEnd = nodeEnd;
    sceneEdge->marker = cmbMarker->itemData(cmbMarker->currentIndex()).value<SceneEdgeMarker *>();
    sceneEdge->angle = txtAngle->number();

    return true;
}

void DSceneEdge::doMarkerChanged(int index)
{
    btnMarker->setEnabled(cmbMarker->currentIndex() > 0);
}

void DSceneEdge::doMarkerClicked()
{
    SceneEdgeMarker *marker = cmbMarker->itemData(cmbMarker->currentIndex()).value<SceneEdgeMarker *>();
    if (marker->showDialog(this) == QDialog::Accepted)
    {
        cmbMarker->setItemText(cmbMarker->currentIndex(), marker->name);
        Util::scene()->refresh();
    }
}

// *************************************************************************************************************************************

DSceneLabel::DSceneLabel(SceneLabel *label, QWidget *parent, bool isNew) : DSceneBasic(parent, isNew)
{
    m_object = label;

    setWindowIcon(icon("scene-label"));
    setWindowTitle(tr("Label"));

    createControls();

    load();

    resize(sizeHint());
    setMinimumSize(sizeHint());
    setMaximumSize(sizeHint());
}

DSceneLabel::~DSceneLabel()
{
    delete txtPointX;
    delete txtPointY;
    delete cmbMarker;
    delete btnMarker;
    delete txtArea;
}

QLayout* DSceneLabel::createContent()
{
    txtPointX = new SLineEditValue();
    txtPointY = new SLineEditValue();
    cmbMarker = new QComboBox();
    connect(cmbMarker, SIGNAL(currentIndexChanged(int)), this, SLOT(doMarkerChanged(int)));
    btnMarker = new QPushButton("...");
    btnMarker->setMaximumWidth(25);
    connect(btnMarker, SIGNAL(clicked()), this, SLOT(doMarkerClicked()));
    txtArea = new SLineEditValue();

    QHBoxLayout *layoutMarker = new QHBoxLayout();
    layoutMarker->addWidget(cmbMarker);
    layoutMarker->addWidget(btnMarker);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(Util::scene()->problemInfo()->labelX() + " (m):", txtPointX);
    layout->addRow(Util::scene()->problemInfo()->labelY() + " (m):", txtPointY);
    layout->addRow(tr("Material:"), layoutMarker);
    layout->addRow(tr("Triangle area (m):"), txtArea);

    fillComboBox();

    return layout;
}

void DSceneLabel::fillComboBox()
{
    // markers
    cmbMarker->clear();
    for (int i = 0; i<Util::scene()->labelMarkers.count(); i++)
    {
        cmbMarker->addItem(Util::scene()->labelMarkers[i]->name, Util::scene()->labelMarkers[i]->variant());
    }
}

bool DSceneLabel::load()
{
    SceneLabel *sceneLabel = dynamic_cast<SceneLabel *>(m_object);

    txtPointX->setNumber(sceneLabel->point.x);
    txtPointY->setNumber(sceneLabel->point.y);
    cmbMarker->setCurrentIndex(cmbMarker->findData(sceneLabel->marker->variant()));
    txtArea->setNumber(sceneLabel->area);

    return true;
}

bool DSceneLabel::save()
{
    if (!txtPointX->evaluate(false)) return false;
    if (!txtPointY->evaluate(false)) return false;
    if (!txtArea->evaluate(false)) return false;

    SceneLabel *sceneLabel = dynamic_cast<SceneLabel *>(m_object);

    Point point(txtPointX->number(), txtPointY->number());

    // check if label doesn't exists
    if (Util::scene()->getLabel(point) && ((sceneLabel->point != point) || isNew))
    {
        QMessageBox::warning(this, "Label", "Label already exists.");
        return false;
    }

    if (!isNew)
    {
        if (sceneLabel->point != point)
        {
            Util::scene()->undoStack()->push(new SceneLabelCommandEdit(sceneLabel->point, point));
        }
    }

    sceneLabel->point = point;
    sceneLabel->marker = cmbMarker->itemData(cmbMarker->currentIndex()).value<SceneLabelMarker *>();
    sceneLabel->area = txtArea->number();

    return true;
}

void DSceneLabel::doMarkerChanged(int index)
{
    btnMarker->setEnabled(cmbMarker->currentIndex() > 0);
}

void DSceneLabel::doMarkerClicked()
{
    SceneLabelMarker *marker = cmbMarker->itemData(cmbMarker->currentIndex()).value<SceneLabelMarker *>();
    if (marker->showDialog(this) == QDialog::Accepted)
    {
        cmbMarker->setItemText(cmbMarker->currentIndex(), marker->name);
        Util::scene()->refresh();
    }
}

// undo framework *******************************************************************************************************************

// Node

SceneNodeCommandAdd::SceneNodeCommandAdd(const Point &point, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
}

void SceneNodeCommandAdd::undo()
{
    SceneNode *node = Util::scene()->getNode(m_point);
    if (node)
    {
        Util::scene()->removeNode(node);
    }
}

void SceneNodeCommandAdd::redo()
{
    Util::scene()->addNode(new SceneNode(m_point));
}

SceneNodeCommandRemove::SceneNodeCommandRemove(const Point &point, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
}

void SceneNodeCommandRemove::undo()
{
    Util::scene()->addNode(new SceneNode(m_point));
}

void SceneNodeCommandRemove::redo()
{
    SceneNode *node = Util::scene()->getNode(m_point);
    if (node)
    {
        Util::scene()->removeNode(node);
    }
}

SceneNodeCommandEdit::SceneNodeCommandEdit(const Point &point, const Point &pointNew, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
    m_pointNew = pointNew;
}

void SceneNodeCommandEdit::undo()
{
    SceneNode *node = Util::scene()->getNode(m_pointNew);
    if (node)
    {
        node->point = m_point;
        Util::scene()->refresh();
    }
}

void SceneNodeCommandEdit::redo()
{
    SceneNode *node = Util::scene()->getNode(m_point);
    if (node)
    {
        node->point = m_pointNew;
        Util::scene()->refresh();
    }
}

// Label

SceneLabelCommandAdd::SceneLabelCommandAdd(const Point &point, const QString &markerName, double area, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
    m_markerName = markerName;
    m_area = area;
}

void SceneLabelCommandAdd::undo()
{
    Util::scene()->removeLabel(Util::scene()->getLabel(m_point));
}

void SceneLabelCommandAdd::redo()
{
    SceneLabelMarker *labelMarker = Util::scene()->getLabelMarker(m_markerName);
    if (labelMarker == NULL) labelMarker = Util::scene()->labelMarkers[0];
    Util::scene()->addLabel(new SceneLabel(m_point, labelMarker, m_area));
}

SceneLabelCommandRemove::SceneLabelCommandRemove(const Point &point, const QString &markerName, double area, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
    m_markerName = markerName;
    m_area = area;
}

void SceneLabelCommandRemove::undo()
{
    SceneLabelMarker *labelMarker = Util::scene()->getLabelMarker(m_markerName);
    if (labelMarker == NULL) labelMarker = Util::scene()->labelMarkers[0];
    Util::scene()->addLabel(new SceneLabel(m_point, labelMarker, m_area));
}

void SceneLabelCommandRemove::redo()
{
    Util::scene()->removeLabel(Util::scene()->getLabel(m_point));
}

SceneLabelCommandEdit::SceneLabelCommandEdit(const Point &point, const Point &pointNew, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
    m_pointNew = pointNew;
}

void SceneLabelCommandEdit::undo()
{
    SceneLabel *label = Util::scene()->getLabel(m_pointNew);
    if (label)
    {
        label->point = m_point;
        Util::scene()->refresh();
    }
}

void SceneLabelCommandEdit::redo()
{
    SceneLabel *label = Util::scene()->getLabel(m_point);
    if (label)
    {
        label->point = m_pointNew;
        Util::scene()->refresh();
    }
}

// Edge

SceneEdgeCommandAdd::SceneEdgeCommandAdd(const Point &pointStart, const Point &pointEnd, const QString &markerName, double angle, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_pointStart = pointStart;
    m_pointEnd = pointEnd;
    m_markerName = markerName;
    m_angle = angle;
}

void SceneEdgeCommandAdd::undo()
{
    Util::scene()->removeEdge(Util::scene()->getEdge(m_pointStart, m_pointEnd, m_angle));
}

void SceneEdgeCommandAdd::redo()
{
    SceneEdgeMarker *edgeMarker = Util::scene()->getEdgeMarker(m_markerName);
    if (edgeMarker == NULL) edgeMarker = Util::scene()->edgeMarkers[0];
    Util::scene()->addEdge(new SceneEdge(Util::scene()->addNode(new SceneNode(m_pointStart)), Util::scene()->addNode(new SceneNode(m_pointEnd)), edgeMarker, m_angle));
}

SceneEdgeCommandRemove::SceneEdgeCommandRemove(const Point &pointStart, const Point &pointEnd, const QString &markerName, double angle, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_pointStart = pointStart;
    m_pointEnd = pointEnd;
    m_markerName = markerName;
    m_angle = angle;
}

void SceneEdgeCommandRemove::undo()
{
    SceneEdgeMarker *edgeMarker = Util::scene()->getEdgeMarker(m_markerName);
    if (edgeMarker == NULL) edgeMarker = Util::scene()->edgeMarkers[0];
    Util::scene()->addEdge(new SceneEdge(Util::scene()->addNode(new SceneNode(m_pointStart)), Util::scene()->addNode(new SceneNode(m_pointEnd)), edgeMarker, m_angle));
}

void SceneEdgeCommandRemove::redo()
{
    Util::scene()->removeEdge(Util::scene()->getEdge(m_pointStart, m_pointEnd, m_angle));
}

SceneEdgeCommandEdit::SceneEdgeCommandEdit(const Point &pointStart, const Point &pointEnd, const Point &pointStartNew, const Point &pointEndNew, double angle, double angleNew, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_pointStart = pointStart;
    m_pointEnd = pointEnd;
    m_pointStartNew = pointStartNew;
    m_pointEndNew = pointEndNew;
    m_angle = angle;
    m_angleNew = angleNew;
}

void SceneEdgeCommandEdit::undo()
{
    SceneEdge *edge = Util::scene()->getEdge(m_pointStartNew, m_pointEndNew, m_angleNew);
    if (edge)
    {
        edge->nodeStart = Util::scene()->getNode(m_pointStart);
        edge->nodeEnd = Util::scene()->getNode(m_pointEnd);
        edge->angle = m_angle;
        Util::scene()->refresh();
    }
}

void SceneEdgeCommandEdit::redo()
{
    SceneEdge *edge = Util::scene()->getEdge(m_pointStart, m_pointEnd, m_angle);
    if (edge)
    {
        edge->nodeStart = Util::scene()->getNode(m_pointStartNew);
        edge->nodeEnd = Util::scene()->getNode(m_pointEndNew);
        edge->angle = m_angleNew;
        Util::scene()->refresh();
    }
}
