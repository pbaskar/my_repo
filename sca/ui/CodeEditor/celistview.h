#ifndef CELISTVIEW_H
#define CELISTVIEW_H

#include <QListView>
#include <QStyledItemDelegate>

class CEListView : public QListView
{
    Q_OBJECT
public:
    CEListView(QWidget *parent = nullptr);
    ~CEListView();
};

class CEOutputViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CEOutputViewDelegate(QWidget *parent = nullptr);
    ~CEOutputViewDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CELISTVIEW_H
