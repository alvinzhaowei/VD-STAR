#include "DTManager.h"

DTInstance *DTManager::removeInstance(int _indexToDel) {
    int length = dtInstanceList.size();
    DTInstance *toDel = dtInstanceList[_indexToDel];
    DTInstance *moved = nullptr;

    if (_indexToDel == length - 1) {
        dtInstanceList.pop_back();
    } else {
        moved = dtInstanceList[length - 1];
        dtInstanceList[_indexToDel] = moved;
        moved->set_dt_index(_indexToDel);
        dtInstanceList.pop_back();
    }

    delete toDel;
    return moved;
}
