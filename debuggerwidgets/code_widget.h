#ifndef CODE_WIDGET_H
#define CODE_WIDGET_H

#include "m64p_types.h"

#include <QTableWidget>
#include <QWheelEvent>
#include <QResizeEvent>

const double SCROLL_FRACTION_DEGREES = 15.;

class CodeWidget : public QTableWidget
{
    Q_OBJECT

public:
    CodeWidget();

public slots:
    void SetAddress(uint32_t addr);

private slots:
    void cellClicked(int, int);

private:
    uint32_t m_address = 0x00000000;

    void Update();
    void resizeEvent(QResizeEvent*);
    void wheelEvent(QWheelEvent* event);
};

#endif
