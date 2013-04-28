#include "preprocessor.hpp"

Preprocessor::Preprocessor()
{
}

qvpvrvr Preprocessor::readFile(const QString& filename){
    qvpvrvr result;
    QFile inputfile(filename);
    if(!inputfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Preprocessor::readFile: unable to open file\n;";
        return result;
    }
    QTextStream inputstream(&inputfile);
    qint32 numberofinputs;
    inputstream >> numberofinputs;
    inputstream.readLine();
    while (!inputstream.atEnd()){
        QString nextline;
        if(inputstream.atEnd()){
            break;
        }
        nextline = inputstream.readLine();
        QTextStream line(&nextline);

        QVector < qreal > inputparameters(numberofinputs);
        for(qint32 index = 0; index < numberofinputs ;++ index){
            qreal tmp;
            line >> tmp;
            if(line.atEnd()){
                qDebug() << "Preprocessor::readFile: unexpected end of line\n";
                return result;
            }
            inputparameters[index] = tmp;
        }
        QVector < qreal > otherparameters;
        while(true){
            if(line.atEnd()){
                break;
            }
            qreal tmp;
            line >> tmp;

            otherparameters.push_back(tmp);
        }
        result.push_back(QPair< QVector < qreal >, QVector < qreal > > (inputparameters, otherparameters));
    }
    return result;
}

bool Preprocessor::writeFile(const QString& filename, qvpvrvr& data, quint32 numberofinputs){
    QFile outputfile(filename);
    if(!outputfile.open(QIODevice::ReadWrite | QIODevice::Text)){
        return false;
    }
    QTextStream outputstream(&outputfile);
    outputstream << numberofinputs << '\n';

    for(auto it = data.begin();it != data.end();++it){
        for(quint32 index = 0;index < numberofinputs;++index){
            outputstream << it->first[index] << ' ';
        }
        for(auto itsecond = it->second.begin(); itsecond !=it->second.end();++itsecond ){
            outputstream << *itsecond << ' ';
        }
        outputstream << '\n';
    }

    return true;
}
