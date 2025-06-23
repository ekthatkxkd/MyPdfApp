#include "include/pdfExporter/PdfExporter.h"
#include "include/model/listmodel/TableModel.h"
#include <QPainter>
#include <QFont>
#include <QVariant>
#include <QResource>
#include <QPixmap>

#include "include/drawingtemplate/TemplateFactory.h"
#include "include/pdfExporter/renderTarget/RenderTargetFactory.h"
#include "include/pdfExporter/renderTarget/DocumentRenderer.h"
#include <QDebug>

PreviewImageProvider::PreviewImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {

    std::shared_ptr<QImage> defaultPage = std::make_shared<QImage>(PREVIEW_PIXEL_WIDTH,
                                                                   PREVIEW_PIXEL_HEIGHT,
                                                                   QImage::Format_ARGB32);
    defaultPage->fill(Qt::white);
    m_previewPages.append(defaultPage);
}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (id.startsWith("page_")) {
        bool ok;
        int pageIndex = id.mid(5).toInt(&ok);
        if (ok && pageIndex >= 0 && pageIndex < m_previewPages.size()) {
            if (size) {
                *size = m_previewPages[pageIndex]->size();
            }

            // if (requestedSize.isValid()) {
            //     return m_previewPages[pageIndex].scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // }

            // mm to pixel with dpi
            return m_previewPages[pageIndex]->scaled(QSize((210*97)/25.4, (297*97)/25.4), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }

    QImage emptyPage(PREVIEW_PIXEL_WIDTH,
                     PREVIEW_PIXEL_HEIGHT,
                     QImage::Format_ARGB32);
    emptyPage.fill(Qt::white);

    return emptyPage;
}

void PreviewImageProvider::updatePreviewImages(QList<std::shared_ptr<QImage>> &images) {
    m_previewPages = images;
}

//////// START PdfExporter definition.
///
PdfExporter::PdfExporter(PreviewImageProvider* provider, QObject *parent)
    : QObject(parent), m_imageProvider(provider) {
    initPageSettings();
}

PdfExporter::~PdfExporter() {
}

bool PdfExporter::hasPreview() const {
    return m_hasPreview;
}
int PdfExporter::pageCount() const {
    return m_pageCount;
}

void PdfExporter::initDefaultPdf(const QString &filePath) {
    pdfWriter = std::make_shared<QPdfWriter>(filePath);
    pdfWriter->setPageSize(QPageSize(QPageSize::A4));
    pdfWriter->setPageMargins(pageSettings.mmMargins, QPageLayout::Millimeter);
    pdfWriter->setResolution(pageSettings.resolution);
}

void PdfExporter::initDefaultImage() {
    // PDF와 동일한 크기의 QImage 설정.
    imagePtr = std::make_shared<QImage>(pageSettings.pxImageSize, QImage::Format_ARGB32);
    imagePtr->fill(Qt::white);

    // DPI 정보 설정 (QPainter 스케일링 정확성을 위해)
    double dotsPerMeter = pageSettings.resolution * 39.3701; // 1인치 = 39.3701 dots/meter
    imagePtr->setDotsPerMeterX(qRound(dotsPerMeter));
    imagePtr->setDotsPerMeterY(qRound(dotsPerMeter));
}

void PdfExporter::setFont(QPainter &painter, int fontSize, bool isBold) {
    QFont font = painter.font();

    font.setPointSize(fontSize);
    font.setBold(isBold);

    painter.setFont(font);
    painter.setPen(QPen(Qt::black, 2));
}

QHash<QString, QQuickItem*> PdfExporter::getChildItems(QQuickItem *rootItem, const QList<QString> childObjNames, const bool &isSearchAllDepth) {
    QHash<QString, QQuickItem*> childItems;

    if (rootItem != nullptr) {
        for (QString childObjName : childObjNames) {
            childItems[childObjName] = getChildItem(rootItem, childObjName, isSearchAllDepth);
        }
    }

    return childItems;
}

QQuickItem* PdfExporter::getChildItem(QQuickItem *rootItem, const QString &childObjName, const bool &isSearchAllDepth) {
    QQuickItem *retChildItem = nullptr;

    if (rootItem != nullptr) {
        const QList<QQuickItem*> childItems = rootItem->childItems();

        for (QQuickItem *childItem : childItems) {
            const QString objectName = childItem->objectName();

            if (objectName == childObjName) {
                retChildItem = childItem;
                break;
            } else if (isSearchAllDepth) {
                // Search all depths within root.
                retChildItem = getChildItem(childItem, childObjName, isSearchAllDepth);

                if (retChildItem != nullptr) {
                    break;
                }
            }
        }
    }

    return retChildItem;
}

void PdfExporter::initTableCellSizes(QPainter &painter, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights, const QVector<QVector<CellData>> &cellDatas,
                                     const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio) {
    //////// cell widths init
    ///
    ///
    qreal remainingWidthValue = pxTableFullWidthSize;
    int maxWidth = 0;
    int maxWidthIndex = 0;
    for (int index = 0; index < pxCellWidths.size(); index++) {
        qreal calculatedWidth = tableWidthRatio[index] * pxTableFullWidthSize;
        pxCellWidths[index] = calculatedWidth;
        remainingWidthValue -= calculatedWidth;

        if (calculatedWidth > maxWidth) {
            maxWidth = calculatedWidth;
            maxWidthIndex = index;
        }
    }

    if (remainingWidthValue > 0) {
        pxCellWidths[maxWidthIndex] += remainingWidthValue;
    }
    ///
    ///
    ////////

    //////// inner cell height init
    ///
    ///
    for (int rowIndex = 0; rowIndex < cellDatas.size(); rowIndex++) {
        std::vector<std::pair<int, qreal>> verticalTextCells;

        for (int colIndex = 0; colIndex < cellDatas[rowIndex].size(); colIndex++) {
            QSizeF textAreaSize;
            const auto &cellData = cellDatas[rowIndex][colIndex];

            QString cellText = cellData.cellText;

            if (cellData.isVerticalDir) {
                if (!cellText.contains("\n")) {
                    cellText = cellText.split("").join("\n");  // qml 내에서 개행문자 추가 안되어 있을 경우 대비.
                }

                textAreaSize = getCalculatedCellTextArea(cellText, painter.font(), cellData.isVerticalDir);

                verticalTextCells.push_back(std::make_pair(colIndex, textAreaSize.height()));
            } else {
                qreal actualCellWidth = 0;
                for (int widthIndex = cellData.startCol; widthIndex < (cellData.startCol + cellData.colSpan); widthIndex++) {
                    actualCellWidth += pxCellWidths[widthIndex];
                }

                textAreaSize = getCalculatedCellTextArea(cellText, painter.font(), cellData.isVerticalDir, QTextOption::WrapAnywhere, (actualCellWidth - (2 * cellTextMargins)));
                if ((textAreaSize.height() + (2 * cellTextMargins)) > pxCellHeights[rowIndex]) {
                    pxCellHeights[rowIndex] = textAreaSize.height() + (2 * cellTextMargins);
                }
            }
        }

        for (int verticalCellIndex = 0; verticalCellIndex < verticalTextCells.size(); verticalCellIndex++) {
            const auto &cellData = cellDatas[rowIndex][verticalTextCells[verticalCellIndex].first];

            qreal cellHeight = verticalTextCells[verticalCellIndex].second + (2 * cellTextMargins);

            int cellRowSpan = cellData.rowSpan;


            qreal remainingCellHeight = cellHeight - pxCellHeights[rowIndex];

            for (int cellStartRow = (cellData.startRow+1); cellStartRow < (cellData.startRow + cellRowSpan); cellStartRow++) {
                qreal dividedHeight = remainingCellHeight / ((cellData.startRow + cellRowSpan) - cellStartRow);

                if (dividedHeight > pxCellHeights[cellStartRow]) {
                    pxCellHeights[cellStartRow] = dividedHeight;
                    remainingCellHeight -= dividedHeight;
                } else {
                    remainingCellHeight -= pxCellHeights[cellStartRow];
                }
            }
        }
    }
    ///
    ///
    ////////
}

QSizeF PdfExporter::getCalculatedCellTextArea(const QString &text, const QFont &font, const bool isVertical, const QTextOption::WrapMode &wrapMode, qreal fixedWidth) {
    qreal totalHeight = 0;

    if (isVertical) {
        int explicitLineCount = text.count('\n') + 1;
        QFontMetrics fm(font);
        totalHeight = explicitLineCount * fm.height();
        fixedWidth = fm.horizontalAdvance("가");
    } else {
        QTextLayout textLayout(text, font);
        QTextOption option;
        option.setWrapMode(wrapMode);
        textLayout.setTextOption(option);

        textLayout.beginLayout();
        QList<QTextLine> lines;

        while(true) {
            QTextLine line = textLayout.createLine();
            if (!line.isValid()) break;

            line.setLineWidth(fixedWidth);
            totalHeight += line.height();
        }

        textLayout.endLayout();
    }

    return QSizeF(fixedWidth, totalHeight);
}

QRectF PdfExporter::drawTemplateTitle(QPainter &painter, QQuickItem *textItem) {
    QString titleText = textItem->property("text").toString();

    setFont(painter, 20, true);

    QFontMetrics metrics(painter.font());
    QRectF boundingBox(QPointF(0, 0), QSizeF(pageSettings.pxContentSize.width(), metrics.height()));

    Qt::Alignment align = Qt::AlignHCenter;

    painter.drawText(boundingBox, align, titleText);

    return boundingBox;
}

QRectF PdfExporter::drawTemplateTable(QPainter &painter, QQuickItem *tableItem, QPointF &cursorPoint, const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio, const bool &isPdf) {
    //////// [LLDDSS] ORIGIN IS TO ALIVE
    ///
    ///
    // QRectF tableArea(QPointF(0, 0), QSizeF(0, 0));

    // const int titleTableMargin = 5;

    // QPointF tableCursorPoint = cursorPoint;

    // QString tableTitle = tableItem->property("tableTextValue").toString();

    // QVector<QVector<CellData>> headerDatas = getCellDatas(tableItem, "headerRowRep");
    // QVector<QVector<CellData>> innerDatas = getCellDatas(tableItem, "innerRowRep");
    // QVector<QVector<CellData>> footerDatas = getCellDatas(tableItem, "footerRowRep");

    // int tableColCount = tableItem->property("dividedColCount").toInt();
    // int innerTableColCount = innerDatas.size();

    // std::vector<double> pxCellWidths(tableColCount, 0);
    // std::vector<double> pxCellHeights(innerTableColCount, 0);


    // //////// innerTableCell height 값 초기화
    // ///
    // ///
    // initTableCellSizes(painter, pxCellWidths, pxCellHeights, innerDatas, pxTableFullWidthSize, tableWidthRatio);
    // ///
    // ///
    // ////////

    // double pxSumTableWidths = 0;
    // for (const auto &pxCellWidth : pxCellWidths) {
    //     pxSumTableWidths += pxCellWidth;
    // }

    // //////// table title 그리기
    // ///
    // ///
    // if (tableTitle != "") {
    //     setFont(painter, 10, true);

    //     QFontMetrics metrics(painter.font());
    //     QRectF boundingBox(tableCursorPoint, QSizeF(pxTableFullWidthSize, metrics.height()));

    //     Qt::AlignmentFlag align = Qt::AlignLeft;

    //     painter.drawText(boundingBox, align, tableTitle);

    //     tableCursorPoint = QPointF(cursorPoint.x(), tableCursorPoint.y() + boundingBox.height() + titleTableMargin);
    // }
    // ///
    // ///
    // ////////


    // //////// header list 그리기
    // ///
    // ///
    // // header list 는 오직 하나의 열만 가지고 있고 text 는 오직 한 줄인 것으로 가정.
    // if (headerDatas.size() != 0) {
    //     setFont(painter, 8, true);

    //     QPointF headerCursorPoint = tableCursorPoint;

    //     std::vector<double> pxHeaderCellWidths(tableColCount, 0);
    //     std::vector<double> pxHeaderCellHeights(1, 0);

    //     initTableCellSizes(painter, pxHeaderCellWidths, pxHeaderCellHeights, headerDatas,
    //                        pxTableFullWidthSize, tableWidthRatio);

    //     for (const auto &headerData : headerDatas) {
    //         for (int colIndex = 0; colIndex < headerData.size(); colIndex++) {
    //             QString cellText = headerData[colIndex].cellText;
    //             int cellStartCol = headerData[colIndex].startCol;
    //             int cellColSpan = headerData[colIndex].colSpan;
    //             QColor cellBgColor = headerData[colIndex].bgColor;
    //             QString cellAlign = headerData[colIndex].alignPosition;

    //             qreal actualWidth = 0;

    //             for (int widthIndex = cellStartCol; widthIndex < (cellStartCol + cellColSpan); widthIndex++) {
    //                 actualWidth += pxCellWidths[widthIndex];
    //             }

    //             QRectF boundingBox(headerCursorPoint, QSizeF(actualWidth, pxHeaderCellHeights[0]));

    //             painter.setPen(QPen(Qt::black, 1.0));
    //             painter.fillRect(boundingBox,
    //                              QColor(cellBgColor));
    //             painter.drawRect(boundingBox);

    //             QTextOption textOption;
    //             Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;

    //             if (cellAlign == "center") {
    //                 align |= Qt::AlignmentFlag::AlignHCenter;
    //             } else if (cellAlign == "right") {
    //                 align |= Qt::AlignmentFlag::AlignRight;
    //             } else {
    //                 align |= Qt::AlignmentFlag::AlignLeft;
    //             }

    //             textOption.setAlignment(align);
    //             textOption.setWrapMode(QTextOption::WrapAnywhere);

    //             QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

    //             painter.drawText(adjustedTextRect, cellText, textOption);

    //             headerCursorPoint = QPointF(headerCursorPoint.x() + actualWidth,
    //                                         headerCursorPoint.y());
    //         }
    //     }

    //     tableCursorPoint = QPointF(tableCursorPoint.x(),
    //                                tableCursorPoint.y() + pxHeaderCellHeights[0]);
    // }
    // ///
    // ///
    // ////////


    // //////// innerTable 그리기
    // ///
    // ///
    // if (innerDatas.size() != 0) {
    //     QPointF innerCursorPoint = tableCursorPoint;

    //     qreal cellRectYPos = innerCursorPoint.y();

    //     for (int rowIndex = 0; rowIndex < innerDatas.size(); rowIndex++) {

    //         if (innerCursorPoint.y() + pxCellHeights[rowIndex] > pageSettings.pxContentSize.height()) {
    //             // pdfWriter.newPage();
    //             createNewPage(painter, isPdf);

    //             cursorPoint = QPointF(0, 0);
    //             tableCursorPoint = cursorPoint;
    //             innerCursorPoint = tableCursorPoint;
    //             cellRectYPos = 0;
    //         }

    //         for (int colIndex = 0; colIndex < innerDatas[rowIndex].size(); colIndex++) {
    //             QString cellText = innerDatas[rowIndex][colIndex].cellText;

    //             int cellStartCol = innerDatas[rowIndex][colIndex].startCol;
    //             int cellColSpan = innerDatas[rowIndex][colIndex].colSpan;
    //             int cellStartRow = innerDatas[rowIndex][colIndex].startRow;
    //             int cellRowSpan = innerDatas[rowIndex][colIndex].rowSpan;

    //             qreal cellRectXPos = tableCursorPoint.x();

    //             for (int widthIndex = 0; widthIndex < cellStartCol; widthIndex++) {
    //                 cellRectXPos += pxCellWidths[widthIndex];
    //             }

    //             qreal cellRectWidth = 0;
    //             qreal cellRectHeight = 0;

    //             for (int widthIndex = cellStartCol; widthIndex < (cellStartCol + cellColSpan); widthIndex++) {
    //                 cellRectWidth += pxCellWidths[widthIndex];
    //             }

    //             for (int heightIndex = cellStartRow; heightIndex < (cellStartRow + cellRowSpan); heightIndex++) {
    //                 cellRectHeight += pxCellHeights[heightIndex];
    //             }

    //             QColor cellRectBgColor = innerDatas[rowIndex][colIndex].bgColor;

    //             QRectF boundingBox(QPointF(cellRectXPos, cellRectYPos),
    //                                QSizeF(cellRectWidth, cellRectHeight));

    //             painter.setPen(QPen(Qt::black, 1.0));
    //             painter.fillRect(boundingBox,
    //                              QColor(cellRectBgColor));
    //             painter.drawRect(boundingBox);

    //             QString cellTextAlign = innerDatas[rowIndex][colIndex].alignPosition;
    //             bool cellTextBold = innerDatas[rowIndex][colIndex].isBold;

    //             setFont(painter, 8, cellTextBold);

    //             QTextOption textOption;
    //             Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;

    //             if (cellTextAlign == "center") {
    //                 align |= Qt::AlignmentFlag::AlignHCenter;
    //             } else if (cellTextAlign == "right") {
    //                 align |= Qt::AlignmentFlag::AlignRight;
    //             } else {
    //                 align |= Qt::AlignmentFlag::AlignLeft;
    //             }

    //             textOption.setAlignment(align);
    //             textOption.setWrapMode(QTextOption::WrapAnywhere);

    //             QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

    //             painter.drawText(adjustedTextRect, cellText, textOption);
    //         }

    //         innerCursorPoint = QPointF(innerCursorPoint.x(),
    //                                    innerCursorPoint.y() + pxCellHeights[rowIndex]);

    //         cellRectYPos += pxCellHeights[rowIndex];
    //     }

    //     tableCursorPoint = QPointF(tableCursorPoint.x(),
    //                                innerCursorPoint.y());
    // }
    // ///
    // ///
    // ////////

    // //////// footer 그리기
    // ///
    // ///
    // if (footerDatas.size() != 0) {
    //     QPointF innerCursorPoint = tableCursorPoint;

    //     qreal cellRectYPos = innerCursorPoint.y();

    //     for (int rowIndex = 0; rowIndex < footerDatas.size(); rowIndex++) {

    //         if (innerCursorPoint.y() + pxCellHeights[rowIndex] > pageSettings.pxContentSize.height()) {
    //             // pdfWriter.newPage();
    //             createNewPage(painter, isPdf);

    //             cursorPoint = QPointF(0, 0);
    //             tableCursorPoint = cursorPoint;
    //             innerCursorPoint = tableCursorPoint;
    //             cellRectYPos = 0;
    //         }

    //         for (int colIndex = 0; colIndex < footerDatas[rowIndex].size(); colIndex++) {
    //             QString cellText = footerDatas[rowIndex][colIndex].cellText;

    //             int cellStartCol = footerDatas[rowIndex][colIndex].startCol;
    //             int cellColSpan = footerDatas[rowIndex][colIndex].colSpan;
    //             int cellStartRow = footerDatas[rowIndex][colIndex].startRow;
    //             int cellRowSpan = footerDatas[rowIndex][colIndex].rowSpan;

    //             qreal cellRectXPos = tableCursorPoint.x();

    //             for (int widthIndex = 0; widthIndex < cellStartCol; widthIndex++) {
    //                 cellRectXPos += pxCellWidths[widthIndex];
    //             }

    //             qreal cellRectWidth = 0;
    //             qreal cellRectHeight = 0;

    //             for (int widthIndex = cellStartCol; widthIndex < (cellStartCol + cellColSpan); widthIndex++) {
    //                 cellRectWidth += pxCellWidths[widthIndex];
    //             }

    //             for (int heightIndex = cellStartRow; heightIndex < (cellStartRow + cellRowSpan); heightIndex++) {
    //                 cellRectHeight += pxCellHeights[heightIndex];
    //             }

    //             QColor cellRectBgColor = footerDatas[rowIndex][colIndex].bgColor;

    //             QRectF boundingBox(QPointF(cellRectXPos, cellRectYPos),
    //                                QSizeF(cellRectWidth, cellRectHeight));

    //             painter.setPen(QPen(Qt::black, 1.0));
    //             painter.fillRect(boundingBox,
    //                              QColor(cellRectBgColor));
    //             painter.drawRect(boundingBox);

    //             QString cellTextAlign = footerDatas[rowIndex][colIndex].alignPosition;
    //             bool cellTextBold = footerDatas[rowIndex][colIndex].isBold;

    //             setFont(painter, 8, cellTextBold);

    //             QTextOption textOption;
    //             Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;

    //             if (cellTextAlign == "center") {
    //                 align |= Qt::AlignmentFlag::AlignHCenter;
    //             } else if (cellTextAlign == "right") {
    //                 align |= Qt::AlignmentFlag::AlignRight;
    //             } else {
    //                 align |= Qt::AlignmentFlag::AlignLeft;
    //             }

    //             textOption.setAlignment(align);
    //             textOption.setWrapMode(QTextOption::WrapAnywhere);

    //             QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

    //             painter.drawText(adjustedTextRect, cellText, textOption);
    //         }

    //         innerCursorPoint = QPointF(innerCursorPoint.x(),
    //                                    innerCursorPoint.y() + pxCellHeights[rowIndex]);

    //         cellRectYPos += pxCellHeights[rowIndex];
    //     }

    //     tableCursorPoint = QPointF(tableCursorPoint.x(),
    //                                innerCursorPoint.y());
    // }
    // ///
    // ///
    // ////////

    // tableArea = QRectF(cursorPoint, QSizeF(pxSumTableWidths, tableCursorPoint.y() - cursorPoint.y()));

    // return tableArea;
    ///
    ///
    ////////

    return QRectF();
}

QRectF PdfExporter::drawTemplateImage(QPainter &painter, QQuickItem *imageItem, QPointF &cursorPoint, const bool &isPdf, const int settingPxImageWidth, const bool isHCenter) {
    QPointF imgCursorPoint = cursorPoint;

    QPixmap scaledPixmap(0, 0);

    QString qrcPath = imageItem->property("source").toString();

    if (qrcPath.startsWith("qrc:")) {
        qrcPath = qrcPath.mid(3);
    }

    QResource resource(qrcPath);

    if (!resource.isValid() || resource.size() == 0) {
        qDebug() << "drawTemplateImage, could not found qrc resource...";

        return QRectF(QPointF(0, 0), QSizeF(0, 0));
    }

    QPixmap originPixmap(qrcPath);
    if (!originPixmap.loadFromData(reinterpret_cast<const uchar*>(resource.data()), resource.size())) {
        qDebug() << "drawTemplateImage, could not load image data to QPixmap...";
        return QRectF(QPointF(0, 0), QSizeF(0, 0));
    }

    if (!originPixmap.isNull()) {
        if (settingPxImageWidth > 0) {
            int targetImgWidth = settingPxImageWidth;

            double aspectRatio = static_cast<double>(originPixmap.height()) / originPixmap.width();
            int targetImgHeight = static_cast<int>(targetImgWidth * aspectRatio);

            if (targetImgWidth > pageSettings.pxContentSize.width() || targetImgHeight > pageSettings.pxContentSize.height()) {
                scaledPixmap = originPixmap.scaled(pageSettings.pxContentSize.width(), pageSettings.pxContentSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                scaledPixmap = originPixmap.scaled(targetImgWidth, targetImgHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        } else {
            scaledPixmap = originPixmap.scaled(pageSettings.pxContentSize.width(), pageSettings.pxContentSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        // newPage
        if ((imgCursorPoint.y() + scaledPixmap.height()) > pageSettings.pxContentSize.height()) {
            createNewPage(painter, isPdf);

            cursorPoint = QPointF(0, 0);
            imgCursorPoint = cursorPoint;
        }

        if (isHCenter) {
            imgCursorPoint = QPointF((pageSettings.pxContentSize.width() - scaledPixmap.width()) / 2,
                                     imgCursorPoint.y());
        }

        painter.drawPixmap(imgCursorPoint, scaledPixmap);
    } else {
        qDebug() << "drawTemplateImage, loaded QPixmap is null...";
        return QRectF(QPointF(0, 0), QSizeF(0, 0));
    }

    return QRectF(imgCursorPoint, QSizeF(scaledPixmap.width(), scaledPixmap.height()));
}

void PdfExporter::drawMaterialTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(materialObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, materialObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawMaterialTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[materialObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[materialObjNames[0]]);
    }
    ///
    ///
    ////////


    //////// informTable 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);

    if (templateQuickItems[materialObjNames[1]] != nullptr) {
        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();
        std::vector<qreal> tableWidthRatio{0.15, 0.3, 0.05, 0.1, 0.4};

        childItemRect[1] = drawTemplateTable(painter, templateQuickItems[materialObjNames[1]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////

    //////// 자재구매 내역 리스트 그리기
    ///
    ///
    setFont(painter);
    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[1].height() + templateItemSpacing);

    if (templateQuickItems[materialObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();
        std::vector<qreal> tableWidthRatio{0.1, 0.6, 0.1, 0.2};

        childItemRect[2] = drawTemplateTable(painter, templateQuickItems[materialObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////
}

void PdfExporter::defectReportTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(defectReportObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, defectReportObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawMaterialTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[defectReportObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[defectReportObjNames[0]]);
    }
    ///
    ///
    ////////

    //////// 상단의 문서 번호 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);
    if (templateQuickItems[defectReportObjNames[1]] != nullptr) {
        qreal pxTableFullWidthSize = (pageSettings.pxContentSize.width() - marginBetweenTables) / 2;

        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[1] = drawTemplateTable(painter, templateQuickItems[defectReportObjNames[1]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////

    //////// 상단의 장소 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(childItemRect[1].x() + childItemRect[1].width() + marginBetweenTables, childItemRect[1].y());
    if (templateQuickItems[defectReportObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = (pageSettings.pxContentSize.width() - marginBetweenTables) / 2;

        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[2] = drawTemplateTable(painter, templateQuickItems[defectReportObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////

    //////// 품목 리스트 그리기
    ///
    ///
    setFont(painter);

    if (templateQuickItems[defectReportObjNames[3]] != nullptr) {
        int maxUpperTableHeight = (childItemRect[1].height() > childItemRect[2].height()) ? childItemRect[1].height() : childItemRect[2].height();

        cursorPoint = QPointF(0, childItemRect[1].y() + maxUpperTableHeight + templateItemSpacing);

        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();

        std::vector<qreal> tableWidthRatio{0.05, 0.25 ,0.36, 0.12, 0.12, 0.1};

        childItemRect[3] = drawTemplateTable(painter, templateQuickItems[defectReportObjNames[3]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////

    //////// 총 수량 테이블
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[3].y() + childItemRect[3].height() + templateItemSpacing);
    if (templateQuickItems[defectReportObjNames[4]] != nullptr) {
        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();
        std::vector<qreal> tableWidthRatio{0.1, 0.25, 0.1, 0.25, 0.1, 0.2};

        childItemRect[4] = drawTemplateTable(painter, templateQuickItems[defectReportObjNames[4]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }

    ///
    ///
    ////////
}

void PdfExporter::orderFormTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(orderFromObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, orderFromObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawMaterialTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[orderFromObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[orderFromObjNames[0]]);
    }
    ///
    ///
    ////////

    //////// 발주일자 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[1]] != nullptr) {
        QQuickItem * orderDateItem = templateQuickItems[orderFromObjNames[1]];

        QString orderDateText = orderDateItem->property("orderDateItemText").toString();

        QFontMetrics metrics(painter.font());
        QRectF boundingBox(QPointF(pageSettings.pxContentSize.width() - metrics.horizontalAdvance(orderDateText), cursorPoint.y()),
                           QSizeF(metrics.horizontalAdvance(orderDateText), metrics.height()));

        Qt::Alignment align = Qt::AlignVCenter;

        painter.drawText(boundingBox, align, orderDateText);

        childItemRect[1] = boundingBox;
    }
    ///
    ///
    ////////

    //////// 수신자 정보 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[0].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();
        std::vector<qreal> tableWidthRatio{0.2, 0.3, 0.2, 0.3};

        childItemRect[2] = drawTemplateTable(painter, templateQuickItems[orderFromObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////

    //////// 발신자 정보 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[2].y() + childItemRect[2].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[3]] != nullptr) {
        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();
        std::vector<qreal> tableWidthRatio{0.2, 0.3, 0.2, 0.3};

        childItemRect[3] = drawTemplateTable(painter, templateQuickItems[orderFromObjNames[3]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }

    ///
    ///
    ////////

    //////// 발주 내용 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[3].y() + childItemRect[3].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[4]] != nullptr) {
        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();
        std::vector<qreal> tableWidthRatio{0.05, 0.1, 0.15, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

        childItemRect[4] = drawTemplateTable(painter, templateQuickItems[orderFromObjNames[4]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////
}

void PdfExporter::drawReceiptVoucherTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(receiptVouchrObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, receiptVouchrObjNames, true);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawReceiptVoucherTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[receiptVouchrObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[receiptVouchrObjNames[0]]);
    }
    ///
    ///
    ////////

    //////// 전표 번호 table 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);

    if (templateQuickItems[receiptVouchrObjNames[1]] != nullptr) {
        qreal pxTableFullWidthSize = (pageSettings.pxContentSize.width() - marginBetweenTables) / 2;

        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[1] = drawTemplateTable(painter, templateQuickItems[receiptVouchrObjNames[1]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////

    //////// 출입고처 table 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(childItemRect[1].x() + childItemRect[1].width() + marginBetweenTables, childItemRect[1].y());

    if (templateQuickItems[receiptVouchrObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = (pageSettings.pxContentSize.width() - marginBetweenTables) / 2;
        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[2] = drawTemplateTable(painter, templateQuickItems[receiptVouchrObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }
    ///
    ///
    ////////

    //////// 제품 리스트 테이블 그리기
    ///
    ///
    {
        setFont(painter);

        int maxUpperTableHeight = (childItemRect[1].height() > childItemRect[2].height()) ? childItemRect[1].height() : childItemRect[2].height();
        cursorPoint = QPointF(0, childItemRect[1].y() + maxUpperTableHeight + templateItemSpacing);

        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();

        std::vector<qreal> tableWidthRatio{0.05, 0.15, 0.5, 0.1, 0.2};

        // 생산 테이블
        if (templateQuickItems[receiptVouchrObjNames[3]] != nullptr) {
            childItemRect[3] = drawTemplateTable(painter, templateQuickItems[receiptVouchrObjNames[3]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
        }

        cursorPoint = QPointF(0, childItemRect[3].y() + childItemRect[3].height());
        // 소모 테이블
        if (templateQuickItems[receiptVouchrObjNames[4]] != nullptr) {
            childItemRect[4] = drawTemplateTable(painter, templateQuickItems[receiptVouchrObjNames[4]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
        }
    }
    ///
    ///
    ////////

    //////// 총 수량 테이블
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[4].y() + childItemRect[4].height() + templateItemSpacing);

    if (templateQuickItems[receiptVouchrObjNames[5]] != nullptr) {
        qreal pxTableFullWidthSize = pageSettings.pxContentSize.width();
        std::vector<qreal> tableWidthRatio{0.1, 0.25, 0.1, 0.25, 0.1, 0.2};

        childItemRect[5] = drawTemplateTable(painter, templateQuickItems[receiptVouchrObjNames[5]], cursorPoint, pxTableFullWidthSize, tableWidthRatio, isPdf);
    }

    ///
    ///
    ////////

    //////// 하단 이미지
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[5].y() + childItemRect[5].height() + templateItemSpacing);

    if (templateQuickItems[receiptVouchrObjNames[6]] != nullptr) {
        childItemRect[6] = drawTemplateImage(painter, templateQuickItems[receiptVouchrObjNames[6]], cursorPoint, isPdf, pageSettings.pxContentSize.width(), true);
    }
    ///
    ///
    ////////
}

bool PdfExporter::exportToPdf(QQuickItem *rootItem, const QString &filePath) {
    qDebug() << "exportToPdf, start";

    ///*
    //////// [LLDDSS] MY MODIFIED
    ///
    ///
    // template 생성. - element 들을 그리는 기능들을 함.
    auto materialDocTemplate = TemplateFactory::createTemplate(TemplateFactory::MATERIAL);

    // PDF or 이미지 렌더 타겟 생성. - pdfwriter, qimage, painter 객체 생성.
    auto pdfTarget = RenderTargetFactory::createPdfTarget(filePath, QSizeF(595, 842));

    // 렌더러 생성. - 렌더 타겟 내의 painter property 를 template 에 전달과 동시에 element 그리기 요청.
    DocumentRenderer renderer(std::move(pdfTarget));
    // 렌더링 실행.
    renderer.renderTemplate(std::move(materialDocTemplate));

    return true;
    ///
    ///
    ////////
    //*/

    /*
    //////// [LLDDSS] ORIGIN IS TO ALIVE
    ///
    ///
    initDefaultPdf(filePath);

    QPainter painter;

    if (!painter.begin(pdfWriter.get())) {
        qDebug() << "exportToPdf, Failed to initialize QPainter...";
        return false;
    }

    setFont(painter);

    if (rootItem->objectName() == templateObjNames[0]) {
        drawMaterialTemplate(painter, rootItem, true);
    } else if (rootItem->objectName() == templateObjNames[1]) {
        defectReportTemplate(painter, rootItem, true);
    } else if (rootItem->objectName() == templateObjNames[2]) {
        orderFormTemplate(painter, rootItem, true);
    } else if (rootItem->objectName() == templateObjNames[3]) {
        drawReceiptVoucherTemplate(painter, rootItem, true);
    }

    painter.end();

    return true;
    ///
    ///
    ////////
    //*/
}

bool PdfExporter::generatePreview(QQuickItem *rootItem) {
    qDebug() << "generatePreview, start";

    m_hasPreview = false;
    emit hasPreviewChanged();

    m_pageCount = 0;
    previewPages.clear();

    initDefaultImage();
    previewPages.append(imagePtr);

    QPainter painter;

    if (!painter.begin(imagePtr.get())) {
        qDebug() << "generatePreview, Failed to initialize QPainter...";
        return false;
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(pageSettings.pxMargins, pageSettings.pxMargins);

    setFont(painter);

    if (rootItem->objectName() == templateObjNames[0]) {
        drawMaterialTemplate(painter, rootItem, false);
    } else if (rootItem->objectName() == templateObjNames[1]) {
        defectReportTemplate(painter, rootItem, false);
    } else if (rootItem->objectName() == templateObjNames[2]) {
        orderFormTemplate(painter, rootItem, false);
    } else if (rootItem->objectName() == templateObjNames[3]) {
        drawReceiptVoucherTemplate(painter, rootItem, false);
    }

    painter.end();

    m_pageCount = previewPages.size();
    emit pageCountChanged();

    m_imageProvider->updatePreviewImages(previewPages);

    m_hasPreview = true;
    emit hasPreviewChanged();
    emit previewUpdated();

    return true;
}

//////// [LLDDSS] ORIGIN IS TO ALIVE
///
///
// QVector<QVector<CellData>> PdfExporter::getCellDatas(QQuickItem *tableItem, const QString &repeaterObjName) {
//     QVector<QVector<CellData>> m_processedTableData;

//     QQuickItem * innerRepQuickItem = getChildItem(tableItem, repeaterObjName, true);

//     if (innerRepQuickItem) {
//         QVariant modelProperty = innerRepQuickItem->property("model");
//         QObject* modelObject = modelProperty.value<QObject*>();

//         if (modelObject) {
//             TableModel* tableModel = qobject_cast<TableModel*>(modelObject);

//             if (tableModel) {
//                 QVariantList allData = tableModel->getAllTableData();

//                 for (int i = 0; i < allData.size(); ++i) {
//                     QVariantList rowData = allData[i].toList();
//                     QVector<CellData> processedRow;

//                     for (int j = 0; j < rowData.size(); ++j) {
//                         QVariantMap cellMap = rowData[j].toMap();
//                         CellData cellData = CellData::fromVariantMap(cellMap);

//                         // vertical text 일 경우 개행문자 추가.
//                         if (cellData.isVerticalDir && !cellData.cellText.contains("\n")) {
//                             cellData.cellText = cellData.cellText.split("").join("\n");

//                             // 앞뒤 개행문자 제거
//                             if (cellData.cellText.startsWith("\n")) {
//                                 cellData.cellText = cellData.cellText.mid(1);
//                             }

//                             if (cellData.cellText.endsWith("\n")) {
//                                 cellData.cellText.chop(1);
//                             }
//                             //
//                         }
//                         //

//                         processedRow.append(cellData);
//                     }

//                     m_processedTableData.append(processedRow);
//                 }
//             }
//         }
//     }

//     return m_processedTableData;
// }
///
///
////////

void PdfExporter::createNewPage(QPainter &painter, const bool &isPdf) {
    if (isPdf) {
        if (pdfWriter != nullptr) {
            pdfWriter->newPage();
        }
    } else {
        QFont font = painter.font();

        painter.end();

        initDefaultImage();
        previewPages.append(imagePtr);

        painter.begin(imagePtr.get());

        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(pageSettings.pxMargins, pageSettings.pxMargins);

        //////// test
        // painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        // painter.drawRect(0, 0, pageSettings.pxContentSize.width(), pageSettings.pxContentSize.height());
        ////////

        painter.setFont(font);
    }
}

void PdfExporter::initPageSettings() {
    // A4 페이지 크기 설정
    QPageSize pageSize(QPageSize::A4);

    pageSettings.mmPageFullSize = pageSize.size(QPageSize::Millimeter);

    // 여백 설정 (QPdfWriter와 동일)
    pageSettings.mmMargins = QMarginsF(mmDefaultPageMargin, mmDefaultPageMargin, mmDefaultPageMargin, mmDefaultPageMargin);
    pageSettings.resolution = defaultDPI;

    // A4 SIZE 전체 크기 mm 을 pixel 단위로 변환.
    pageSettings.pxImageSize = QSize(
        qRound(pageSettings.mmPageFullSize.width() / 25.4 * pageSettings.resolution),
        qRound(pageSettings.mmPageFullSize.height() / 25.4 * pageSettings.resolution)
        );

    // 여백을 픽셀로 변환
    pageSettings.pxMargins = qRound(pageSettings.mmMargins.left() / 25.4 * pageSettings.resolution);

    // 내용 영역 크기 계산 (여백 제외)
    pageSettings.pxContentSize = QSize(
        pageSettings.pxImageSize.width() - (2 * pageSettings.pxMargins),
        pageSettings.pxImageSize.height() - (2 * pageSettings.pxMargins)
        );
}
