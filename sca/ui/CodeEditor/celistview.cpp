#include "celistview.h"
#include "constants.h"

CEListView::CEListView(QWidget *parent)
  : QListView(parent)
{
    setViewportMargins(Constants::MARGIN, Constants::MARGIN, Constants::MARGIN, Constants::MARGIN);
}
