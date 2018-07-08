#ifndef MODEL_H
#define MODEL_H



#include <QAbstractTableModel>

class Model : public QAbstractTableModel
{
    Q_OBJECT
public:
   Model(QObject *parent);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MODEL_H
