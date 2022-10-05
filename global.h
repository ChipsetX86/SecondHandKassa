#ifndef GLOBAL_H
#define GLOBAL_H

#include <QVector>
#include <QMap>

const static QStringList typeNames = {"Верхняя",  "Мелочь", "Женское", "Юбки",
                                      "Брюки", "Обувь", "Мужское", "Детское"};

//Женское
const QVector<ushort> priceWoman = {200, 300, 399, 450, 500, 550, 650, 800, 850, 1000, 1299, 1500,
                                   1899, 2499, 2999};
//Юбки
const QVector<ushort> priceSkirt = {300, 399, 450, 500, 550, 650, 850, 1000, 1299, 1499, 1899, 2500};
//Мужское
const QVector<ushort> priceMen = {200, 300, 399, 450, 500, 550, 650, 850, 1000, 1299,
                                  1500, 1799, 2000, 2300, 2500, 3000, 3500};
//Детское
const QVector<ushort> priceChildren = {50, 100, 200, 300, 399, 450, 500, 550,
                                       650, 850, 1000, 1299, 1500, 1799, 2000, 2499, 3000};
//Брюки
const QVector<ushort> priceTrousers = {200, 280, 300, 399, 400, 450, 500, 550, 600, 650, 750, 800, 850, 1000,
                                       1299, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000};
//Обувь
const QVector<ushort> priceShoes = {60, 100, 200, 300, 399, 450, 500, 550, 650, 850, 1000, 1299, 1500,
                                    2000, 2500, 3000, 3500, 4000, 4500, 5000};
//Верхняя одежда
const QVector<ushort> priceOuterwear = {300, 500, 750, 1000, 1500, 2000, 2300, 2500, 3000, 3500, 4000,
                                        4500, 5000, 6000, 7000, 8000, 9000, 10000};
//Мелочь
const QVector<ushort> priceTrifle = {5, 10, 20, 30, 40, 50, 60, 100,
                                     150, 199, 200, 250, 299, 300, 350, 399,
                                     400, 450, 650, 850, 1000, 1299, 1500, 2000, 2499,
                                     2899, 3499, 3899, 4499, 5000};

const QVector<ushort> priceNew = {10, 15, 20, 30, 40, 50, 60, 70, 80, 100, 120, 130, 140, 150, 180, 200, 250,
                                 300, 350, 400, 450, 500, 550, 650, 700, 750, 1000, 1350, 1500, 2000, 2500};

const QMap<QString, QVector<ushort>> typeNamesPrices =
            {{"Верхняя", priceOuterwear},
             {"Мелочь", priceTrifle},
             {"Женское", priceWoman},
             {"Юбки", priceSkirt},
             {"Брюки", priceTrousers},
             {"Обувь", priceShoes},
             {"Мужское", priceMen},
             {"Детское", priceChildren},
             {"Новое", priceNew},
            };

#endif // GLOBAL_H
