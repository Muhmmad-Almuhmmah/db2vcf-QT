/***
 * -------------------------------------------------
#
# Project created by QtCreator 2020-02-20T21:31:19
# written By Muhammad Almuhmmah
#
-------------------------------------------------
***/

#ifndef CONTACTS2QT_H
#define CONTACTS2QT_H
#include <QSqlDatabase>
#include <QDebug>
#include <QtSql>
#include <QThread>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <iostream>
#include <fstream>
using namespace std;
#define sleep(x) QThread::msleep(x)
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void la(QString commant,int i=0)
{
    SetConsoleTextAttribute(hConsole, 15);
    if(i==0)
        cout <<commant.toStdString()<<" : ";
    else
        cout <<commant.toStdString();
}

void lp(const QString &result)
{
    SetConsoleTextAttribute(hConsole, 9);
    cout <<result.toStdString()<<std::endl;
}

void lpe(const QString &result)
{
    SetConsoleTextAttribute(hConsole, 12);
    cout <<result.toStdString()<<std::endl;
}

void le(const QString &commant)
{
    SetConsoleTextAttribute(hConsole, 12);
    cout <<commant.toStdString()<<std::endl;
}


class Contacts2Qt
{
    typedef struct Vcf{
        QString Name;
        QString Number;
        QString Email;
        bool CheckVcf(bool BypassEmail=true){
            if(Name.isEmpty())
                return false;
            if(Number.isEmpty())
                return false;
            if(BypassEmail)
                return true;
            if(Email.isEmpty())
                return false;
            return true;
        }
    }VcfItem;
    QList<VcfItem> VcfCards;
    QSqlDatabase db;
    int type=0;
public:
    Contacts2Qt(){
        VcfCards.clear();
    }
    ~Contacts2Qt(){
        db.close();
    }
    bool OpenDB(const QString &fileDb){
        la("Opening Data");
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(fileDb);
        if(!db.open())
        {
            lpe("Failed");
            le(db.lastError().text());
            return false;
        }
        sleep(500);
        lp("Done");
        return true;
    }
    bool SearchContacts()
    {
        la("Analyzing Data");
        QSqlQuery contactList=db.exec("SELECT _id,name_raw_contact_id FROM contacts");
        while (contactList.next()){
            SearchInfo(contactList.value(0).toString());
        }
        sleep(500);
        if(VcfCards.count()>0)
            lp("Done");
        else
            lpe("Failed");
        return VcfCards.count();
    }
    bool CheckList(const VcfItem &Item){
        for(int i=0;i<VcfCards.count();i++)
        {
            if(VcfCards.at(i).Name==Item.Name and VcfCards.at(i).Number==Item.Number)
                return true;
        }
        return false;
    }
    bool SearchInfo(const QString &Contact_ID){
        VcfItem item;
        QSqlQuery dataRec=db.exec("select _id,mimetype_id,data1,raw_contact_id from data where raw_contact_id="+Contact_ID);
        while(dataRec.next())
        {
            type=dataRec.value(1).toInt();
            if(type==7)
            {
                item.Name=dataRec.value(2).toString();
            }
            if(type==5){
                item.Number=dataRec.value(2).toString().replace(" ","").replace("-","").remove("\n");
            }
            if(type==1)
                item.Email=dataRec.value(2).toString();
            if(item.Name.isEmpty() or item.Number.isEmpty() or item.Email.isEmpty())
                continue;
            break;
        }
        if(CheckList(item))
            return true;
        VcfCards.append(item);
        return true;
    }
    QString StructItem(VcfItem& item){
        if(!item.CheckVcf())
            return "";
        QString tmp;
        tmp.append("BEGIN:VCARD\nVERSION:2.1\n");
        if(item.Name.length())
            tmp.append(QString("N:;%1;;;\nFN:%1\n").arg(item.Name));
        if(item.Number.length())
            tmp.append(QString("TEL;TYPE=cell:%1\n").arg(item.Number));
        if(item.Email.length())
            tmp.append(QString("EMAIL;TYPE=WORK:%s\n").arg(item.Email));
        tmp.append("END:VCARD\n");
        return tmp;
    }
    QString RawVcf(){
        if(!VcfCards.count())
            return "";
        QString vcf;
        VcfItem item;
        for(int i=0;i<VcfCards.count();i++)
        {
            item=VcfCards.at(i);
            if(!item.CheckVcf())
                continue;
            vcf.append(StructItem(item));
        }
        //QMessageBox::warning(NULL,"Error",vcf);
        return vcf;
    }
    bool SaveToVcf(const QString &file)
    {
        la(QString("Saving To VCF File [%1]").arg(QFileInfo(file).fileName()));
        QFile cfile(file);
        if(!cfile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
            lpe(cfile.errorString());
            return false;
        }
        sleep(500);
        QTextStream stream(&cfile);
        stream<<RawVcf();
        stream.setCodec("UTF-8");
        cfile.flush();
        cfile.close();
        lp("Done");
        return true;
    }
};

#endif // CONTACTS2QT_H
