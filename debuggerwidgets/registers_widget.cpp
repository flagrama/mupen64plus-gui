#include "registers_widget.h"
#include "general_purpose_registers_widget.h"
#include "core_interface.h"

RegistersWidget::RegistersWidget()
{
    layout = new QGridLayout(this);

    for(int i = 0; i < 32; i++)
    {
        GeneralPurposeRegisterWidget *thisRegister = new GeneralPurposeRegisterWidget(generalPurposeRegisterNames[i].c_str());
        layout->addWidget(thisRegister, i % 16, i / 16);
    }
}

void RegistersWidget::UpdateRegisters()
{
    uint64_t *registers = static_cast<uint64_t*>((*DebugGetCPUDataPtr)(M64P_CPU_REG_REG));
    for(int i = 0; i < 32; i++)
    {
        GeneralPurposeRegisterWidget *thisRegister = static_cast<GeneralPurposeRegisterWidget*>(this->layout->itemAt(i)->widget());
        thisRegister->UpdateValue(registers[i]);
    }
}
