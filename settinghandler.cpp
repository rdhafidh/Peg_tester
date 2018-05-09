#include "settinghandler.h"

SettingHandler::SettingHandler()
{
}

bool SettingHandler::writeDefaultSettings()
{ 
    QScopedPointer<QSettings> settings(new QSettings("pengaturan.ini",QSettings::IniFormat));
    
    settings->setIniCodec("UTF-8");
    settings->setPath(QSettings::IniFormat,QSettings::UserScope,".");
    settings->setDefaultFormat(QSettings::IniFormat);
    
    settings->beginGroup("startup");
    settings->setValue("awal",true);
    settings->endGroup();
    
    settings->beginGroup("parserlint");
    settings->setValue("parserlint1","");
    settings->setValue("multikompiler",false);
    settings->setValue("parserlint2","");
    settings->setValue("grammarfile","");
    settings->setValue("testfile","");
    settings->setValue("modeverbose",false);
    settings->setValue("multikonkurensionly",false);
    settings->setValue("multikonkurensidantidak",true);
    settings->setValue("linuxbenchmark",false);
    settings->endGroup();
    
    settings->beginGroup("laporangrafik");
    settings->setValue("iterasitest",10);
    settings->setValue("tabulardata",false);
    settings->setValue("namaexcel","testfile");
    settings->setValue("satuanbenchmark","nano");
    settings->endGroup();
    return settings->isWritable();
}

bool SettingHandler::setSetting(const QString &group, const QString &key, const QVariant &value)
{
    QScopedPointer<QSettings> settings(new QSettings("pengaturan.ini",QSettings::IniFormat));
    
    settings->setIniCodec("UTF-8");
    settings->setPath(QSettings::IniFormat,QSettings::UserScope,".");
    settings->setDefaultFormat(QSettings::IniFormat);
    settings->beginGroup(group);
    settings->setValue(key,value);
    settings->endGroup();
    settings->sync();
    return settings->isWritable();
}

QVariant SettingHandler::getSetting(const QString &group, const QString &key)
{ 
        QScopedPointer<QSettings> settings(new QSettings("pengaturan.ini",QSettings::IniFormat));
    
        settings->setIniCodec("UTF-8");
        settings->setPath(QSettings::IniFormat,QSettings::UserScope,"."); 
        return settings->value(group + "/" + key,getDefaultValue(group,key));
} 
QVariant SettingHandler::getDefaultValue(const QString &group, const QString &name)
{
    if(group == "parserlint" && name == "parserlint1")
        return "";
 
    if(group == "parserlint" && name == "multikompiler")
        return false;
    
    if(group == "parserlint" && name == "parserlint2")
        return "";  
    
    if(group == "parserlint" && name == "grammarfile")
        return "";  
    
    if(group == "parserlint" && name == "testfile")
        return "";  
    
    if(group == "parserlint" && name == "modeverbose")
        return false;  
    
    if(group == "parserlint" && name == "multikonkurensionly")
        return false;  
    
    if(group == "parserlint" && name == "multikonkurensidantidak")
        return true;  
    
    //second group
    if(group == "parserlint" && name == "tabulardata"){
        return false;  
    }
    if(group == "laporangrafik" && name == "iterasitest"){
        return 10;  
    } 
    if (group=="laporangrafik" && name =="namaexcel"){
        return "testfile";
    }
    if (group=="parserlint" && name =="linuxbenchmark"){
        return false;
    }
    if (group=="parserlint" && name=="satuanbenchmark"){
        return "nano";
    }
    return ""; 
}
