#ifndef DEBUGGERDIALOG_H
#define DEBUGGERDIALOG_H

#include "search_widget.h"

#include <QDialog>

enum DebuggerTabs
{
    CODE_TAB,
    MEMORY_TAB
};

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
    void UpdateSearchWidget(int);
    void Search(QString);
    void GoTo(uint32_t);

private:
    QWidget *dataWidget = nullptr;
    QPushButton *runButton = nullptr;
    QPushButton *breakButton = nullptr;
    QPushButton *stepButton = nullptr;
    SearchWidget *searchWidget = nullptr;
    DebuggerTabs currentTab = static_cast<DebuggerTabs>(0);

    void PausedLayout();
    void RunningLayout();

signals:
    void Update(uint32_t addr);
    void GoToMemory(uint32_t addr);
    void FindMemory(QString query);
};

#endif
