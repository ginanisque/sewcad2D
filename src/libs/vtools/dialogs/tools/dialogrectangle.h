/****************************************************************************
**  @file   dialogrectangle.h
**  @author Seamly Codex
**  @date   2025-01-05
**
**  @brief  Dialog for creating an axis aligned rectangle defined by width and
**          length formulas.
*****************************************************************************/

#ifndef DIALOGRECTANGLE_H
#define DIALOGRECTANGLE_H

#include <qcompilerdetection.h>
#include <QString>
#include <QtGlobal>

#include "dialogtool.h"

namespace Ui
{
class DialogRectangle;
}

class DialogRectangle : public DialogTool
{
    Q_OBJECT
public:
    explicit DialogRectangle(const VContainer *data, const quint32 &toolId, QWidget *parent = nullptr);
    ~DialogRectangle() override;

    quint32 GetBasePointId() const;
    void    SetBasePointId(const quint32 &value);

    QString GetWidthFormula() const;
    void    SetWidthFormula(const QString &value);

    QString GetLengthFormula() const;
    void    SetLengthFormula(const QString &value);

    QString getWidthPointName() const;
    void    setWidthPointName(const QString &value);

    QString getLengthPointName() const;
    void    setLengthPointName(const QString &value);

    QString getCornerPointName() const;
    void    setCornerPointName(const QString &value);

    QString getLineType() const;
    void    setLineType(const QString &value);

    QString getLineWeight() const;
    void    setLineWeight(const QString &value);

    QString getLineColor() const;
    void    setLineColor(const QString &value);

public slots:
    void ChosenObject(quint32 id, const SceneObject &type) override;
    void FormulaChanged();
    void WidthNameChanged();
    void LengthNameChanged();
    void CornerNameChanged();

    void FXWidth();
    void FXLength();

protected:
    void ShowDialog(bool click) override;
    void SaveData() override;
    void ShowVisualization() override;

private:
    Q_DISABLE_COPY(DialogRectangle)

    void fillCombos();
    void checkState();

    Ui::DialogRectangle *ui;

    QString m_widthFormula;
    QString m_lengthFormula;

    bool m_widthOk;
    bool m_lengthOk;
    bool m_widthNameOk;
    bool m_lengthNameOk;
    bool m_cornerNameOk;
    bool m_basePointOk;
};

#endif // DIALOGRECTANGLE_H
