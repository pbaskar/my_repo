#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QListView>
#include "socketclient.h"
#include "codetextedit.h"
#include "celistview.h"
#include "cfgmodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createMenus();
    void createWidgets();

private slots:
    void openFile();
    void run();
    void closeWindow();
    void onResultsAvailable(const QVariantList results);

private:
    CEListView* p_outputView;
    CodeTextEdit* p_codeEdit;
};
#endif // MAINWINDOW_H
