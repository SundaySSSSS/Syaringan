#include "config.h"
#include <QStringList>

Config::Config()
{

}

CommandType Config::parseCommandType(QString content)
{
    CommandType type = INVALID_CMD;
    //TODO 应该从配置文件中进行读取
    if (content.startsWith("ns "))
    {
        type = QUERY_NET;
    }
    else
    {
        type = QUERY_LOCAL;
    }
    return type;
}

QString Config::getNetSearchUrl(QString input)
{
    QString url = "";
    QString keyword = getNetSearchKeyword(input);
    if (keyword != QString(""))
    {
        url = getNetSearchPrefix() + keyword;
    }
    return url;
}

QString Config::getNetSearchPrefix()
{
    //TODO 应该从配置文件中读取
    return QString("https://www.baidu.com/s?wd=");
}

QString Config::getNetSearchKeyword(QString input)
{
    QString keyword = "";
    if (input.startsWith("ns "))
    {
        QStringList tempList = input.split(" ");
        if (tempList.size() > 1)
        {   //第一个一定为触发词, 故舍弃第一个
            for (int i = 1; i < tempList.size(); ++i)
            {
                if (i != 1)
                    keyword.append("%20");
                keyword.append(tempList.at(i));
            }
        }
    }
    return keyword;
}
