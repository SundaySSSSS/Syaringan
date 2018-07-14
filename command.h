#ifndef COMMAND_H
#define COMMAND_H

#include <QString>

typedef enum _CommandType
{
    QUERY_LOCAL,
    QUERY_NET,
    OPEN_EXE,
    INVALID_CMD,
}
CommandType;

class Command
{
public:
    Command() { m_type = INVALID_CMD; m_content = ""; }
    Command(CommandType type, QString content);
    void setType(CommandType type) { m_type = type; }
    void setContent(QString content) { m_content = content; }
    CommandType getType() { return m_type; }
    QString getContent() { return m_content; }

    bool isValid() { if (m_type == INVALID_CMD) return false; else return true; }

    static Command createInvalidCmd();
private:
    CommandType m_type;
    QString m_content;
};

#endif // COMMAND_H
