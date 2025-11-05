/**************************************************************************
 **
 **  @file   vtoolrectangle.h
 **  @author Seamly Codex
 **  @date   5 Jan, 2025
 **
 **  @brief
 **  Rectangle construction tool built on top of Point-Intersect XY that
 **  manages the auxiliary width/height points and lines.
 **
 **************************************************************************/

#ifndef VTOOLRECTANGLE_H
#define VTOOLRECTANGLE_H

#include "point_intersectxy_tool.h"

#include <QSharedPointer>

class DialogRectangle;
class VToolEndLine;

class VFormula;

class VToolRectangle : public PointIntersectXYTool
{
    Q_OBJECT
public:
    static VToolRectangle *Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene *scene,
                                  VAbstractPattern *doc, VContainer *data);
    static VToolRectangle *Create(const quint32 _id, const QString &pointName, const QString &lineType,
                                  const QString &lineWeight, const QString &lineColor,
                                  quint32 basePointId, const quint32 widthPointId,
                                  const quint32 heightPointId, qreal mx, qreal my, bool showPointName,
                                  VMainGraphicsScene *scene, VAbstractPattern *doc, VContainer *data,
                                  const Document &parse, const Source &typeCreation);

    static const QString ToolType;

    enum {Type = UserType + static_cast<int>(Tool::Rectangle)};
    int type() const override { return Type; }

    quint32 basePointId() const;
    QString basePointName() const;

    void setBasePointId(const quint32 &value);
    void setWidthFormula(const VFormula &formula);
    void setHeightFormula(const VFormula &formula);
    void setWidthPointName(const QString &name);
    void setHeightPointName(const QString &name);
    void setLineAttributes(const QString &lineType, const QString &lineColor, const QString &lineWeight);

    VToolEndLine *widthLineTool() const;
    VToolEndLine *heightLineTool() const;

protected:
    void setDialog() override;
    void SaveDialog(QDomElement &domElement) override;
    void SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj) override;
    void ReadToolAttributes(const QDomElement &domElement) override;

private:
    Q_DISABLE_COPY(VToolRectangle)

    VToolRectangle(VAbstractPattern *doc, VContainer *data, const quint32 &id, const QString &lineType,
                   const QString &lineWeight, const QString &lineColor, quint32 basePointId,
                   const quint32 widthPointId, const quint32 heightPointId, const Source &typeCreation,
                   QGraphicsItem *parent = nullptr);

    void applyDialogChanges(const QSharedPointer<DialogRectangle> &dialogTool);
    VToolEndLine *lineToolForPoint(quint32 pointId) const;

    quint32 m_basePointId;
};

#endif // VTOOLRECTANGLE_H
