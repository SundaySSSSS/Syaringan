#ifndef CONFIG_H
#define CONFIG_H

#include "../command.h"

class Config
{
public:
    Config();

    static CommandType parseCommandType(QString content);   //分析命令类型
    //根据用户输入, 构造查找关键字的url, 如果input不是合法的查找字串, 则返回空字串
    static QString getNetSearchUrl(QString input);

private:
    static QString getNetSearchPrefix();                //获取网络查找前缀, 前缀 + 关键字 要构成可直接使用的url
    static QString getNetSearchKeyword(QString input);  //从用户输入中取得用户要查找的关键字
};

#endif // CONFIG_H
