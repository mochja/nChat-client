#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>
#include <QList>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->installEventFilter(this);
    _pSocket = new QTcpSocket( this );
    connect( _pSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );
    on_actionConnect_triggered();
    ui->userNameEdit->setText("nick");
    ui->hostEdit->setText("127.0.0.1");
    ui->portEdit->setText("9000");
}

void MainWindow::updateUserList() {
    ui->userListView->setModel( new QStringListModel(_userList) ) ;
}

void MainWindow::readTcpData() {
    QByteArray data = _pSocket->readAll();
    QDataStream ds(&data, QIODevice::ReadOnly);
    quint32 t;
    quint8 id;
    ds >> t;
    ds >> id;
    if (t != 1) {
        ui->textEdit->append("<span style=\"color: red; font-weight: bold\">unknow packet " + (QString) id + "</span>");
        return;
    }
    switch (id) {
        case 0x01: {
            QString txt;
            ds >> txt;
            ui->textEdit->append(txt.toHtmlEscaped());
            break;
        }
        case 0x02: {
            QList<QString> userList;
            ds >> userList;
            _userList = userList;
            updateUserList();
            break;
        }
        case 0x03: {
            QString txt;
            ds >> txt;
            ui->textEdit->moveCursor(QTextCursor::End);
            ui->textEdit->append(txt);
            break;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test() {
    QByteArray data;
    QDataStream ds(&data, QIODevice::ReadWrite);
    QList<QString> userList;
    userList.append("test");
    userList.append("nick");
    ds << userList;
    _pSocket->write( data );
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray data;
    QDataStream ds(&data, QIODevice::ReadWrite);
    ds << (qint32) 1;
    ds << (quint8) 0x02;
    ds << ui->msg->toPlainText().toUtf8();
    _pSocket->write( data );
    ui->msg->clear();
}

void MainWindow::on_actionConnect_triggered()
{
    QString nick = ui->userNameEdit->text();

    if (_pSocket->state() == QTcpSocket::ConnectedState) {
        _pSocket->disconnect();
        _pSocket->abort();
        delete _pSocket;
        _pSocket = new QTcpSocket( this );
        connect( _pSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );
    }

    _pSocket->connectToHost(ui->hostEdit->text(), ui->portEdit->text().toInt());
    if( _pSocket->waitForConnected() ) {
        ui->textEdit->append("Connected...\n");
        ui->pushButton->setDisabled(false);
        QByteArray *data = new QByteArray();
        QDataStream ds(data, QIODevice::ReadWrite);
        ds << (qint32) 1;
        ds << (quint16) (nick.length()*2 + 1);
        ds << (quint8) 0x01;
        ds << nick.toUtf8();
        _pSocket->write( data->begin(), data->length() );
        delete data;
    }
}

void MainWindow::on_reconnectButton_clicked()
{
    on_actionConnect_triggered();
}
