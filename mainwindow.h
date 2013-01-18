#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMutex>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QTcpSocket *_pSocket;
    QStringList _userList;
    QMutex _mutex;
    void updateUserList();
    void test();

public slots:
    void readTcpData();
private slots:
    void on_pushButton_clicked();
    void on_actionConnect_triggered();
    void on_reconnectButton_clicked();
};

#endif // MAINWINDOW_H
