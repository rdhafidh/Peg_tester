#include "lastfiledialogfolderdir.h"
#include <QFileInfo>

LastFileDialogFolderDir::LastFileDialogFolderDir() {}

LastFileDialogFolderDir *LastFileDialogFolderDir::instance() {
  static LastFileDialogFolderDir dir;
  return &dir;
}

void LastFileDialogFolderDir::setLastdir(const QString &dir) {
  QFileInfo mdir(dir);
  if (mdir.isFile()) {
    currdir = mdir.absolutePath();
  } else {
    if (mdir.isDir()) {
      currdir = dir;
    }
  }
}

QString LastFileDialogFolderDir::getLastDirUsed() {
  if (currdir.isEmpty()) return ".";
  QFileInfo mdir(currdir);
  if(!mdir.isFile () && !mdir.isDir ()){
      return ".";
  }
  return currdir;
}
