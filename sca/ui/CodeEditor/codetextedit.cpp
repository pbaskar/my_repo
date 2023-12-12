#include "codetextedit.h"
#include "constants.h"

CodeTextEdit::CodeTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setFontFamily(Constants::FONT_FAMILY);
    setFontPointSize(Constants::FONT_POINT_SIZE);
    setTextColor(Constants::FONT_COLOR);
    setViewportMargins(Constants::MARGIN, Constants::MARGIN, Constants::MARGIN, Constants::MARGIN);
}

void CodeTextEdit::wheelEvent(QWheelEvent *e)
{
    QTextEdit::wheelEvent(e);
    qDebug() << "wheel event " <<e->isAccepted();
    if(!e->isAccepted() && e->modifiers() & Qt::ControlModifier )
    {
        qDebug()<<"text edit wheel event " << e->angleDelta().x() <<" " <<e->angleDelta().y()<< " "<<fontPointSize()<<" " <<e->isAccepted();
        selectAll();
        setFontPointSize(qMax(fontPointSize(),Constants::FONT_POINT_SIZE) +  e->angleDelta().y()/120 );
        e->accept();
    }
}
