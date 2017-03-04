#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "combobox.h"
#include "qlistview.h"

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
    this->setView(new QListView());
}

