#include <Contacts2Qt.h>
#include <iostream>
bool db2vcf(const QString &db, const QString &vcf)
{
    la(QString("Checking %1").arg(db));
    if(!QFile(db).exists())
    {
        lpe("Failed detect database");
        return false;
    }
    lp("Done");
    Contacts2Qt c2q;
    if(c2q.OpenDB(db))
        if(c2q.SearchContacts())
            if(c2q.SaveToVcf(vcf))
                return true;
    return false;
}

int main(int argc,char *argv[]){

    if(argc==3){
        db2vcf(argv[1],argv[2]);
    }else{
        le("DB2VCF v1.0 ...Written by Muhammad Almuhmmah");
        le("db2vcf [db file] [output file vcf]");
        le("db2vcf contacts2.db contacts.vcf");
    }
    SetConsoleTextAttribute(hConsole, 7);
    return 0;
}
