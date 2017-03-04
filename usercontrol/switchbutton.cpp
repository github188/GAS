#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "switchbutton.h"

SwitchButton::SwitchButton(QWidget *parent): QPushButton(parent)
{
    setCursor(QCursor(Qt::PointingHandCursor));
    isCheck = false;
    styleOn = "border-image:url(:/image/btn_check_on.png);";
    styleOff = "border-image:url(:/image/btn_check_off.png);";

    setFocusPolicy(Qt::NoFocus);    
    setStyleSheet(styleOff);
    connect(this, SIGNAL(pressed()), this, SLOT(ChangeOnOff()));
}

void SwitchButton::ChangeOnOff()
{
    if (isCheck) {
        setStyleSheet(styleOff);
        isCheck = false;
    } else {
        setStyleSheet(styleOn);
        isCheck = true;
    }
}

//设置当前选中状态
void SwitchButton::SetCheck(bool isCheck)
{
    if (this->isCheck != isCheck) {
        this->isCheck = !isCheck;
        ChangeOnOff();
    }
}
