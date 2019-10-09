#ifndef DEBUGGERDIALOG_H
#define DEBUGGERDIALOG_H

#include <QDialog>

class DebuggerDialog : public QDialog
{
    Q_OBJECT

public:
    DebuggerDialog();

private slots:
    void RunClicked();
    void BreakClicked();
    void StepClicked();

private:
    QPushButton *runButton;
    QPushButton *breakButton;
    QPushButton *stepButton;
};

#endif
