/*
 * clrpids.h - resource and message IDs for the ColorPalette WPS classes.
 *
 * Rescued from the SOM 1.0 precompiler flow: the 1993 sample carried these
 * in a 'passthru: C.ph;' section of CLRPALET.CSC, from which the old
 * toolchain generated clrpalet.ph.  The modern SOM compiler (sc) emits no
 * .ph, so they live here instead and are included by the C sources and by
 * clrpalet.rc.
 */

#define ID_CLRPALET                 1
#define IDDLG_COLORPICKINGDIALOG    10
#define DID_COLORWHEEL              801
#define DID_UNDO                    802
#define DID_HELP                    803
/* Original name kept: used for the groupbox/static text items, whose IDs
 * are never tested - they just have to be unique within the template.
 */
#define DID_BOGUS                   804
#define DID_SPINR                   805
#define DID_SPING                   806
#define DID_SPINB                   807
#define COLOR_WHEEL_CLASS           "ColorWheelControlClass"

/* Private messages sent only to our own ColorPickingDialog window proc;
 * WM_USER is safe there because no standard control class posts WM_USER
 * traffic to its owner.
 */
#define MSG_EDITCELL                WM_USER
#define MSG_SETRGBSPINBTNS          WM_USER + 1
