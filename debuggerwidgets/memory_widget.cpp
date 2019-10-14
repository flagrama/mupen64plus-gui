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
#include <QMenu>

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
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ContextMenu()));
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
            uint32_t memAddr = addr + col * 4;
            uint32_t value = DebuggerGetMemory(memAddr);
            QString qs_value;
            if(value != M64P_MEM_INVALID)
                qs_value = QString("%1").arg(value, 8, 16, QChar('0')).toUpper();
            else
                qs_value = QString("-");
            auto* memoryItem = new QTableWidgetItem(qs_value);
            memoryItem->setTextAlignment(Qt::AlignCenter);
            memoryItem->setData(MemoryData::ADDRESS, memAddr);
            int breakpointFlags = DebuggerGetBreakpoint(memAddr, m64p_dbg_bkp_flags(0));
            if(breakpointFlags != -1)
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, breakpointFlags);
                int breakpointIndex = DebuggerGetBreakpoint(memAddr, m64p_dbg_bkp_flags(breakpointFlags));
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, breakpointIndex);
                if(breakpointFlags == (M64P_BKP_FLAG_READ | M64P_BKP_FLAG_WRITE | M64P_BKP_FLAG_ENABLED))
                    memoryItem->setBackgroundColor(Qt::magenta);
                if(breakpointFlags == (M64P_BKP_FLAG_READ | M64P_BKP_FLAG_ENABLED))
                    memoryItem->setBackgroundColor(Qt::cyan);
                if(breakpointFlags == (M64P_BKP_FLAG_WRITE | M64P_BKP_FLAG_ENABLED))
                    memoryItem->setBackgroundColor(Qt::green);
            }
            else
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, 0);
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, -1);
            }
            
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

void MemoryWidget::ContextMenu()
{
    QTableWidgetItem* memoryItem = itemAt(mapFromGlobal(QCursor::pos()));
    if(memoryItem != nullptr && memoryItem->column() != 0)
    {
        unsigned int breakpointFlags = memoryItem->data(MemoryData::BREAKPOINT_FLAGS).toUInt();

        QMenu *breakpointMenu = new QMenu();
        QAction *addReadBreakpoint = new QAction("Add Read Breakpoint");
        connect(addReadBreakpoint, &QAction::triggered, [=] 
        {
            m64p_dbg_bkp_flags newFlags = m64p_dbg_bkp_flags(breakpointFlags | M64P_BKP_FLAG_READ);
            if(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt() != -1)
            {
                DebuggerRemoveBreakpoint(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt());
            }

            int index = DebuggerSetBreakpoint(memoryItem->data(MemoryData::ADDRESS).toUInt(), newFlags);
            if(index != -1)
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, newFlags);
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, index);
            }
            else
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, 0);
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, -1);
            }
            Update();
        });
        QAction *removeReadBreakpoint = new QAction("Remove Read breakpoint");
        connect(removeReadBreakpoint, &QAction::triggered, [=] 
        {
            m64p_dbg_bkp_flags newFlags = m64p_dbg_bkp_flags(breakpointFlags & ~M64P_BKP_FLAG_READ);
            if(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt() != -1)
            {
                DebuggerRemoveBreakpoint(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt());
            }
            if(newFlags != M64P_BKP_FLAG_ENABLED)
            {
                int index = DebuggerSetBreakpoint(memoryItem->data(MemoryData::ADDRESS).toUInt(), newFlags);
                if(index != -1)
                {
                    memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, newFlags);
                    memoryItem->setData(MemoryData::BREAKPOINT_INDEX, index);
                }
            }
            else
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, 0);
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, -1);
            }
            Update();
        });
        QAction *addWriteBreakpoint = new QAction("Add Write Breakpoint");
        connect(addWriteBreakpoint, &QAction::triggered, [=] 
        {
            m64p_dbg_bkp_flags newFlags = m64p_dbg_bkp_flags(breakpointFlags | M64P_BKP_FLAG_WRITE);
            if(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt() != -1)
            {
                DebuggerRemoveBreakpoint(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt());
            }

            int index = DebuggerSetBreakpoint(memoryItem->data(MemoryData::ADDRESS).toUInt(), newFlags);
            if(index != -1)
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, newFlags);
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, index);
            }
            else
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, 0);
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, -1);
            }
            Update();
        });
        QAction *removeWriteBreakpoint = new QAction("Remove Write breakpoint");
        connect(removeWriteBreakpoint, &QAction::triggered, [=] 
        {
            m64p_dbg_bkp_flags newFlags = m64p_dbg_bkp_flags(breakpointFlags & ~M64P_BKP_FLAG_WRITE);
            if(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt() != -1)
            {
                DebuggerRemoveBreakpoint(memoryItem->data(MemoryData::BREAKPOINT_INDEX).toInt());
            }
            
            if(newFlags != M64P_BKP_FLAG_ENABLED)
            {
                int index = DebuggerSetBreakpoint(memoryItem->data(MemoryData::ADDRESS).toUInt(), newFlags);
                if(index != -1)
                {
                    memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, newFlags);
                    memoryItem->setData(MemoryData::BREAKPOINT_INDEX, index);
                }
            }
            else
            {
                memoryItem->setData(MemoryData::BREAKPOINT_FLAGS, 0);
                memoryItem->setData(MemoryData::BREAKPOINT_INDEX, -1);
            }
            Update();
        });

        if(breakpointFlags == (M64P_BKP_FLAG_ENABLED | M64P_BKP_FLAG_READ | M64P_BKP_FLAG_WRITE))
        {
            breakpointMenu->addAction(removeReadBreakpoint);
            breakpointMenu->addAction(removeWriteBreakpoint);
        }
        else if(breakpointFlags == (M64P_BKP_FLAG_ENABLED | M64P_BKP_FLAG_READ))
        {
            breakpointMenu->addAction(removeReadBreakpoint);
            breakpointMenu->addAction(addWriteBreakpoint);
        }
        else if(breakpointFlags == (M64P_BKP_FLAG_ENABLED | M64P_BKP_FLAG_WRITE))
        {
            breakpointMenu->addAction(addReadBreakpoint);
            breakpointMenu->addAction(removeWriteBreakpoint);
        }
        else
        {
            breakpointMenu->addAction(addReadBreakpoint);
            breakpointMenu->addAction(addWriteBreakpoint);
        }
        breakpointMenu->exec(QCursor::pos());
        breakpointMenu->clear();
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
    m_address += delta * (columnCount() - 1) * 4;
    Update();
}
