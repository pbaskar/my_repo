#include "celistview.h"
#include "constants.h"

CEListView::CEListView(QWidget *parent)
  : QListView(parent)
{
    setViewportMargins(Constants::MARGIN, Constants::MARGIN, Constants::MARGIN, Constants::MARGIN);
}

CEListView::~CEListView()
{
    qDebug() <<Q_FUNC_INFO;;
}

CEOutputViewDelegate::CEOutputViewDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

CEOutputViewDelegate::~CEOutputViewDelegate()
{
    qDebug() <<Q_FUNC_INFO;
}

void CEOutputViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data();
    qDebug() <<Q_FUNC_INFO <<"model data " <<data.toMap();
    QStyledItemDelegate::paint(painter, option, index);
}
