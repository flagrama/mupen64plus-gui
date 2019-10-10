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
    void ForceUpdate();

private:
    QWidget *dataWidget;
    QPushButton *runButton;
    QPushButton *breakButton;
    QPushButton *stepButton;

    void PausedLayout();
    void RunningLayout();

signals:
    void Update(uint32_t addr);
};

#endif
