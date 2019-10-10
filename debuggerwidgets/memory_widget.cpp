#include "memory_widget.h"
#include "debugger.h"
#include "debuggercommon.h"

#include <cmath>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QFontDatabase>
#include <QString>

MemoryWidget::MemoryWidget()
{
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setShowGrid(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAutoScroll(false);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    Update();
}

void MemoryWidget::Update()
{
    if(rowCount() == 0)
    {
        setRowCount(1);
        setColumnCount(1);
        auto* initItem = new QTableWidgetItem(QString("%1").arg(0, 8, 16, QChar('0')).toUpper());
        setItem(0, 0, initItem);
        resizeColumnToContents(0);
        m_columnWidth = columnWidth(0);
        delete item(0, 0);
    }

    setColumnCount(width() / m_columnWidth);
    for(int col = 0; col < columnCount(); col++)
        setColumnWidth(col, m_columnWidth);
    int rows = std::round((height() / static_cast<float>(rowHeight(0))) - 0.25);
    setRowCount(rows);

    for(int row = 0; row < rowCount(); row++)
    {
        const uint32_t addr = m_address - ((rowCount() / 2) * (columnCount() - 1)) + row * (columnCount() - 1);
        auto* addrItem = new QTableWidgetItem(QString("%1").arg(addr, 8, 16, QChar('0')).toUpper());
        addrItem->setBackgroundColor(Qt::gray);
        setItem(row, 0, addrItem);
        resizeRowToContents(row);
        for(int col = 1; col < columnCount(); col++)
        {
            uint32_t value = DebuggerGetMemory(addr + col - 2);
            QString qs_value;
            if(value != M64P_MEM_INVALID)
                qs_value = QString("%1").arg(value, 8, 16, QChar('0')).toUpper();
            else
                qs_value = QString("-");
            auto* memoryItem = new QTableWidgetItem(qs_value);
            memoryItem->setTextAlignment(Qt::AlignCenter);
            setItem(row, col, memoryItem);
        }
    }
}

void MemoryWidget::resizeEvent(QResizeEvent*)
{
    Update();
}

void MemoryWidget::wheelEvent(QWheelEvent* event)
{
    auto delta = -static_cast<int>(std::round((event->angleDelta().y() / (SCROLL_FRACTION_DEGREES * 8))));

    if(delta == 0)
        return;
    m_address += delta * (columnCount() - 1);
    Update();
}
