#include "code_widget.h"
#include "debugger.h"
#include "debuggercommon.h"

#include <cmath>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QFontDatabase>
#include <QString>

CodeWidget::CodeWidget()
{
    setColumnCount(4);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setShowGrid(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked(int, int)));
    Update();
}

void CodeWidget::Update()
{
    if(rowCount() == 0)
        setRowCount(1);

    int rows = std::round((height() / static_cast<float>(rowHeight(0))) - 0.25) + 1;
    setRowCount(rows);

    for(int row = 0; row < rowCount(); row++)
    {
        char op[64], args[64];
        QString qs_args, qs_args_addr;
        const uint32_t addr = m_address - ((rowCount() / 2) * 4) + row * 4;;
        if(DebuggerGetMemory(addr) != M64P_MEM_INVALID)
        {
            DebuggerGetInstruction(op, args, addr);
            GetInstructionStrings(qs_args, qs_args_addr, args);
        }
        else
        {
            strcpy(op, "-");
            strcpy(args, "-");
        }

        auto* bkpt_item = new QTableWidgetItem(QString(" "));
        bkpt_item->setBackgroundColor(Qt::gray);
        auto* addr_item = new QTableWidgetItem(QString("%1").arg(addr, 8, 16, QChar('0')).toUpper());
        auto* op_item = new QTableWidgetItem(QString(op).toUpper());
        auto* args_item = new QTableWidgetItem(QString("%1%2").arg(qs_args, qs_args_addr));

        int breakpoint = DebuggerGetBreakpoint(addr);
        if(breakpoint != -1) 
        {
            bkpt_item->setData(Qt::UserRole, breakpoint);
            bkpt_item->setBackgroundColor(Qt::red);
        }

        setItem(row, 0, bkpt_item);
        setItem(row, 1, addr_item);
        setItem(row, 2, op_item);
        setItem(row, 3, args_item);

        resizeRowToContents(row);

        for(int col = 0; col < columnCount(); col++)
        {
            if(addr == l_DebugProgramCounter)
                if(col != 0)
                    item(row, col)->setBackgroundColor(Qt::yellow);
            resizeColumnToContents(col);
            if(col == 2 && columnWidth(col) < 70)
            {
                setColumnWidth(col, 70);
            }
        }
    }
}

void CodeWidget::SetAddress(uint32_t address)
{
    if(m_address == address)
        return;
    m_address = address;
    Update();
}

void CodeWidget::resizeEvent(QResizeEvent*)
{
    Update();
}

void CodeWidget::wheelEvent(QWheelEvent* event)
{
    auto delta = -static_cast<int>(std::round((event->angleDelta().y() / (SCROLL_FRACTION_DEGREES * 8))));

    if(delta == 0)
        return;
    m_address += delta * sizeof(uint32_t);
    Update();
}

void CodeWidget::cellClicked(int row, int col)
{
    if(col != 0)
        return;
    QTableWidgetItem* breakpoint = item(row, col);
    uint32_t addr = item(row, col + 1)->text().toUInt(nullptr, 16);
    if(breakpoint->backgroundColor() != Qt::red)
    {
        int index = DebuggerSetBreakpoint(addr);
        if(index == -1)
            return;
        breakpoint->setData(Qt::UserRole, index);
        breakpoint->setBackgroundColor(Qt::red);
    }
    else {
        int index = breakpoint->data(Qt::UserRole).toInt();
        DebuggerRemoveBreakpoint(index);
        breakpoint->setData(Qt::UserRole, -1);
        breakpoint->setBackgroundColor(Qt::gray);
    }
}
