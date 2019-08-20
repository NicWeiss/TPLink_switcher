#include "menu.h"
#include "ui_menu.h"
#include "static.h"

menu::menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::menu)
{
    ui->setupUi(this);

    if (menu_type==0){
       ui->label->setText("<html><head/><body><p><span style=' font-size:20pt;'>Добавление нового устройства</span></p></body></html>");
    }
}

menu::~menu()
{
    delete ui;
}

/*
 * QFile file("moConfig.ini");
       if(file.open(QIODevice::ReadOnly |QIODevice::Text))
       {
           int tmp;
           QString tms="";
           while(!file.atEnd())
           {
               //читаем строку
               QString str = file.readLine();
               //Делим строку на слова разделенные пробелом
               QStringList lst = str.split(" ");
               //заносим значения в переменные
               if(lst.at(0).at(0) != QString("#")){
                   if(lst.at(0)=="dbhost"){
                       dbhost=lst.at(2);
                       tmp = lst.at(2).length();
                       dbhost=dbhost.left(tmp-1);
                   }
                   if(lst.at(0)=="dbname"){
                       dbname=lst.at(2);
                       tmp = lst.at(2).length();
                       dbname=dbname.left(tmp-1);
                   }
                   if(lst.at(0)=="dbuser"){
                       dbuser=lst.at(2);
                       tmp = lst.at(2).length();
                       dbuser=dbuser.left(tmp-1);
                   }
                   if(lst.at(0)=="dbpass"){
                       dbpass=lst.at(2);
                       tmp = lst.at(2).length();
                       dbpass=dbpass.left(tmp-1);
                   }
               }
           }

       }
 *
 *
 */

void menu::on_pushButton_clicked()
{
    if(!QDir("devices").exists()){
        QDir().mkdir("devices");
    }
if (menu_type==0){
    QFile fileOut("devices/"+ui->lineEdit_3->text().toUtf8());
    if(fileOut.open(QIODevice::WriteOnly)){
            QTextStream writeStream(&fileOut);
            writeStream.setCodec("UTF8");
            writeStream << QString(ui->lineEdit_3->text().toUtf8()+"\r\n"); //name
            writeStream << QString(ui->lineEdit->text().toUtf8()+"\r\n"); //token
            writeStream << QString(ui->lineEdit_2->text().toUtf8()+"\r\n"); //device
            fileOut.close();
            ui->lineEdit_2->setText("");
            ui->lineEdit_3->setText("");
            QMessageBox msgBox;
            msgBox.setText("Устройство было добавлено.");
            msgBox.exec();
    }
}

if (menu_type==1){
    //modify
}




}
