#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <array>
#include <QString>
#include <QVariantMap>

class Configuration {
public:
    Configuration(QString in, QString out, int bx, int by, int bz, int particle_num, int step_num,
                  double step_size, double perm_prob, double D0, double d,
                  double scale, double tstep, double vsize, int init_in, bool saveAll);

    Configuration(const QVariantMap &map);
    void fromVariantMap(const QVariantMap &map);
    void updateFromMap(const QVariantMap &map);
    QVariantMap toVariantMap() const;
    int bx,by,bz;
    QString in,out;
    int particle_num;
    int step_num;
    double step_size, perm_prob, D0, d,scale ,tstep ,vsize;
    int init_in;
    bool saveAll;

};

#endif // CONFIGURATION_H
