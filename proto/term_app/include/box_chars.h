#pragma once

/* ======================== */
/*      Double Line Style   */
/* ======================== */
/* Corners */
#define CORNER_TOPLEFT_DOUBLE     L'╔'
#define CORNER_TOPRIGHT_DOUBLE    L'╗'
#define CORNER_BOTTOMLEFT_DOUBLE  L'╚'
#define CORNER_BOTTOMRIGHT_DOUBLE L'╝'
/* Lines */
#define LINE_VERTICAL_DOUBLE      L'║'
#define LINE_HORIZONTAL_DOUBLE    L'═'
/* Junctions */
#define JUNCTION_LEFT_DOUBLE      L'╠'
#define JUNCTION_RIGHT_DOUBLE     L'╣'
#define JUNCTION_TOP_DOUBLE       L'╦'
#define JUNCTION_BOTTOM_DOUBLE    L'╩'
#define JUNCTION_CENTER_DOUBLE    L'╬'

/* ======================== */
/*      Single Line Style   */
/* ======================== */
/* Corners */
#define CORNER_TOPLEFT_SINGLE     L'┌'
#define CORNER_TOPRIGHT_SINGLE    L'┐'
#define CORNER_BOTTOMLEFT_SINGLE  L'└'
#define CORNER_BOTTOMRIGHT_SINGLE L'┘'
/* Lines */
#define LINE_VERTICAL_SINGLE      L'│'
#define LINE_HORIZONTAL_SINGLE    L'─'
/* Junctions */
#define JUNCTION_LEFT_SINGLE      L'├'
#define JUNCTION_RIGHT_SINGLE     L'┤'
#define JUNCTION_TOP_SINGLE       L'┬'
#define JUNCTION_BOTTOM_SINGLE    L'┴'
#define JUNCTION_CENTER_SINGLE    L'┼'

/* ======================== */
/*  Mixed Style Junctions   */
/* ======================== */
/* Double vertical + Single horizontal */
#define JUNCTION_LEFT_DOUBLE_SINGLE  L'╟'
#define JUNCTION_RIGHT_DOUBLE_SINGLE L'╢'
#define JUNCTION_CENTER_DOUBLE_VERT   L'╫'
/* Single vertical + Double horizontal */
#define JUNCTION_TOP_SINGLE_DOUBLE   L'╤'
#define JUNCTION_BOTTOM_SINGLE_DOUBLE L'╧'
#define JUNCTION_CENTER_SINGLE_VERT   L'╪'
