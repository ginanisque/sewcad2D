/**************************************************************************
 **
 **  @file   vtoolrectangle.cpp
 **  @author Seamly Codex
 **  @date   5 Jan, 2025
 **
 **  @brief  Implementation of the rectangle drafting helper tool.
 **
 **************************************************************************/

#include "vtoolrectangle.h"

#include "../../../../dialogs/tools/dialogrectangle.h"
#include "toollinepoint/vtoolendline.h"
#include "../../../../../vgeometry/vpointf.h"
#include "../../../../../vwidgets/vmaingraphicsscene.h"
#include "../../../../../vmisc/vabstractapplication.h"
#include "../../../../../vpatterndb/vcontainer.h"
#include "../../../../../vpatterndb/vtranslatevars.h"
#include "../../../../../ifc/xml/vabstractpattern.h"
#include "../../../../../ifc/exception/vexceptionbadid.h"

#include <QSharedPointer>
#include <new>

const QString VToolRectangle::ToolType = QStringLiteral("rectangle");

//---------------------------------------------------------------------------------------------------------------------
VToolRectangle::VToolRectangle(VAbstractPattern *doc, VContainer *data, const quint32 &id,
                               const QString &lineType, const QString &lineWeight, const QString &lineColor,
                               quint32 basePointId, const quint32 widthPointId, const quint32 heightPointId,
                               const Source &typeCreation, QGraphicsItem *parent)
    : PointIntersectXYTool(doc, data, id, lineType, lineWeight, lineColor, widthPointId, heightPointId,
                           typeCreation, parent)
    , m_basePointId(basePointId)
{
}

//---------------------------------------------------------------------------------------------------------------------
VToolRectangle *VToolRectangle::Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene *scene,
                                       VAbstractPattern *doc, VContainer *data)
{
    SCASSERT(not dialog.isNull())
    QSharedPointer<DialogRectangle> dialogTool = dialog.objectCast<DialogRectangle>();
    SCASSERT(not dialogTool.isNull())

    const quint32 basePointId = dialogTool->GetBasePointId();
    if (basePointId == NULL_ID)
    {
        return nullptr;
    }

    QString widthFormula = dialogTool->GetWidthFormula();
    QString lengthFormula = dialogTool->GetLengthFormula();
    QString horizontalAngle = QStringLiteral("0");
    QString verticalAngle = QStringLiteral("90");

    const QString widthPointName = dialogTool->getWidthPointName();
    const QString heightPointName = dialogTool->getLengthPointName();
    const QString cornerPointName = dialogTool->getCornerPointName();

    const QString lineType = dialogTool->getLineType();
    const QString lineWeight = dialogTool->getLineWeight();
    const QString lineColor = dialogTool->getLineColor();

    VToolEndLine *widthTool = VToolEndLine::Create(0, widthPointName, lineType, lineWeight, lineColor,
                                                   widthFormula, horizontalAngle, basePointId,
                                                   5, 10, true, scene, doc, data,
                                                   Document::FullParse, Source::FromGui);
    if (widthTool == nullptr)
    {
        return nullptr;
    }

    VToolEndLine *heightTool = VToolEndLine::Create(0, heightPointName, lineType, lineWeight, lineColor,
                                                    lengthFormula, verticalAngle, basePointId,
                                                    5, 10, true, scene, doc, data,
                                                    Document::FullParse, Source::FromGui);
    if (heightTool == nullptr)
    {
        return nullptr;
    }

    VToolRectangle *cornerTool = VToolRectangle::Create(0, cornerPointName, lineType, lineWeight, lineColor,
                                                        basePointId, widthTool->getId(), heightTool->getId(),
                                                        5, 10, true, scene, doc, data,
                                                        Document::FullParse, Source::FromGui);
    if (cornerTool != nullptr)
    {
        cornerTool->m_dialog = dialogTool;
    }

    return cornerTool;
}

//---------------------------------------------------------------------------------------------------------------------
VToolRectangle *VToolRectangle::Create(const quint32 _id, const QString &pointName, const QString &lineType,
                                       const QString &lineWeight, const QString &lineColor,
                                       quint32 basePointId, const quint32 widthPointId,
                                       const quint32 heightPointId, qreal mx, qreal my, bool showPointName,
                                       VMainGraphicsScene *scene, VAbstractPattern *doc, VContainer *data,
                                       const Document &parse, const Source &typeCreation)
{
    const QSharedPointer<VPointF> widthPoint = data->GeometricObject<VPointF>(widthPointId);
    const QSharedPointer<VPointF> heightPoint = data->GeometricObject<VPointF>(heightPointId);
    if (widthPoint.isNull() || heightPoint.isNull())
    {
        return nullptr;
    }

    QPointF point(widthPoint->x(), heightPoint->y());
    quint32 id = _id;
    VPointF *p = new VPointF(point, pointName, mx, my);
    p->setShowPointName(showPointName);

    if (typeCreation == Source::FromGui)
    {
        id = data->AddGObject(p);
        data->AddLine(widthPointId, id);
        data->AddLine(heightPointId, id);
    }
    else
    {
        data->UpdateGObject(id, p);
        data->AddLine(widthPointId, id);
        data->AddLine(heightPointId, id);
        if (parse != Document::FullParse)
        {
            doc->UpdateToolData(id, data);
        }
    }

    if (parse == Document::FullParse)
    {
        VDrawTool::AddRecord(id, Tool::Rectangle, doc);
        VToolRectangle *pointTool = new VToolRectangle(doc, data, id, lineType, lineWeight, lineColor,
                                                       basePointId, widthPointId, heightPointId, typeCreation);
        scene->addItem(pointTool);
        InitToolConnections(scene, pointTool);
        VAbstractPattern::AddTool(id, pointTool);
        doc->IncrementReferens(widthPoint->getIdTool());
        doc->IncrementReferens(heightPoint->getIdTool());
        return pointTool;
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolRectangle::basePointId() const
{
    if (VToolEndLine *width = widthLineTool())
    {
        return width->GetBasePointId();
    }
    return m_basePointId;
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolRectangle::basePointName() const
{
    const quint32 id = basePointId();
    if (id == NULL_ID)
    {
        return QString();
    }

    const auto point = VAbstractTool::data.GeometricObject<VPointF>(id);
    return point.isNull() ? QString() : point->name();
}

//---------------------------------------------------------------------------------------------------------------------
VToolEndLine *VToolRectangle::widthLineTool() const
{
    return lineToolForPoint(firstPointId);
}

//---------------------------------------------------------------------------------------------------------------------
VToolEndLine *VToolRectangle::heightLineTool() const
{
    return lineToolForPoint(secondPointId);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::setBasePointId(const quint32 &value)
{
    if (value == NULL_ID)
    {
        return;
    }

    if (VToolEndLine *width = widthLineTool())
    {
        width->SetBasePointId(value);
    }

    if (VToolEndLine *height = heightLineTool())
    {
        height->SetBasePointId(value);
    }

    m_basePointId = value;

    QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
    SaveOption(obj);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::setWidthFormula(const VFormula &formula)
{
    if (VToolEndLine *width = widthLineTool())
    {
        VFormula newFormula(formula);
        newFormula.setData(getData());
        newFormula.setToolId(width->getId());
        newFormula.setPostfix(UnitsToStr(qApp->patternUnit()));
        width->SetFormulaLength(newFormula);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::setHeightFormula(const VFormula &formula)
{
    if (VToolEndLine *height = heightLineTool())
    {
        VFormula newFormula(formula);
        newFormula.setData(getData());
        newFormula.setToolId(height->getId());
        newFormula.setPostfix(UnitsToStr(qApp->patternUnit()));
        height->SetFormulaLength(newFormula);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::setWidthPointName(const QString &name)
{
    if (name.isEmpty())
    {
        return;
    }

    if (VToolEndLine *width = widthLineTool())
    {
        if (width->name() != name)
        {
            width->setName(name);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::setHeightPointName(const QString &name)
{
    if (name.isEmpty())
    {
        return;
    }

    if (VToolEndLine *height = heightLineTool())
    {
        if (height->name() != name)
        {
            height->setName(name);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::setLineAttributes(const QString &lineTypeValue, const QString &lineColorValue,
                                       const QString &lineWeightValue)
{
    setLineType(lineTypeValue);
    setLineColor(lineColorValue);
    setLineWeight(lineWeightValue);

    if (VToolEndLine *width = widthLineTool())
    {
        width->setLineType(lineTypeValue);
        width->setLineColor(lineColorValue);
        width->setLineWeight(lineWeightValue);
    }

    if (VToolEndLine *height = heightLineTool())
    {
        height->setLineType(lineTypeValue);
        height->setLineColor(lineColorValue);
        height->setLineWeight(lineWeightValue);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::setDialog()
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogRectangle> dialogTool = m_dialog.objectCast<DialogRectangle>();
    SCASSERT(not dialogTool.isNull())

    const QSharedPointer<VPointF> cornerPoint = VAbstractTool::data.GeometricObject<VPointF>(m_id);
    dialogTool->setCornerPointName(cornerPoint->name());

    dialogTool->setLineType(m_lineType);
    dialogTool->setLineWeight(m_lineWeight);
    dialogTool->setLineColor(lineColor);

    dialogTool->setWidthPointName(firstPointName());
    dialogTool->setLengthPointName(secondPointName());
    dialogTool->SetBasePointId(basePointId());

    if (VToolEndLine *width = widthLineTool())
    {
        dialogTool->SetWidthFormula(width->GetFormulaLength().GetFormula(FormulaType::FromUser));
    }

    if (VToolEndLine *height = heightLineTool())
    {
        dialogTool->SetLengthFormula(height->GetFormulaLength().GetFormula(FormulaType::FromUser));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::SaveDialog(QDomElement &domElement)
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogRectangle> dialogTool = m_dialog.objectCast<DialogRectangle>();
    SCASSERT(not dialogTool.isNull())

    doc->SetAttribute(domElement, AttrName,        dialogTool->getCornerPointName());
    doc->SetAttribute(domElement, AttrLineType,    dialogTool->getLineType());
    doc->SetAttribute(domElement, AttrLineWeight,  dialogTool->getLineWeight());
    doc->SetAttribute(domElement, AttrLineColor,   dialogTool->getLineColor());
    doc->SetAttribute(domElement, AttrFirstPoint,  QString::number(firstPointId));
    doc->SetAttribute(domElement, AttrSecondPoint, QString::number(secondPointId));
    doc->SetAttribute(domElement, AttrBasePoint,   QString::number(dialogTool->GetBasePointId()));

    applyDialogChanges(dialogTool);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj)
{
    DoubleLinePointTool::SaveOptions(tag, obj);

    doc->SetAttribute(tag, AttrType,        ToolType);
    doc->SetAttribute(tag, AttrFirstPoint,  firstPointId);
    doc->SetAttribute(tag, AttrSecondPoint, secondPointId);
    doc->SetAttribute(tag, AttrBasePoint,   QString::number(basePointId()));
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::ReadToolAttributes(const QDomElement &domElement)
{
    PointIntersectXYTool::ReadToolAttributes(domElement);
    m_basePointId = doc->GetParametrUInt(domElement, AttrBasePoint, NULL_ID_STR);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolRectangle::applyDialogChanges(const QSharedPointer<DialogRectangle> &dialogTool)
{
    if (!dialogTool->getCornerPointName().isEmpty() && name() != dialogTool->getCornerPointName())
    {
        setName(dialogTool->getCornerPointName());
    }

    setLineAttributes(dialogTool->getLineType(), dialogTool->getLineColor(), dialogTool->getLineWeight());

    setWidthPointName(dialogTool->getWidthPointName());
    setHeightPointName(dialogTool->getLengthPointName());

    VFormula widthFormula(dialogTool->GetWidthFormula(), getData());
    widthFormula.setCheckZero(true);
    widthFormula.setPostfix(UnitsToStr(qApp->patternUnit()));
    setWidthFormula(widthFormula);

    VFormula heightFormula(dialogTool->GetLengthFormula(), getData());
    heightFormula.setCheckZero(true);
    heightFormula.setPostfix(UnitsToStr(qApp->patternUnit()));
    setHeightFormula(heightFormula);

    if (dialogTool->GetBasePointId() != NULL_ID)
    {
        setBasePointId(dialogTool->GetBasePointId());
    }
}

//---------------------------------------------------------------------------------------------------------------------
VToolEndLine *VToolRectangle::lineToolForPoint(quint32 pointId) const
{
    if (pointId == NULL_ID)
    {
        return nullptr;
    }

    const QSharedPointer<VPointF> point = VAbstractTool::data.GeometricObject<VPointF>(pointId);
    if (point.isNull())
    {
        return nullptr;
    }

    VDataTool *tool = nullptr;
    try
    {
        tool = VAbstractPattern::getTool(point->getIdTool());
    }
    catch (const VExceptionBadId &)
    {
        return nullptr;
    }

    return qobject_cast<VToolEndLine *>(tool);
}
