#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QListView>
#include "socketclient.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createMenus();

private slots:
    void openFile();
    void run();
    void closeWindow();

private:
    QListView* p_outputView;
    QTextEdit* p_codeEdit;

    SocketClient p_socketClient;
};
#endif // MAINWINDOW_H
