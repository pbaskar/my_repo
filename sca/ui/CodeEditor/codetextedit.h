#ifndef CODETEXTEDIT_H
#define CODETEXTEDIT_H

#include <QWidget>
#include <QTextEdit>
#include <QWheelEvent>

class CodeTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    CodeTextEdit(QWidget *parent = nullptr);
protected:
    void wheelEvent(QWheelEvent *e);
};

#endif // CODETEXTEDIT_H
