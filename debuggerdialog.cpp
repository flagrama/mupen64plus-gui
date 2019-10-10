#include "debuggerdialog.h"
#include "debugger.h"
#include "current_instruction_widget.h"
#include "code_widget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QCoreApplication>

DebuggerDialog::DebuggerDialog()
{
    connect(this, SIGNAL(Update(uint32_t)), this, SLOT(ForceUpdate()));
    CurrentInstructionWidget *currentInstructionWidget = new CurrentInstructionWidget();
    runButton = new QPushButton("Run");
    runButton->setDisabled(true);
    connect(runButton, SIGNAL(clicked()), this, SLOT(RunClicked()));
    breakButton = new QPushButton("Break");
    connect(breakButton, SIGNAL(clicked()), this, SLOT(BreakClicked()));
    connect(this, SIGNAL(Update(uint32_t)), currentInstructionWidget, SLOT(UpdateCurrentInstruction()));
    stepButton = new QPushButton("Step");
    stepButton->setDisabled(true);
    connect(stepButton, SIGNAL(clicked()), this, SLOT(StepClicked()));

    QVBoxLayout *sidebarLayout = new QVBoxLayout();
    sidebarLayout->addWidget(currentInstructionWidget, 0, Qt::AlignTop);
    sidebarLayout->addWidget(runButton, 0, Qt::AlignBottom);
    sidebarLayout->addWidget(stepButton, 0, 0);
    sidebarLayout->addWidget(breakButton, 0, 0);

    QWidget *sidebar = new QWidget();
    sidebar->setMinimumWidth(260);
    sidebar->setLayout(sidebarLayout);

    CodeWidget *codeWidget = new CodeWidget();
    connect(this, SIGNAL(Update(uint32_t)), codeWidget, SLOT(SetAddress(uint32_t)));
    QTabWidget *dataTabs = new QTabWidget();
    dataTabs->addTab(codeWidget, "Code");
    QVBoxLayout *dataLayout = new QVBoxLayout();
    dataLayout->addWidget(dataTabs);

    dataWidget = new QWidget();
    dataWidget->setDisabled(true);
    dataWidget->setLayout(dataLayout);

    QHBoxLayout *debuggerLayout = new QHBoxLayout();
    debuggerLayout->addWidget(dataWidget);
    debuggerLayout->addWidget(sidebar);
    this->setMinimumSize(700, 480);
    this->setLayout(debuggerLayout);
}

void DebuggerDialog::RunClicked()
{
    if(DebuggerGetRunState() == M64P_DBG_RUNSTATE_RUNNING)
    {
        RunningLayout();
        return;
    }
    DebuggerSetRunState(M64P_DBG_RUNSTATE_RUNNING);
    RunningLayout();
    while(DebuggerGetRunState() == M64P_DBG_RUNSTATE_RUNNING)
    {
        QCoreApplication::processEvents();
        if(DebuggerGetRunState() == M64P_DBG_RUNSTATE_PAUSED)
            break;
    }

    emit Update(l_DebugProgramCounter);
}

void DebuggerDialog::BreakClicked()
{
    if(DebuggerGetRunState() == M64P_DBG_RUNSTATE_PAUSED)
        return;
    l_DebugLoopWait = true;
    DebuggerSetRunState(M64P_DBG_RUNSTATE_PAUSED);
    PausedLayout();
    while(l_DebugLoopWait)
        QCoreApplication::processEvents();
    emit Update(l_DebugProgramCounter);
}

void DebuggerDialog::StepClicked()
{
    if(DebuggerGetRunState() == M64P_DBG_RUNSTATE_RUNNING)
        return;
    l_DebugLoopWait = true;
    DebuggerSetRunState(M64P_DBG_RUNSTATE_STEPPING);
    DebuggerStep();
    while(l_DebugLoopWait)
        QCoreApplication::processEvents();
    emit Update(l_DebugProgramCounter);
}

void DebuggerDialog::ForceUpdate()
{
    PausedLayout();
}

void DebuggerDialog::PausedLayout()
{
    dataWidget->setEnabled(true);
    runButton->setEnabled(true);
    breakButton->setDisabled(true);
    stepButton->setEnabled(true);
}

void DebuggerDialog::RunningLayout()
{
    dataWidget->setDisabled(true);
    runButton->setDisabled(true);
    breakButton->setEnabled(true);
    stepButton->setDisabled(true);
}
