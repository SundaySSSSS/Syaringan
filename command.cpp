#include "command.h"


Command::Command(CommandType type, QString content)
{
    m_type = type;
    m_content = content;
}

Command Command::createInvalidCmd()
{
    Command cmd(INVALID_CMD, "");
    return cmd;
}
