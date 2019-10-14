#include "debuggercommon.h"

void GetInstructionStrings(QString &qs_args, QString &qs_addr, char* args)
{
    std::string s_args, s_address, s_offset;
    s_args = args;
    uint64_t lastArgPos = s_args.find_last_of(',') + 1;
    uint64_t addressPos = s_args.find("0x", lastArgPos);

    if(addressPos != std::string::npos)
    {
        addressPos += 2;
        s_address = s_args.substr(addressPos);
        s_args = s_args.substr(0, addressPos);
    }
    
    //QString qs_addr;
    if(!s_address.empty())
    {
        qs_addr = QString("%1").arg(stol(s_address, nullptr, 16), 4, 16, QChar('0')).toUpper();
    }
    
    char* isLastArgNaN = nullptr;
    uint64_t offset_pos = s_args.find('(', lastArgPos);
    if(offset_pos != std::string::npos)
    {
        s_offset = s_args.substr(lastArgPos, offset_pos - lastArgPos);
    }
    else if(!s_args.empty())
    {
        s_offset = std::to_string(strtol(s_args.substr(lastArgPos).c_str(), &isLastArgNaN, 16));
        if(*isLastArgNaN)
            s_offset.clear();
    }

    qs_args = s_args.c_str();

    if(!s_offset.empty())
    {
        uint16_t offset = stoi(s_offset, nullptr, 16);
        QString qs_offset = QString("%1").arg(offset, 4, 16, QChar('0')).toUpper();
        if(isLastArgNaN != nullptr && !*isLastArgNaN)
            qs_args.replace(lastArgPos, qs_args.size(), QString("0x%1($sp)").arg(qs_offset));
        else
            qs_args.replace(lastArgPos, s_offset.size(), QString("0x%1").arg(qs_offset));
    }
}
