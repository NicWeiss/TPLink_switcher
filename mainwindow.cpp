#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "static.h"
#include "menu.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    network = new QNetworkAccessManager(this);
    connect(network, SIGNAL(finished(QNetworkReply*)),
    this, SLOT(network_replyFinished(QNetworkReply*)));
    reqest_inc=0;
    lock=false;
//читаем сохранённые устройства
        QDir temp;
        bool no_file=true;
            temp.setPath("devices/");
            temp.makeAbsolute();
            QStringList files = temp.entryList(QDir::Files,QDir::Name);

            ui->tableWidget->setRowCount(files.length());
                for(int i=0; i<files.length();i++){
                    for(int j=0; j<7; j++){
                      ui->tableWidget->setItem(i, j, new QTableWidgetItem(" "));
                    }
                }
            device_count = files.length(); //колличество устройств

            for(int i=0;i<files.length();i++){
                files[i] = files.at(i);
                    ui->tableWidget->item(i,0)->setBackgroundColor(QColor(255, 0, 0, 255));
               /* QMessageBox msgBox;
                msgBox.setText(files.at(i));
                msgBox.exec();*/

            QFile file("devices/"+files.at(i));
                   if(file.open(QIODevice::ReadOnly |QIODevice::Text))
                   {
                       no_file=false;
                       int tmp;
                       QString tms="";
                       int cur=1;
                       while(!file.atEnd())
                       {
                           //читаем строку
                           QString str = file.readLine();
                           QString tmp = str.left(str.length()-1);
                          if (cur==1) {ui->tableWidget->item(i,1)->setText(tmp);}else{ui->tableWidget->item(i,cur+1)->setText(tmp);}
                            cur++;
                       }

                   }else{
                       no_file=true;
                   }

            }
            ui->tableWidget->resizeColumnsToContents();
            //ui->tableWidget->setResizeMode(QHeaderView::Stretch);

//задаём розетку.
            if(no_file == false){
                token = ui->tableWidget->item(0,3)->text();
                deviceid = ui->tableWidget->item(0,4)->text();
                ui->horizontalGroupBox_2->setTitle(ui->tableWidget->item(0,1)->text());
                row_index=0;
            }

//задаём таймер для обратного отсчёта
        tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
        tmr->setInterval(1000); // Задаем интервал таймера
        connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime())); // Подключаем сигнал таймера к нашему слоту
        tmr->start(); // Запускаем таймер
//задаём таймер для опроса состояния розеток
        tm_query = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
        tm_query->setInterval(10); // Задаем интервал таймера
        connect(tm_query, SIGNAL(timeout()), this, SLOT(updateQuery())); // Подключаем сигнал таймера к нашему слоту
        tm_query->start(); // Запускаем таймер

// Инициализируем иконку трея, устанавливаем иконку из набора системных иконок,
// а также задаем всплывающую подсказку

        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));
// После чего создаем контекстное меню из двух пунктов*/
        QMenu * menu = new QMenu(this);
        QAction * viewWindow = new QAction(trUtf8("Развернуть окно"), this);
        QAction * quitAction = new QAction(trUtf8("Выход"), this);
// подключаем сигналы нажатий на пункты меню к соответсвующим слотам.
// Первый пункт меню разворачивает приложение из трея,
// а второй пункт меню завершает приложение
        connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
        connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
        menu->addAction(viewWindow);
        menu->addAction(quitAction);
// Устанавливаем контекстное меню на иконку
// и показываем иконку приложения в трее
        trayIcon->setContextMenu(menu);
        trayIcon->show();
// Также подключаем сигнал нажатия на иконку к обработчику
// данного нажатия
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

}

//--------------------------------------------------------разрушение формы
MainWindow::~MainWindow()
{
    delete ui;
}

//------------------------------------------------------- Метод, который обрабатывает событие закрытия окна приложения
void MainWindow::closeEvent(QCloseEvent * event)
{
    /* Если окно видимо и чекбокс отмечен, то завершение приложения
     * игнорируется, а окно просто скрывается, что сопровождается
     * соответствующим всплывающим сообщением
     */
    if(this->isVisible()){
        event->ignore();
        this->hide();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);

        trayIcon->showMessage("Sovremennik Power Controller","Успешно свёрнуто!", icon,2000);
    }
}


//------------------------------------------------------ Метод, который обрабатывает нажатие на иконку приложения в трее
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
            /* иначе, если окно видимо, то оно скрывается,
             * и наоборот, если скрыто, то разворачивается на экран
             * */
            if(!this->isVisible()){
                this->show();
            } else {
                this->hide();
            }
        break;
    default:
        break;
    }
}


//------------------------------------------------------ Включение розетки
void MainWindow::on_pushButton_clicked()
{
    state = "1";
    post_request();
}

//------------------------------------------------------ Выключение розетки
void MainWindow::on_pushButton_3_clicked()
{
    state = "0";
    post_request();
}
//------------------------------------------------------ Обновление по таймеру
void MainWindow::updateQuery()
{
   if (!lock and device_count != 0){
        qDebug()<<device_count;
            QNetworkRequest request(QUrl("https://eu-wap.tplinkcloud.com/?token="+ui->tableWidget->item(reqest_inc,3)->text()));
            request.setRawHeader(QByteArray("Content-Type"),QByteArray("application/json"));
            QString str;
            str="{\"method\":\"passthrough\", \"params\": {\"deviceId\": \""+ui->tableWidget->item(reqest_inc,4)->text()+"\", \"requestData\": \"{\\\"system\\\":{\\\"get_sysinfo\\\":null}}\" }}";
            QByteArray ba;
            ba.append(str);
            network->post(request,ba);
            lock=true;
   }

  }
//------------------------------------------------------ Обновление по таймеру
void MainWindow::updateTime()
{
    QString tmp,tmp2,to_lcd_out;
    to_lcd_out="00:00:00";
    for(int i=0;i<sizeof(arr);i++){
    if (arr[i]>0){
        arr[i]--;
        int m = arr[i]/60 % 60;
        int h = arr[i] / 60 / 60;
        int s = arr[i] % 60;
        text = QString::number(h)+":"+QString::number(m)+":"+QString::number(s);
        ui->tableWidget->item(i,2)->setText(text);
        if(row_index == i){
            to_lcd_out=text;
            }
        }
    if (arr[i]==0){
       if(i<device_count){
            ui->tableWidget->item(i,2)->setText("Свободно");
       }
      }
    if (arr[i]==1){
            tmp=token;
            tmp2=deviceid;
            state = "0";
            token = ui->tableWidget->item(i,3)->text();
            deviceid = ui->tableWidget->item(i,4)->text();
            post_request();
            token=tmp;
            deviceid=tmp2;
        }
    }
    ui->lcdNumber->display(to_lcd_out);
    trayIcon->setToolTip(ui->horizontalGroupBox_2->title()+"\n"+"Осталось: "+to_lcd_out);

}

//-------------------------------------------------------- POST запрос на сервер TP-Link
void MainWindow::post_request()
{
    QNetworkRequest request(QUrl("https://eu-wap.tplinkcloud.com/?token="+token));
        request.setRawHeader(QByteArray("Content-Type"),QByteArray("application/json"));
        QString str;
        str="{\"method\":\"passthrough\", \"params\": {\"deviceId\": \""+deviceid+"\", \"requestData\": \"{\\\"system\\\":{\\\"set_relay_state\\\":{\\\"state\\\":"+state+"}}}\" }}";
        QByteArray ba;
        ba.append(str);
        network->post(request,ba);
}

void MainWindow::new_token_request()
{
    QNetworkRequest request(QUrl("https://wap.tplinkcloud.com"));
        request.setRawHeader(QByteArray("Content-Type"),QByteArray("application/json"));
        QString str;
        str="{'method': 'login','params': {'appType': 'Kasa_Android',";
                             str=str+"'cloudUserName': 'it@sovremennik.info',";
                             str=str+"'cloudPassword': 'LsBlk19Rt14ss288Kr',";
                             str=str+"'terminalUUID': 'd764150a-4295-405e-9a91-d2a632e34ac8'";
                             str=str+"}}'";
        QByteArray ba;
        ba.append(str);
        network->post(request,ba);
}
//-------------------------------------------------------- Разбор ответа
void MainWindow::network_replyFinished(QNetworkReply *rep)
{
    //
    QString page;
    QJsonDocument json;
    page = rep->readAll();
    int err_code,i;
    QString msg,new_token;
    bool bool_realy=false;

    //only timer reqest
     QMessageBox msgBox;

    json = QJsonDocument::fromJson(page.toUtf8());
    //qDebug()<<json;
    msg=json.object().value("msg").toString();
    err_code=json.object().value("error_code").toDouble();
    //qDebug()<<'error code: '<<err_code;
    //qDebug()<<'error message: '+msg;
    if (err_code == -20651 ){
        //tmr->stop();
        new_token_request();
    }
    if (err_code == -20571 ){
        if (reqest_inc == row_index) ui->label_2->setText("<html><head/><body><p><span style=' font-weight:600; color:#000;'>нет питания</span></p></body></html>");
        ui->tableWidget->item(reqest_inc,0)->setBackgroundColor(QColor(0, 0, 0, 255));
    }
    if (err_code == 0 ){
        i=ui->tableWidget->currentRow();
        new_token=json.object().value("result").toObject().value("token").toString();
        if(new_token != ""){
                //qDebug()<<"New Token: "+new_token;
                token=json.object().value("result").toObject().value("token").toString();
                ui->tableWidget->item(reqest_inc,3)->setText(new_token);
                QFile fileOut("devices/"+ui->tableWidget->item(reqest_inc,1)->text());
                if(fileOut.open(QIODevice::WriteOnly)){
                        QTextStream writeStream(&fileOut);
                        writeStream.setCodec("UTF8");
                        writeStream << ui->tableWidget->item(reqest_inc,1)->text()+"\r\n"; //name
                        writeStream << ui->tableWidget->item(reqest_inc,3)->text()+"\r\n"; //token
                        writeStream <<ui->tableWidget->item(reqest_inc,4)->text()+"\r\n"; //device
                        fileOut.close();
                        lock=false;
                }
        }
       // token = ui->tableWidget->item(0,3)->text();
    }

                    json = QJsonDocument::fromJson(json.object().value("result").toObject().value("responseData").toString().toUtf8());
                    realy_state = json.object().value("system").toObject().value("get_sysinfo").toObject().value("relay_state").toDouble();
                    responce_device =  json.object().value("system").toObject().value("get_sysinfo").toObject().value("deviceId").toString();
     if(json.object().value("system").toString() != " "){bool_realy=true;}


     if (realy_state==1){
            if (responce_device == deviceid) ui->label_2->setText("<html><head/><body><p><span style=' font-weight:600; color:#090;'>работает</span></p></body></html>");
            if (ui->tableWidget->item(reqest_inc,4)->text()==responce_device)ui->tableWidget->item(reqest_inc,0)->setBackgroundColor(QColor(0, 225, 0, 255));
     };
     if (realy_state==0){
         if (responce_device == deviceid) ui->label_2->setText("<html><head/><body><p><span style=' font-weight:600; color:#900;'>выключен</span></p></body></html>");
         if (ui->tableWidget->item(reqest_inc,4)->text()==responce_device)ui->tableWidget->item(reqest_inc,0)->setBackgroundColor(QColor(255, 0, 0, 255));
     };

     //qDebug()<<"Realy_sate: "<<bool_realy;
     if (bool_realy){
            lock=false;
            reqest_inc++;
            if (reqest_inc>device_count-1){reqest_inc=0;}
     }

}

//--------------------------------------------------------- Установка таймера
void MainWindow::on_pushButton_4_clicked()
{
    text = ui->timeEdit->time().toString();
    arr[row_index] = ui->timeEdit->dateTime().toSecsSinceEpoch();
    state = "1";
    post_request();

}

void MainWindow::on_pushButton_2_clicked()
{
    menu *m = new menu;
    menu_type = 0;
    m->show();
}

void MainWindow::on_pushButton_5_clicked()  //удаление устройства
{
    int i;
    i = ui->tableWidget->currentRow();
    QFile("devices/"+ui->tableWidget->item(i,1)->text()).remove();
    QMessageBox msgBox;
    msgBox.setText("Устройство \""+ui->tableWidget->item(i,1)->text() +"\" было удалено.");
    msgBox.exec();
}

void MainWindow::on_tableWidget_clicked(const QModelIndex &index)
{
    int i;
    i = ui->tableWidget->currentRow();
    row_index = ui->tableWidget->currentRow();
    token = ui->tableWidget->item(i,3)->text();
    deviceid = ui->tableWidget->item(i,4)->text();
    ui->horizontalGroupBox_2->setTitle(ui->tableWidget->item(i,1)->text());
    QMessageBox msgBox;
    msgBox.setText("Выбрано \""+ui->tableWidget->item(i,1)->text());
   // msgBox.exec();
}

void MainWindow::on_pushButton_7_clicked()
{
    new_token_request();
}
