#include "memory_widget.h"
#include "debugger.h"
#include "debuggercommon.h"

#include <cmath>
#include <sstream>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QFontDatabase>
#include <QString>
#include <QCoreApplication>
#include <QGuiApplication>

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
        const uint32_t addr = m_address - ((rowCount() / 2) * (columnCount() - 1) * 4) + row * (columnCount() - 1) * 4;
        auto* addrItem = new QTableWidgetItem(QString("%1").arg(addr, 8, 16, QChar('0')).toUpper());
        if(addr == m_address)
            addrItem->setBackgroundColor(Qt::yellow);
        else
            addrItem->setBackgroundColor(Qt::gray);
        setItem(row, 0, addrItem);
        resizeRowToContents(row);
        for(int col = 0; col < columnCount(); col++)
        {
            uint32_t value = DebuggerGetMemory(addr + col * 4);
            QString qs_value;
            if(value != M64P_MEM_INVALID)
                qs_value = QString("%1").arg(value, 8, 16, QChar('0')).toUpper();
            else
                qs_value = QString("-");
            auto* memoryItem = new QTableWidgetItem(qs_value);
            memoryItem->setTextAlignment(Qt::AlignCenter);
            setItem(row, col + 1, memoryItem);
        }
    }
}

void MemoryWidget::SetAddress(uint32_t address)
{
    if(m_address == address)
        return;
    m_address = address;
    Update();
}

void MemoryWidget::Search(QString query)
{
    uint32_t addr = m_address;
    QGuiApplication::setOverrideCursor(Qt::CursorShape::WaitCursor);
    for(uint32_t i = 0; i < 0x2000000; i++)
    {
        addr += 8;
        QCoreApplication::processEvents();
        uint64_t value = DebuggerGetMemory(addr, 64);
        if(value == M64P_MEM_INVALID)
            continue;
        std::stringstream stream;
        stream << std::hex << value;
        std::string s_value( stream.str() );
        if(s_value.find(query.toLower().toStdString()) != std::string::npos)
            break;
    }
    SetAddress(addr);
    QGuiApplication::setOverrideCursor(Qt::CursorShape::ArrowCursor);
    emit SearchCompleted();
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
    m_address += delta * (columnCount() - 1) * 4;
    Update();
}
