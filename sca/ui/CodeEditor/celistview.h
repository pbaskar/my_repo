#ifndef CELISTVIEW_H
#define CELISTVIEW_H

#include <QListView>

class CEListView : public QListView
{
    Q_OBJECT
public:
    CEListView(QWidget *parent = nullptr);
};

#endif // CELISTVIEW_H
