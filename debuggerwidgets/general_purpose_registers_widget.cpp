#include "general_purpose_registers_widget.h"
#include <QFontDatabase>

GeneralPurposeRegisterWidget::GeneralPurposeRegisterWidget(std::string name, QWidget *parent) : QWidget(parent)
{
    layout = new QHBoxLayout(this);
    layout->setMargin(0);

    QLabel *nameLabel = new QLabel(name.c_str());
    nameLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(nameLabel);

    QLabel *valueLabel = new QLabel();
    valueLabel->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);
    valueLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    layout->addWidget(valueLabel);
    UpdateValue(0);
}

void GeneralPurposeRegisterWidget::UpdateValue(uint64_t value)
{
    QLabel *registerWidget = static_cast<QLabel*>(this->layout->itemAt(layout->count() - 1)->widget());
    registerWidget->setText(QString("%1").arg(value, 16, 16, QChar('0')).toUpper());
}
