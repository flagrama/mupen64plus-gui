#ifndef REGISTERS_WIDGETS_H
#define REGISTERS_WIDGETS_H

#include <QWidget>
#include <QGridLayout>

static const std::string generalPurposeRegisterNames[] = {
    "$r0",
    "$at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "$gp",
    "$sp",
    "sB",
    "$ra"
};

class RegistersWidget : public QWidget
{
    Q_OBJECT

public:
    RegistersWidget();
public slots:
    void UpdateRegisters();
private:
    QGridLayout *layout;
};

#endif
