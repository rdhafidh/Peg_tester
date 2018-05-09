#ifndef LASTFILEDIALOGFOLDERDIR_H
#define LASTFILEDIALOGFOLDERDIR_H
#include <QString>

class LastFileDialogFolderDir
{
public:
    LastFileDialogFolderDir();
    static LastFileDialogFolderDir* instance();
    void setLastdir(const QString &dir);
    QString getLastDirUsed();
private:
    QString currdir;
};

#endif // LASTFILEDIALOGFOLDERDIR_H