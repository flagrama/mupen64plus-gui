#ifndef CURRENT_INSTRUCTION_WIDGET_H
#define CURRENT_INSTRUCTION_WIDGET_H

#include <QWidget>
#include <QLabel>

class CurrentInstructionWidget : public QWidget
{
    Q_OBJECT

public:
    CurrentInstructionWidget();

public slots:
    void UpdateCurrentInstruction();

private:
    QLabel *programCounter;
    QLabel *currentInstruction;
    QLabel *delaySlot;
};

#endif
