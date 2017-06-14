import QtQuick 2.7

GridView {
    // The standard size. height is always equal to this, but
    // the size changes to make stuff centered
    property int idealCellHeight
    property int idealCellWidth

    // The actual cell height is always as desired, but the cell width
    // is calculated from the current width of the view and how many cells fit
    cellHeight: idealCellHeight
    cellWidth: width / Math.min(count, Math.floor(width / idealCellWidth))
}

