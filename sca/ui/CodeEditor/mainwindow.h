#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QListView>
#include <QProcess>
#include <QLabel>
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

private slots:
    void openFile();
    void openSampleOne();
    void openSampleTwo();
    void openSampleThree();
    void openSampleFour();
    void openSampleFive();
    void openSampleSix();
    void tabChanged(int index);
    void saveFile();
    void saveAsFile();
    void run();
    void closeWindow();
    void onResultsAvailable(const QVariantList results);

private:
    void createMenus();
    void createWidgets();

    void openSample(QString fileName);
    void updateViewTab(int index);

    CEListView* p_outputView;
    CodeTextEdit* p_codeEdit;
    QTabWidget* p_codeVisual;
    QLabel* p_codeTextEditLabel;
    QLabel* p_codeBlockLabel;
    QProcess p_server;
    QString p_fileName;
};
#endif // MAINWINDOW_H
