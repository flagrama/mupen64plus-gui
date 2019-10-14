#ifndef GENERAL_PURPOSE_REGISTER_WIDGET_H
#define GENERAL_PURPOSE_REGISTER_WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class GeneralPurposeRegisterWidget : public QWidget
{
public:
    GeneralPurposeRegisterWidget(std::string name, QWidget *parent = nullptr);
    void UpdateValue(uint64_t value);
private:
    QHBoxLayout *layout;
};

#endif
