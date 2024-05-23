
/* pdsctls.h	   Created:    1992-11-30  Revised:    1992-11-30	*/

#define	USER_RESERVED	   16	   /* Control Reserved Memory Size	*/

#define	QUCWP_DATA (QWL_USER +	4) /* Pointer to Styles	Data Pointer	*/
#define	QUCWP_HEAP (QWL_USER +	8) /* Pointer to Heap Handle		*/
#define	QUCWP_WNDP (QWL_USER + 12) /* Pointer to Heap Pointer		*/

/* --- Colour Wheel Window Information Structures ---------------------	*/

typedef	struct _CLRWHEEL
   {
   HWND	     hWnd;		   /* Control Window Handle		*/
   HPS	     hpsBitmap;		   /* Bitmap Presentation Space	Handle	*/
   ULONG     id;		   /* ID Value				*/
   TID	     tid;		   /* Thread ID				*/
   ULONG     flStyle;		   /* Style				*/
   BOOL	     fCapture;		   /* Capture Flag			*/
   ARCPARAMS ap;		   /* Arc Parameters			*/
   RECTL     rcl;		   /* Frame Rectangle			*/
   POINTL    aptlXHair[4];	   /* Cross Hair Point Array		*/
   POINTL    aptl[4];		   /* Bitmap Point Array		*/
   LONG	     lRadius;		   /* Radius				*/
   LONG	     lAngle;		   /* Angle				*/
   LONG	     lSaturationInc;	   /* Saturation Increment		*/
   LONG	     cx;		   /* Control Width			*/
   LONG	     cy;		   /* Control Height			*/
   POINTL    ptlOrigin;		   /* Wheel Origin			*/
   HWND	     hwndOwner;		   /* Owner Window Handle		*/
   HWND	     hwndParent;	   /* Parent Window Handle		*/
   HPOINTER  hptrArrow;		   /* Arrow Pointer			*/
   HBITMAP   hbm;		   /* Bitmap Handle			*/
   HBITMAP   hbmConstruct;	   /* Bitmap Handle			*/
   ULONG     ulOptions;		   /* Colour Table Options		*/
   ULONG     cPlanes;		   /* Bitmap Planes Count		*/
   ULONG     cBitCount;		   /* Bitmap Bits per Plane Count	*/
   } CLRWHEEL ;

typedef	CLRWHEEL *PCLRWHEEL;

/* --- Colour Wheel Support -------------------------------------------	*/

typedef	struct _CLRWHLCDATA
   {
   ULONG    cb;			   /* Structure	Size			*/
   LONG	    lAngle;		   /* Angle				*/
   LONG	    lSaturationInc;	   /* Saturation Increment		*/
   LONG	    lRadius;		   /* Radius				*/
   } CLRWHLCDATA ;

typedef	CLRWHLCDATA *PCLRWHLCDATA;

#define	CWN_RGBCLRSELECTED 750L
#define	CWN_HSBCLRSELECTED 751L
#define	CWN_XHAIRSELECTED  752L
#define	CWN_XHAIRRELEASED  753L

#define	CWS_SOLIDCLR	       0x0000001L
#define	CWS_HSB		       0x0000002L
#define	CWS_RGB		       0x0000004L
#define	CWS_BITMAP	       0x0000008L
#define	CWS_AUTOSIZE	       0x0000010L
#define	CWS_THREADED	       0x0000020L

#define	CWM_QUERYXHAIR	       (WM_USER	+ 32)
#define	CWM_SETXHAIR	       (WM_USER	+ 33)
#define	CWM_QUERYRGBCLR	       (WM_USER	+ 34)
#define	CWM_SETRGBCLR	       (WM_USER	+ 35)
#define	CWM_QUERYHSBCLR	       (WM_USER	+ 36)
#define	CWM_SETHSBCLR	       (WM_USER	+ 37)
