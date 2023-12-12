#include "mainwindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QQuickWidget>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QFile>
#include <QStringListModel>
#include <QScreen>
#include "constants.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createWidgets();
    createMenus();

    QSize s = qApp->primaryScreen()->availableSize();
    int h = s.height();
    int w = s.width();
    setGeometry(w*0.25,h*0.25,w*0.5,h*0.5);
}

void MainWindow::createWidgets()
{
    QSplitter* mainWidget = new QSplitter;
    mainWidget->setChildrenCollapsible(false);

    QQuickWidget* cfgQuickWidget = new QQuickWidget;
    cfgQuickWidget->setSource(QUrl("qrc:/Visualizer/CFGView.qml"));

    QVBoxLayout* codeTextEditLayout  = new QVBoxLayout;
    QWidget* codeTextEditWidget = new QWidget;

    QLabel* codeTextEditLabel = new QLabel(tr("Code"));
    p_codeEdit = new CodeTextEdit;
    codeTextEditLayout->addWidget(codeTextEditLabel);
    codeTextEditLayout->addWidget(p_codeEdit);
    codeTextEditWidget->setLayout(codeTextEditLayout);

    QVBoxLayout* outputViewLayout  = new QVBoxLayout;
    QWidget* outputViewWidget = new QWidget;

    QLabel* outputViewLabel = new QLabel(tr("Output"));
    p_outputView = new CEListView;
    outputViewLayout->addWidget(outputViewLabel);
    outputViewLayout->addWidget(p_outputView);
    outputViewWidget->setLayout(outputViewLayout);

    QTabWidget* codeVisual = new QTabWidget;
    codeVisual->addTab(codeTextEditWidget, tr("Text"));
    codeVisual->addTab(cfgQuickWidget, tr("Visual"));

    mainWidget->addWidget(codeVisual);
    mainWidget->addWidget(outputViewWidget);

    setCentralWidget(mainWidget);
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = new QMenu(tr("File"));
    QAction* openFileAction = fileMenu->addAction(tr("Open"), QKeySequence(Qt::ALT | Qt::Key_O));
    QAction* runAction = fileMenu->addAction(tr("Run"), QKeySequence(Qt::ALT | Qt::Key_R));
    QAction* closeAction = fileMenu->addAction(tr("Close"), QKeySequence(Qt::ALT | Qt::Key_C));
    menuBar()->addMenu(fileMenu);

    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(runAction, &QAction::triggered, this, &MainWindow::run);
    connect(closeAction, &QAction::triggered, this, &MainWindow::close);

    QToolBar* fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openFileAction);
    fileToolBar->addAction(runAction);
    fileToolBar->addAction(closeAction);
}

void MainWindow::openFile()
{
    QFile file("C:\\workspace\\my_repo\\sca\\test\\instructions.c");
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;
    QTextStream ts(&file);
    p_codeEdit->setPlainText(ts.readAll());
    file.close();
    statusBar()->showMessage(tr("File opened"), 2000);
}

void MainWindow::run()
{
    QStringListModel* model = new QStringListModel(this);
    QStringList messages;
    messages <<"a" <<"b" <<"c";
    model->setStringList(messages);
    p_outputView->setModel(model);
    //p_socketClient.writeToSocket("run");
    statusBar()->showMessage(tr("Run command"), 2000);
}

void MainWindow::closeWindow()
{
    close();
}

MainWindow::~MainWindow()
{
}

