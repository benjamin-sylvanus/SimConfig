#include "configuration.h"

Configuration::Configuration(QString in, QString out, int bx,int by, int bz, int particle_num, int step_num,
                             double step_size, double perm_prob, double D0, double d,
                             double scale, double tstep, double vsize, int init_in, bool saveAll)
    : in(in), out(out),bx(bx),by(by),bz(bz), particle_num(particle_num), step_num(step_num),
    step_size(step_size), perm_prob(perm_prob), D0(D0), d(d),
    scale(scale), tstep(tstep), vsize(vsize), init_in(init_in), saveAll(saveAll) {

}


Configuration::Configuration(const QVariantMap &map) {
    in = map["in"].toString();
    out = map["out"].toString();
    bx = map["bx"].toInt();
    by = map["by"].toInt();
    bz = map["bz"].toInt();
    particle_num = map["particle_num"].toInt();
    step_num = map["step_num"].toInt();
    step_size = map["step_size"].toDouble();
    perm_prob = map["perm_prob"].toDouble();
    D0 = map["D0"].toDouble();
    d = map["d"].toDouble();
    scale = map["scale"].toDouble();
    tstep = map["tstep"].toDouble();
    vsize = map["vsize"].toDouble();
    init_in = map["init_in"].toInt();
    saveAll = map["saveAll"].toBool();
}

QVariantMap Configuration::toVariantMap() const {
    QVariantMap map;
    map["in"] = in;
    map["out"] = out;
    map["bx"] = bx;
    map["by"] = by;
    map["bz"] = bz;
    map["particle_num"] = particle_num;
    map["step_num"] = step_num;
    map["step_size"] = step_size;
    map["perm_prob"] = perm_prob;
    map["D0"] = D0;
    map["d"] = d;
    map["scale"] = scale;
    map["tstep"] = tstep;
    map["vsize"] = vsize;
    map["init_in"] = init_in;
    map["saveAll"] = saveAll;

    return map;
}

void Configuration::fromVariantMap(const QVariantMap& map) {
    in = map["in"].toString();
    out = map["out"].toString();
    bx = map["bx"].toInt();
    by = map["by"].toInt();
    bz = map["bz"].toInt();
    particle_num = map["particle_num"].toInt();
    step_num = map["step_num"].toInt();
    step_size = map["step_size"].toDouble();
    perm_prob = map["perm_prob"].toDouble();
    D0 = map["D0"].toDouble();
    d = map["d"].toDouble();
    scale = map["scale"].toDouble();
    tstep = map["tstep"].toDouble();
    vsize = map["vsize"].toDouble();
    init_in = map["init_in"].toInt();
    saveAll = map["saveAll"].toBool();
}

void Configuration::updateFromMap(const QVariantMap& map)
{
    QVariantMap currentMap = this->toVariantMap();

    // loop over map
    for (auto it = map.begin(); it != map.end(); it++ ) {
        currentMap[it.key()] = it.value();
    }

    fromVariantMap(currentMap);
}






