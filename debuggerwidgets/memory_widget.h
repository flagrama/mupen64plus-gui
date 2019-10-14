#ifndef MEMORY_WIDGET_H
#define MEMORY_WIDGET_H

#include "m64p_types.h"

#include <QTableWidget>
#include <QWheelEvent>
#include <QResizeEvent>

enum MemoryData
{
    ADDRESS = Qt::UserRole,
    BREAKPOINT_FLAGS,
    BREAKPOINT_INDEX
};

class MemoryWidget : public QTableWidget
{
    Q_OBJECT

public:
    MemoryWidget();

signals:
    void SearchCompleted();

private:
    uint32_t m_address = 0x00000000;
    int m_columnWidth = 0;

    void resizeEvent(QResizeEvent*);
    void wheelEvent(QWheelEvent* event);

public slots:
    void Update();
    void Search(QString);
    void SetAddress(uint32_t);

private slots:
    void ContextMenu();
};

#endif
