/****************************************************************************
**  @file   dialogrectangle.cpp
*****************************************************************************/

#include "dialogrectangle.h"
#include "ui_dialogrectangle.h"

#include <QLineEdit>
#include <QToolButton>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <new>

#include "../vpatterndb/vcontainer.h"
#include "../vpatterndb/vtranslatevars.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/vcommonsettings.h"
#include "../ifc/xml/vabstractpattern.h"
#include "../support/edit_formula_dialog.h"

DialogRectangle::DialogRectangle(const VContainer *data, const quint32 &toolId, QWidget *parent)
    : DialogTool(data, toolId, parent)
    , ui(new Ui::DialogRectangle)
    , m_widthFormula()
    , m_lengthFormula()
    , m_widthOk(false)
    , m_lengthOk(false)
    , m_widthNameOk(false)
    , m_lengthNameOk(false)
    , m_cornerNameOk(false)
    , m_basePointOk(false)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(QIcon(":/toolicon/32x32/rectangle.png"));

    setDialogPosition();
    initializeOkCancelApply(ui);

    ui->lineEditWidthPoint->setClearButtonEnabled(true);
    ui->lineEditHeightPoint->setClearButtonEnabled(true);
    ui->lineEditCornerPoint->setClearButtonEnabled(true);

    const auto currentDoc = qApp->getCurrentDocument();
    if (currentDoc != nullptr)
    {
        const QString widthName = currentDoc->GenerateLabel(LabelType::NewLabel);
        ui->lineEditWidthPoint->setText(widthName);
        const QString heightName = currentDoc->GenerateLabel(LabelType::NewLabel, widthName);
        ui->lineEditHeightPoint->setText(heightName);
        const QString cornerName = currentDoc->GenerateLabel(LabelType::NewLabel, heightName);
        ui->lineEditCornerPoint->setText(cornerName);
    }

    fillCombos();

    connect(ui->comboBoxBasePoint, &QComboBox::currentIndexChanged, this, &DialogRectangle::FormulaChanged);
    connect(ui->lineEditWidthFormula, &QLineEdit::textChanged, this, &DialogRectangle::FormulaChanged);
    connect(ui->lineEditLengthFormula, &QLineEdit::textChanged, this, &DialogRectangle::FormulaChanged);
    connect(ui->lineEditWidthPoint, &QLineEdit::textChanged, this, &DialogRectangle::WidthNameChanged);
    connect(ui->lineEditHeightPoint, &QLineEdit::textChanged, this, &DialogRectangle::LengthNameChanged);
    connect(ui->lineEditCornerPoint, &QLineEdit::textChanged, this, &DialogRectangle::CornerNameChanged);

    connect(ui->toolButtonFXWidth, &QToolButton::clicked, this, &DialogRectangle::FXWidth);
    connect(ui->toolButtonFXLength, &QToolButton::clicked, this, &DialogRectangle::FXLength);

    FormulaChanged();
    WidthNameChanged();
    LengthNameChanged();
    CornerNameChanged();
}

DialogRectangle::~DialogRectangle()
{
    delete ui;
}

void DialogRectangle::fillCombos()
{
    fillComboBoxPoints(ui->comboBoxBasePoint, FillComboBox::NoChildren);
    m_basePointOk = ui->comboBoxBasePoint->currentIndex() >= 0;

    const auto currentDoc = qApp->getCurrentDocument();
    if (currentDoc != nullptr)
    {
        auto lineColor = currentDoc->getDefaultLineColor();
        auto lineWeight = currentDoc->getDefaultLineWeight();
        auto lineType = currentDoc->getDefaultLineType();

        int index = ui->comboBoxLineColor->findData(lineColor);
        if (index != -1)
        {
            ui->comboBoxLineColor->setCurrentIndex(index);
        }

        index = ui->comboBoxLineWeight->findData(lineWeight);
        if (index != -1)
        {
            ui->comboBoxLineWeight->setCurrentIndex(index);
        }

        index = ui->comboBoxLineType->findData(lineType);
        if (index != -1)
        {
            ui->comboBoxLineType->setCurrentIndex(index);
        }
    }
}

void DialogRectangle::FormulaChanged()
{
    m_widthFormula = ui->lineEditWidthFormula->text();
    m_lengthFormula = ui->lineEditLengthFormula->text();

    m_widthOk = !m_widthFormula.trimmed().isEmpty();
    m_lengthOk = !m_lengthFormula.trimmed().isEmpty();
    m_basePointOk = ui->comboBoxBasePoint->currentIndex() >= 0;
    checkState();
}

void DialogRectangle::WidthNameChanged()
{
    m_widthNameOk = !ui->lineEditWidthPoint->text().trimmed().isEmpty();
    checkState();
}

void DialogRectangle::LengthNameChanged()
{
    m_lengthNameOk = !ui->lineEditHeightPoint->text().trimmed().isEmpty();
    checkState();
}

void DialogRectangle::CornerNameChanged()
{
    m_cornerNameOk = !ui->lineEditCornerPoint->text().trimmed().isEmpty();
    checkState();
}

void DialogRectangle::checkState()
{
    const bool enabled = m_widthOk && m_lengthOk && m_widthNameOk && m_lengthNameOk && m_cornerNameOk && m_basePointOk;
    ok_Button->setEnabled(enabled);
    apply_Button->setEnabled(enabled);
}

void DialogRectangle::ShowDialog(bool click)
{
    DialogTool::ShowDialog(click);
}

void DialogRectangle::SaveData()
{
    m_widthFormula = ui->lineEditWidthFormula->text();
    m_lengthFormula = ui->lineEditLengthFormula->text();
}

void DialogRectangle::ShowVisualization()
{
    // No live preview for rectangle construction.
}

quint32 DialogRectangle::GetBasePointId() const
{
    return getCurrentObjectId(ui->comboBoxBasePoint);
}

void DialogRectangle::SetBasePointId(const quint32 &value)
{
    setCurrentPointId(ui->comboBoxBasePoint, value, FillComboBox::NoChildren, 0, 0);
    m_basePointOk = ui->comboBoxBasePoint->currentIndex() >= 0;
    checkState();
}

QString DialogRectangle::GetWidthFormula() const
{
    return m_widthFormula;
}

void DialogRectangle::SetWidthFormula(const QString &value)
{
    ui->lineEditWidthFormula->setText(value);
    m_widthFormula = value;
    FormulaChanged();
}

QString DialogRectangle::GetLengthFormula() const
{
    return m_lengthFormula;
}

void DialogRectangle::SetLengthFormula(const QString &value)
{
    ui->lineEditLengthFormula->setText(value);
    m_lengthFormula = value;
    FormulaChanged();
}

QString DialogRectangle::getWidthPointName() const
{
    return ui->lineEditWidthPoint->text();
}

void DialogRectangle::setWidthPointName(const QString &value)
{
    ui->lineEditWidthPoint->setText(value);
}

QString DialogRectangle::getLengthPointName() const
{
    return ui->lineEditHeightPoint->text();
}

void DialogRectangle::setLengthPointName(const QString &value)
{
    ui->lineEditHeightPoint->setText(value);
}

QString DialogRectangle::getCornerPointName() const
{
    return ui->lineEditCornerPoint->text();
}

void DialogRectangle::setCornerPointName(const QString &value)
{
    ui->lineEditCornerPoint->setText(value);
}

QString DialogRectangle::getLineType() const
{
    return ui->comboBoxLineType->currentData().toString();
}

void DialogRectangle::setLineType(const QString &value)
{
    int index = ui->comboBoxLineType->findData(value);
    if (index != -1)
    {
        ui->comboBoxLineType->setCurrentIndex(index);
    }
}

QString DialogRectangle::getLineWeight() const
{
    return ui->comboBoxLineWeight->currentData().toString();
}

void DialogRectangle::setLineWeight(const QString &value)
{
    int index = ui->comboBoxLineWeight->findData(value);
    if (index != -1)
    {
        ui->comboBoxLineWeight->setCurrentIndex(index);
    }
}

QString DialogRectangle::getLineColor() const
{
    return ui->comboBoxLineColor->currentData().toString();
}

void DialogRectangle::setLineColor(const QString &value)
{
    int index = ui->comboBoxLineColor->findData(value);
    if (index != -1)
    {
        ui->comboBoxLineColor->setCurrentIndex(index);
    }
}

void DialogRectangle::ChosenObject(quint32 id, const SceneObject &type)
{
    if (type == SceneObject::Point)
    {
        setCurrentPointId(ui->comboBoxBasePoint, id, FillComboBox::NoChildren, 0, 0);
        m_basePointOk = ui->comboBoxBasePoint->currentIndex() >= 0;
        checkState();
    }
}

void DialogRectangle::FXWidth()
{
    EditFormulaDialog dialog(data, toolId, ToolDialog, this);
    dialog.setWindowTitle(tr("Edit width"));
    dialog.SetFormula(GetWidthFormula());
    dialog.setPostfix(UnitsToStr(qApp->patternUnit(), true));
    if (dialog.exec() == QDialog::Accepted)
    {
        SetWidthFormula(dialog.GetFormula());
    }
}

void DialogRectangle::FXLength()
{
    EditFormulaDialog dialog(data, toolId, ToolDialog, this);
    dialog.setWindowTitle(tr("Edit length"));
    dialog.SetFormula(GetLengthFormula());
    dialog.setPostfix(UnitsToStr(qApp->patternUnit(), true));
    if (dialog.exec() == QDialog::Accepted)
    {
        SetLengthFormula(dialog.GetFormula());
    }
}
