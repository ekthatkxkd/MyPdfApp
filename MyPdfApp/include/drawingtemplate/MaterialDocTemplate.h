#ifndef MATERIALDOCTEMPLATE_H
#define MATERIALDOCTEMPLATE_H

#include "include/drawingtemplate/DocumentTemplate.h"
#include "include/drawingtemplate/dataProvider/SqliteDataProvider.h"

// 구체적인 템플릿 클래스들
class MaterialDocTemplate : public DocumentTemplate {
public:
    MaterialDocTemplate();

    virtual void setupTemplate(const QMap<QString, QList<QPair<QString, QStringList>>> &elementDatas) override;

private:
    void initInformTableData(const QList<QPair<QString, QStringList>> &dbDatas);
    void initHistoryTableData(const QList<QPair<QString, QStringList>> &dbDatas);

    const QString historyTableTitle = "▶자재구매 내역";

    std::shared_ptr<IDataProvider> dataProvider;

    QVector<qreal> informTableWidthRatio{0.15, 0.3, 0.05, 0.1, 0.4};
    QVector<QVector<CellData>> informTableInnerDatas {
        {
            CellData("",0,0,1,1,"구매번호",Qt::lightGray,true,12,"center",false),
            CellData("number",0,1,1,1,"",Qt::white,false,12,"left",false),
            CellData("",0,2,3,1,"구매처",Qt::lightGray,true,12,"center",true),
            CellData("",0,3,1,1,"상호",Qt::lightGray,true,12,"center",false),
            CellData("company",0,4,1,1,"",Qt::white,false,12,"left",false)
        },
        {
            CellData("",1,0,1,1,"구매창고",Qt::lightGray,true,12,"center",false),
            CellData("storage",1,1,1,1,"",Qt::white,false,12,"left",false),
            CellData("",1,3,1,1,"주소",Qt::lightGray,true,12,"center",false),
            CellData("address",1,4,1,1,"",Qt::white,false,12,"left",false)
        },
        {
            CellData("",2,0,1,1,"담당자",Qt::lightGray,true,12,"center",false),
            CellData("manager",2,1,1,1,"",Qt::white,false,12,"left",false),
            CellData("",2,3,1,1,"TEL",Qt::lightGray,true,12,"center",false),
            CellData("tel",2,4,1,1,"",Qt::white,false,12,"left",false)
        }
    };

    QVector<qreal> historyWidthRatio{0.1, 0.6, 0.1, 0.2};
    QVector<CellData> historyHeaderDatas {
        CellData("",0,0,1,1,"일자",Qt::lightGray,true,12,"center",false),
        CellData("",0,1,1,1,"품목명[규격명]",Qt::lightGray,true,12,"center",false),
        CellData("",0,2,1,1,"수량",Qt::lightGray,true,12,"center",false),
        CellData("",0,3,1,1,"적요",Qt::lightGray,true,12,"center",false)
    };
    QVector<CellData> defaultHistoryInnerDatas {
        CellData("date",0,0,1,1,"",Qt::white,false,12,"center",false),
        CellData("itemName",0,1,1,1,"",Qt::white,false,12,"left",false),
        CellData("quantity",0,2,1,1,"",Qt::white,false,12,"right",false),
        CellData("briefs",0,3,1,1,"",Qt::white,false,12,"right",false)
    };
    QVector<QVector<CellData>> historyInnerDatas{
    };
    QVector<CellData> historyFooterDatas {
        CellData("",0,0,1,2,"총합계",Qt::lightGray,true,12,"center",false),
        CellData("sum",0,2,1,1,"",Qt::lightGray,true,12,"center",false),
        CellData("",0,3,1,1,"",Qt::lightGray,true,12,"center",false),
    };
};

#endif // MATERIALDOCTEMPLATE_H
