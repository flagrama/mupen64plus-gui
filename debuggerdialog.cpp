#include "debuggerdialog.h"
#include "debugger.h"
#include "current_instruction_widget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QTabWidget>

DebuggerDialog::DebuggerDialog()
{
    CurrentInstructionWidget *currentInstructionWidget = new CurrentInstructionWidget();
    runButton = new QPushButton("Run");
    runButton->setDisabled(true);
    connect(runButton, SIGNAL(clicked()), this, SLOT(RunClicked()));
    breakButton = new QPushButton("Break");
    connect(breakButton, SIGNAL(clicked()), this, SLOT(BreakClicked()));
    connect(breakButton, SIGNAL(clicked()), currentInstructionWidget, SLOT(UpdateCurrentInstruction()));
    stepButton = new QPushButton("Step");
    stepButton->setDisabled(true);
    connect(stepButton, SIGNAL(clicked()), this, SLOT(StepClicked()));
    connect(stepButton, SIGNAL(clicked()), currentInstructionWidget, SLOT(UpdateCurrentInstruction()));

    QVBoxLayout *sidebarLayout = new QVBoxLayout();
    sidebarLayout->addWidget(currentInstructionWidget, 0, Qt::AlignTop);
    sidebarLayout->addWidget(runButton, 0, Qt::AlignBottom);
    sidebarLayout->addWidget(stepButton, 0, 0);
    sidebarLayout->addWidget(breakButton, 0, 0);

    QWidget *sidebar = new QWidget();
    sidebar->setMinimumWidth(300);
    sidebar->setLayout(sidebarLayout);

    QHBoxLayout *debuggerLayout = new QHBoxLayout();
    debuggerLayout->addWidget(sidebar);
    this->setMinimumSize(640, 480);
    this->setLayout(debuggerLayout);
}

void DebuggerDialog::RunClicked()
{
    if(DebuggerGetRunState() == M64P_DBG_RUNSTATE_RUNNING)
        return;
    runButton->setDisabled(true);
    breakButton->setEnabled(true);
    stepButton->setDisabled(true);
    DebuggerSetRunState(M64P_DBG_RUNSTATE_RUNNING);
}

void DebuggerDialog::BreakClicked()
{
    if(DebuggerGetRunState() == M64P_DBG_RUNSTATE_PAUSED)
        return;
    runButton->setEnabled(true);
    breakButton->setDisabled(true);
    stepButton->setEnabled(true);
    DebuggerSetRunState(M64P_DBG_RUNSTATE_PAUSED);
}

void DebuggerDialog::StepClicked()
{
    if(DebuggerGetRunState() == M64P_DBG_RUNSTATE_RUNNING)
        return;
    l_DebugLoopWait = true;
    DebuggerSetRunState(M64P_DBG_RUNSTATE_STEPPING);
    DebuggerStep();
}
