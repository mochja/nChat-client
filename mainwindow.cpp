#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _pSocket = new QTcpSocket( this );
    connect( _pSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );
    on_actionConnect_triggered();
    ui->userNameEdit->setText("User123");
    ui->hostEdit->setText("127.0.0.1");
    ui->portEdit->setText("9000");
}

void MainWindow::updateUserList() {
    ui->userListView->setModel(new QStringListModel(_userList));
}

void MainWindow::readTcpData() {
    QByteArray data = _pSocket->readAll();
    ui->textEdit->append(data);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray data;
    data.append(ui->msg->toPlainText());
    _pSocket->write( data );
    ui->msg->clear();
}

void MainWindow::on_actionConnect_triggered()
{
    // delete _pSocket;
    QString nick = ui->userNameEdit->text();

    if (_pSocket->state() == QTcpSocket::ConnectedState) {
        _pSocket->abort();
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
