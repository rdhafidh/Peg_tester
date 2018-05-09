#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H
#include <QtCore>

class SettingHandler
{
public:
    SettingHandler();
    
    static bool writeDefaultSettings();
    static bool setSetting(const QString &group, const QString &key, const QVariant & value);
    static QVariant getSetting(const QString & group, const QString & key);
     static QVariant getDefaultValue(const QString &group,const QString &name);
    ~SettingHandler(){}
private:
    
};

#endif // SETTINGHANDLER_H
