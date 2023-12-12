#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QListView>
#include "socketclient.h"
#include "codetextedit.h"
#include "celistview.h"

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

private:
    CEListView* p_outputView;
    CodeTextEdit* p_codeEdit;

   // SocketClient p_socketClient;
};
#endif // MAINWINDOW_H
