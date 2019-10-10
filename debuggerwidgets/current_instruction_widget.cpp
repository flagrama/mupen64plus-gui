#include "current_instruction_widget.h"
#include "debugger.h"
#include "debuggercommon.h"

#include <QVBoxLayout>
#include <QFontDatabase>

CurrentInstructionWidget::CurrentInstructionWidget()
{
    programCounter = new QLabel();
    QString qs_programCounter = QString("%1").arg(l_DebugProgramCounter, 8, 16, QChar('0')).toUpper();
    programCounter->setText(QString("Program Counter:\n%1").arg(qs_programCounter));
    programCounter->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    currentInstruction = new QLabel();
    currentInstruction->setText(QString("No data..."));
    currentInstruction->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    delaySlot = new QLabel();
    delaySlot->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    delaySlot->hide();

    QVBoxLayout *currentInstructionLayout = new QVBoxLayout();
    currentInstructionLayout->addWidget(programCounter);
    currentInstructionLayout->addSpacerItem(new QSpacerItem(0, 15));
    currentInstructionLayout->addWidget(currentInstruction);
    currentInstructionLayout->addSpacerItem(new QSpacerItem(0, 15));
    currentInstructionLayout->addWidget(delaySlot);

    this->setLayout(currentInstructionLayout);
}

void CurrentInstructionWidget::UpdateCurrentInstruction()
{
    delaySlot->hide();
    int instructionType;

    QString qs_programCounter = QString("%1").arg(l_DebugProgramCounter, 8, 16, QChar('0')).toUpper();
    programCounter->setText(QString("Program Counter:\n%1").arg(qs_programCounter));

    char op[64], args[64];
    instructionType = DebuggerGetInstruction(op, args);

    QString qs_args, qs_addr;
    GetInstructionStrings(qs_args, qs_addr, args);
    currentInstruction->setText(QString("Current Instruction:\n%1\n%2%3").arg(QString(op).toUpper(), qs_args, qs_addr));

    if(instructionType == 2)
    {
        qs_addr.clear();
        DebuggerGetInstruction(op, args, l_DebugProgramCounter + 4);
        GetInstructionStrings(qs_args, qs_addr, args);
        delaySlot->setText(QString("Delay Slot:\n%1\n%2%3").arg(QString(op).toUpper(), qs_args, qs_addr));
        delaySlot->show();
    }
}
