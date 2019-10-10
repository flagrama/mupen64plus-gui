#ifndef MEMORY_WIDGET_H
#define MEMORY_WIDGET_H

#include "m64p_types.h"

#include <QTableWidget>
#include <QWheelEvent>
#include <QResizeEvent>

class MemoryWidget : public QTableWidget
{
    Q_OBJECT

public:
    MemoryWidget();

public slots:
    void Update();

private:
    uint32_t m_address = 0x00000000;
    int m_columnWidth = 0;

    void resizeEvent(QResizeEvent*);
    void wheelEvent(QWheelEvent* event);
};

#endif
