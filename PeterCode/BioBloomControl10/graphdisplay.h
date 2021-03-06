#ifndef GRAPHDISPLAY_H
#define GRAPHDISPLAY_H

#include <QWidget>
#include <QComboBox>
#include "chart.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <qnetworkreply.h>


class GraphDisplay : public QWidget
{
    Q_OBJECT

public:
    GraphDisplay(QString graphType, QWidget *parent = 0);
    GraphDisplay(QString graphType, int ideal, QWidget *parent = 0);
    GraphDisplay(QString graphType, int ideal, QString potMac, QWidget *parent);

    ~GraphDisplay();
    void addPoint(QDateTime dataNumber, int dataValue);//int dataNumber
    void setIdeal(float ideal);
public slots:
    void graphDataFetchFinished(QNetworkReply* reply);

private:
    QComboBox *menu;
    Chart *chart;


};

#endif // GRAPHDISPLAY_H
