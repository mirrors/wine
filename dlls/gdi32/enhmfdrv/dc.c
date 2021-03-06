/*
 * Enhanced MetaFile driver dc value functions
 *
 * Copyright 1999 Huw D M Davies
 * Copyright 2016 Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <assert.h>
#include "enhmfdrv/enhmetafiledrv.h"

/* get the emf physdev from the path physdev */
static inline PHYSDEV get_emfdev( PHYSDEV path )
{
    return &CONTAINING_RECORD( path, EMFDRV_PDEVICE, pathdev )->dev;
}

static const struct gdi_dc_funcs emfpath_driver;

INT CDECL EMFDRV_SaveDC( PHYSDEV dev )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSaveDC );
    INT ret = next->funcs->pSaveDC( next );

    if (ret)
    {
        EMRSAVEDC emr;
        emr.emr.iType = EMR_SAVEDC;
        emr.emr.nSize = sizeof(emr);
        EMFDRV_WriteRecord( dev, &emr.emr );
    }
    return ret;
}

BOOL CDECL EMFDRV_RestoreDC( PHYSDEV dev, INT level )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pRestoreDC );
    EMFDRV_PDEVICE* physDev = get_emf_physdev( dev );
    DC *dc = get_physdev_dc( dev );
    EMRRESTOREDC emr;
    BOOL ret;

    emr.emr.iType = EMR_RESTOREDC;
    emr.emr.nSize = sizeof(emr);

    if (level < 0)
        emr.iRelative = level;
    else
        emr.iRelative = level - dc->saveLevel - 1;

    physDev->restoring++;
    ret = next->funcs->pRestoreDC( next, level );
    physDev->restoring--;

    if (ret) EMFDRV_WriteRecord( dev, &emr.emr );
    return ret;
}

UINT CDECL EMFDRV_SetTextAlign( PHYSDEV dev, UINT align )
{
    EMRSETTEXTALIGN emr;
    emr.emr.iType = EMR_SETTEXTALIGN;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = align;
    return EMFDRV_WriteRecord( dev, &emr.emr ) ? align : GDI_ERROR;
}

BOOL CDECL EMFDRV_SetTextJustification(PHYSDEV dev, INT nBreakExtra, INT nBreakCount)
{
    EMRSETTEXTJUSTIFICATION emr;
    emr.emr.iType = EMR_SETTEXTJUSTIFICATION;
    emr.emr.nSize = sizeof(emr);
    emr.nBreakExtra = nBreakExtra;
    emr.nBreakCount = nBreakCount;
    return EMFDRV_WriteRecord(dev, &emr.emr);
}

INT CDECL EMFDRV_SetBkMode( PHYSDEV dev, INT mode )
{
    EMRSETBKMODE emr;
    emr.emr.iType = EMR_SETBKMODE;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = mode;
    return EMFDRV_WriteRecord( dev, &emr.emr ) ? mode : 0;
}

COLORREF CDECL EMFDRV_SetBkColor( PHYSDEV dev, COLORREF color )
{
    EMRSETBKCOLOR emr;
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );

    if (physDev->restoring) return color;  /* don't output records during RestoreDC */

    emr.emr.iType = EMR_SETBKCOLOR;
    emr.emr.nSize = sizeof(emr);
    emr.crColor = color;
    return EMFDRV_WriteRecord( dev, &emr.emr ) ? color : CLR_INVALID;
}


COLORREF CDECL EMFDRV_SetTextColor( PHYSDEV dev, COLORREF color )
{
    EMRSETTEXTCOLOR emr;
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );

    if (physDev->restoring) return color;  /* don't output records during RestoreDC */

    emr.emr.iType = EMR_SETTEXTCOLOR;
    emr.emr.nSize = sizeof(emr);
    emr.crColor = color;
    return EMFDRV_WriteRecord( dev, &emr.emr ) ? color : CLR_INVALID;
}

INT CDECL EMFDRV_SetROP2( PHYSDEV dev, INT rop )
{
    EMRSETROP2 emr;
    emr.emr.iType = EMR_SETROP2;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = rop;
    return EMFDRV_WriteRecord( dev, &emr.emr ) ? rop : 0;
}

INT CDECL EMFDRV_SetPolyFillMode( PHYSDEV dev, INT mode )
{
    EMRSETPOLYFILLMODE emr;
    emr.emr.iType = EMR_SETPOLYFILLMODE;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = mode;
    return EMFDRV_WriteRecord( dev, &emr.emr ) ? mode : 0;
}

INT CDECL EMFDRV_SetStretchBltMode( PHYSDEV dev, INT mode )
{
    EMRSETSTRETCHBLTMODE emr;
    emr.emr.iType = EMR_SETSTRETCHBLTMODE;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = mode;
    return EMFDRV_WriteRecord( dev, &emr.emr ) ? mode : 0;
}

INT CDECL EMFDRV_SetArcDirection(PHYSDEV dev, INT arcDirection)
{
    EMRSETARCDIRECTION emr;

    emr.emr.iType = EMR_SETARCDIRECTION;
    emr.emr.nSize = sizeof(emr);
    emr.iArcDirection = arcDirection;
    return EMFDRV_WriteRecord(dev, &emr.emr) ? arcDirection : 0;
}

INT CDECL EMFDRV_ExcludeClipRect( PHYSDEV dev, INT left, INT top, INT right, INT bottom )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pExcludeClipRect );
    EMREXCLUDECLIPRECT emr;

    emr.emr.iType      = EMR_EXCLUDECLIPRECT;
    emr.emr.nSize      = sizeof(emr);
    emr.rclClip.left   = left;
    emr.rclClip.top    = top;
    emr.rclClip.right  = right;
    emr.rclClip.bottom = bottom;
    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return ERROR;
    return next->funcs->pExcludeClipRect( next, left, top, right, bottom );
}

INT CDECL EMFDRV_IntersectClipRect( PHYSDEV dev, INT left, INT top, INT right, INT bottom)
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pIntersectClipRect );
    EMRINTERSECTCLIPRECT emr;

    emr.emr.iType      = EMR_INTERSECTCLIPRECT;
    emr.emr.nSize      = sizeof(emr);
    emr.rclClip.left   = left;
    emr.rclClip.top    = top;
    emr.rclClip.right  = right;
    emr.rclClip.bottom = bottom;
    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return ERROR;
    return next->funcs->pIntersectClipRect( next, left, top, right, bottom );
}

INT CDECL EMFDRV_OffsetClipRgn( PHYSDEV dev, INT x, INT y )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pOffsetClipRgn );
    EMROFFSETCLIPRGN emr;

    emr.emr.iType   = EMR_OFFSETCLIPRGN;
    emr.emr.nSize   = sizeof(emr);
    emr.ptlOffset.x = x;
    emr.ptlOffset.y = y;
    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return ERROR;
    return next->funcs->pOffsetClipRgn( next, x, y );
}

INT CDECL EMFDRV_ExtSelectClipRgn( PHYSDEV dev, HRGN hrgn, INT mode )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pExtSelectClipRgn );
    EMREXTSELECTCLIPRGN *emr;
    DWORD size, rgnsize;
    BOOL ret;

    if (!hrgn)
    {
        if (mode != RGN_COPY) return ERROR;
        rgnsize = 0;
    }
    else rgnsize = NtGdiGetRegionData( hrgn, 0, NULL );

    size = rgnsize + offsetof(EMREXTSELECTCLIPRGN,RgnData);
    emr = HeapAlloc( GetProcessHeap(), 0, size );
    if (rgnsize) NtGdiGetRegionData( hrgn, rgnsize, (RGNDATA *)&emr->RgnData );

    emr->emr.iType = EMR_EXTSELECTCLIPRGN;
    emr->emr.nSize = size;
    emr->cbRgnData = rgnsize;
    emr->iMode     = mode;

    ret = EMFDRV_WriteRecord( dev, &emr->emr );
    HeapFree( GetProcessHeap(), 0, emr );
    return ret ? next->funcs->pExtSelectClipRgn( next, hrgn, mode ) : ERROR;
}

INT CDECL EMFDRV_SetMapMode( PHYSDEV dev, INT mode )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSetMapMode );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETMAPMODE emr;
    INT ret;

    emr.emr.iType = EMR_SETMAPMODE;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = mode;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return 0;
    physDev->modifying_transform++;
    ret = next->funcs->pSetMapMode( next, mode );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_SetViewportExtEx( PHYSDEV dev, INT cx, INT cy, SIZE *size )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSetViewportExtEx );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETVIEWPORTEXTEX emr;
    BOOL ret;

    emr.emr.iType = EMR_SETVIEWPORTEXTEX;
    emr.emr.nSize = sizeof(emr);
    emr.szlExtent.cx = cx;
    emr.szlExtent.cy = cy;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pSetViewportExtEx( next, cx, cy, size );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_SetWindowExtEx( PHYSDEV dev, INT cx, INT cy, SIZE *size )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSetWindowExtEx );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETWINDOWEXTEX emr;
    BOOL ret;

    emr.emr.iType = EMR_SETWINDOWEXTEX;
    emr.emr.nSize = sizeof(emr);
    emr.szlExtent.cx = cx;
    emr.szlExtent.cy = cy;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pSetWindowExtEx( next, cx, cy, size );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_SetViewportOrgEx( PHYSDEV dev, INT x, INT y, POINT *pt )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSetViewportOrgEx );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETVIEWPORTORGEX emr;
    BOOL ret;

    emr.emr.iType = EMR_SETVIEWPORTORGEX;
    emr.emr.nSize = sizeof(emr);
    emr.ptlOrigin.x = x;
    emr.ptlOrigin.y = y;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pSetViewportOrgEx( next, x, y, pt );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_SetWindowOrgEx( PHYSDEV dev, INT x, INT y, POINT *pt )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSetWindowOrgEx );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETWINDOWORGEX emr;
    BOOL ret;

    emr.emr.iType = EMR_SETWINDOWORGEX;
    emr.emr.nSize = sizeof(emr);
    emr.ptlOrigin.x = x;
    emr.ptlOrigin.y = y;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pSetWindowOrgEx( next, x, y, pt );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_ScaleViewportExtEx( PHYSDEV dev, INT xNum, INT xDenom, INT yNum, INT yDenom, SIZE *size )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pScaleViewportExtEx );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSCALEVIEWPORTEXTEX emr;
    BOOL ret;

    emr.emr.iType = EMR_SCALEVIEWPORTEXTEX;
    emr.emr.nSize = sizeof(emr);
    emr.xNum      = xNum;
    emr.xDenom    = xDenom;
    emr.yNum      = yNum;
    emr.yDenom    = yDenom;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pScaleViewportExtEx( next, xNum, xDenom, yNum, yDenom, size );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_ScaleWindowExtEx( PHYSDEV dev, INT xNum, INT xDenom, INT yNum, INT yDenom, SIZE *size )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pScaleWindowExtEx );
    EMRSCALEWINDOWEXTEX emr;

    emr.emr.iType = EMR_SCALEWINDOWEXTEX;
    emr.emr.nSize = sizeof(emr);
    emr.xNum      = xNum;
    emr.xDenom    = xDenom;
    emr.yNum      = yNum;
    emr.yDenom    = yDenom;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    return next->funcs->pScaleWindowExtEx( next, xNum, xDenom, yNum, yDenom, size );
}

DWORD CDECL EMFDRV_SetLayout( PHYSDEV dev, DWORD layout )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSetLayout );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETLAYOUT emr;
    DWORD ret;

    emr.emr.iType = EMR_SETLAYOUT;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = layout;
    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return GDI_ERROR;
    physDev->modifying_transform++;
    ret = next->funcs->pSetLayout( next, layout );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_SetWorldTransform( PHYSDEV dev, const XFORM *xform)
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSetWorldTransform );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETWORLDTRANSFORM emr;
    BOOL ret;

    emr.emr.iType = EMR_SETWORLDTRANSFORM;
    emr.emr.nSize = sizeof(emr);
    emr.xform = *xform;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pSetWorldTransform( next, xform );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_ModifyWorldTransform( PHYSDEV dev, const XFORM *xform, DWORD mode)
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pModifyWorldTransform );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRMODIFYWORLDTRANSFORM emr;
    BOOL ret;

    emr.emr.iType = EMR_MODIFYWORLDTRANSFORM;
    emr.emr.nSize = sizeof(emr);
    if (mode == MWT_IDENTITY)
    {
        emr.xform.eM11 = 1.0f;
        emr.xform.eM12 = 0.0f;
        emr.xform.eM21 = 0.0f;
        emr.xform.eM22 = 1.0f;
        emr.xform.eDx = 0.0f;
        emr.xform.eDy = 0.0f;
    }
    else
    {
        emr.xform = *xform;
    }
    emr.iMode = mode;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pModifyWorldTransform( next, xform, mode );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_OffsetViewportOrgEx( PHYSDEV dev, INT x, INT y, POINT *pt )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pOffsetViewportOrgEx );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETVIEWPORTORGEX emr;
    POINT prev;
    BOOL ret;

    GetViewportOrgEx( dev->hdc, &prev );

    emr.emr.iType = EMR_SETVIEWPORTORGEX;
    emr.emr.nSize = sizeof(emr);
    emr.ptlOrigin.x = prev.x + x;
    emr.ptlOrigin.y = prev.y + y;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pOffsetViewportOrgEx( next, x, y, pt );
    physDev->modifying_transform--;
    return ret;
}

BOOL CDECL EMFDRV_OffsetWindowOrgEx( PHYSDEV dev, INT x, INT y, POINT *pt )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pOffsetWindowOrgEx );
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    EMRSETWINDOWORGEX emr;
    POINT prev;
    BOOL ret;

    GetWindowOrgEx( dev->hdc, &prev );

    emr.emr.iType = EMR_SETWINDOWORGEX;
    emr.emr.nSize = sizeof(emr);
    emr.ptlOrigin.x = prev.x + x;
    emr.ptlOrigin.y = prev.y + y;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    physDev->modifying_transform++;
    ret = next->funcs->pOffsetWindowOrgEx( next, x, y, pt );
    physDev->modifying_transform--;
    return ret;
}

DWORD CDECL EMFDRV_SetMapperFlags( PHYSDEV dev, DWORD flags )
{
    EMRSETMAPPERFLAGS emr;

    emr.emr.iType = EMR_SETMAPPERFLAGS;
    emr.emr.nSize = sizeof(emr);
    emr.dwFlags   = flags;

    return EMFDRV_WriteRecord( dev, &emr.emr ) ? flags : GDI_ERROR;
}

BOOL CDECL EMFDRV_AbortPath( PHYSDEV dev )
{
    EMRABORTPATH emr;

    emr.emr.iType = EMR_ABORTPATH;
    emr.emr.nSize = sizeof(emr);

    return EMFDRV_WriteRecord( dev, &emr.emr );
}

BOOL CDECL EMFDRV_BeginPath( PHYSDEV dev )
{
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pBeginPath );
    EMRBEGINPATH emr;
    DC *dc = get_physdev_dc( dev );

    emr.emr.iType = EMR_BEGINPATH;
    emr.emr.nSize = sizeof(emr);

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    if (physDev->path) return TRUE;  /* already open */

    if (!next->funcs->pBeginPath( next )) return FALSE;
    push_dc_driver( &dc->physDev, &physDev->pathdev, &emfpath_driver );
    physDev->path = TRUE;
    return TRUE;
}

BOOL CDECL EMFDRV_CloseFigure( PHYSDEV dev )
{
    EMRCLOSEFIGURE emr;

    emr.emr.iType = EMR_CLOSEFIGURE;
    emr.emr.nSize = sizeof(emr);

    EMFDRV_WriteRecord( dev, &emr.emr );
    return FALSE;  /* always fails without a path */
}

BOOL CDECL EMFDRV_EndPath( PHYSDEV dev )
{
    EMRENDPATH emr;

    emr.emr.iType = EMR_ENDPATH;
    emr.emr.nSize = sizeof(emr);

    EMFDRV_WriteRecord( dev, &emr.emr );
    return FALSE;  /* always fails without a path */
}

BOOL CDECL EMFDRV_FlattenPath( PHYSDEV dev )
{
    EMRFLATTENPATH emr;

    emr.emr.iType = EMR_FLATTENPATH;
    emr.emr.nSize = sizeof(emr);

    return EMFDRV_WriteRecord( dev, &emr.emr );
}

BOOL CDECL EMFDRV_SelectClipPath( PHYSDEV dev, INT iMode )
{
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pSelectClipPath );
    EMRSELECTCLIPPATH emr;
    BOOL ret = FALSE;
    HRGN hrgn;

    emr.emr.iType = EMR_SELECTCLIPPATH;
    emr.emr.nSize = sizeof(emr);
    emr.iMode = iMode;

    if (!EMFDRV_WriteRecord( dev, &emr.emr )) return FALSE;
    hrgn = PathToRegion( dev->hdc );
    if (hrgn)
    {
        ret = next->funcs->pExtSelectClipRgn( next, hrgn, iMode );
        DeleteObject( hrgn );
    }
    return ret;
}

BOOL CDECL EMFDRV_WidenPath( PHYSDEV dev )
{
    EMRWIDENPATH emr;

    emr.emr.iType = EMR_WIDENPATH;
    emr.emr.nSize = sizeof(emr);

    return EMFDRV_WriteRecord( dev, &emr.emr );
}

INT CDECL EMFDRV_GetDeviceCaps(PHYSDEV dev, INT cap)
{
    EMFDRV_PDEVICE *physDev = get_emf_physdev( dev );

    if (cap >= 0 && cap < ARRAY_SIZE( physDev->dev_caps ))
        return physDev->dev_caps[cap];
    return 0;
}


/***********************************************************************
 *           emfpathdrv_AbortPath
 */
static BOOL CDECL emfpathdrv_AbortPath( PHYSDEV dev )
{
    PHYSDEV emfdev = get_emfdev( dev );
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pAbortPath );
    DC *dc = get_physdev_dc( dev );

    emfpath_driver.pDeleteDC( pop_dc_driver( dc, &emfpath_driver ));
    emfdev->funcs->pAbortPath( emfdev );
    return next->funcs->pAbortPath( next );
}

/***********************************************************************
 *           emfpathdrv_BeginPath
 */
static BOOL CDECL emfpathdrv_BeginPath( PHYSDEV dev )
{
    PHYSDEV emfdev = get_emfdev( dev );
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pBeginPath );

    return (emfdev->funcs->pBeginPath( emfdev ) && next->funcs->pBeginPath( next ));
}

/***********************************************************************
 *           emfpathdrv_CloseFigure
 */
static BOOL CDECL emfpathdrv_CloseFigure( PHYSDEV dev )
{
    PHYSDEV emfdev = get_emfdev( dev );
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pCloseFigure );

    emfdev->funcs->pCloseFigure( emfdev );
    return next->funcs->pCloseFigure( next );
}

/***********************************************************************
 *           emfpathdrv_CreateDC
 */
static BOOL CDECL emfpathdrv_CreateDC( PHYSDEV *dev, LPCWSTR driver, LPCWSTR device,
                                       LPCWSTR output, const DEVMODEW *devmode )
{
    assert( 0 );  /* should never be called */
    return TRUE;
}

/*************************************************************
 *           emfpathdrv_DeleteDC
 */
static BOOL CDECL emfpathdrv_DeleteDC( PHYSDEV dev )
{
    EMFDRV_PDEVICE *physdev = (EMFDRV_PDEVICE *)get_emfdev( dev );

    physdev->path = FALSE;
    return TRUE;
}

/***********************************************************************
 *           emfpathdrv_EndPath
 */
static BOOL CDECL emfpathdrv_EndPath( PHYSDEV dev )
{
    PHYSDEV emfdev = get_emfdev( dev );
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pEndPath );
    DC *dc = get_physdev_dc( dev );

    emfpath_driver.pDeleteDC( pop_dc_driver( dc, &emfpath_driver ));
    emfdev->funcs->pEndPath( emfdev );
    return next->funcs->pEndPath( next );
}

/***********************************************************************
 *           emfpathdrv_ExtTextOut
 */
static BOOL CDECL emfpathdrv_ExtTextOut( PHYSDEV dev, INT x, INT y, UINT flags, const RECT *rect,
                                         LPCWSTR str, UINT count, const INT *dx )
{
    PHYSDEV emfdev = get_emfdev( dev );
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pExtTextOut );

    return (emfdev->funcs->pExtTextOut( emfdev, x, y, flags, rect, str, count, dx ) &&
            next->funcs->pExtTextOut( next, x, y, flags, rect, str, count, dx ));
}

/***********************************************************************
 *           emfpathdrv_PolyDraw
 */
static BOOL CDECL emfpathdrv_PolyDraw( PHYSDEV dev, const POINT *pts, const BYTE *types, DWORD count )
{
    PHYSDEV emfdev = get_emfdev( dev );
    PHYSDEV next = GET_NEXT_PHYSDEV( dev, pPolyDraw );

    return (emfdev->funcs->pPolyDraw( emfdev, pts, types, count ) &&
            next->funcs->pPolyDraw( next, pts, types, count ));
}


static const struct gdi_dc_funcs emfpath_driver =
{
    NULL,                               /* pAbortDoc */
    emfpathdrv_AbortPath,               /* pAbortPath */
    NULL,                               /* pAlphaBlend */
    NULL,                               /* pAngleArc */
    NULL,                               /* pArc */
    NULL,                               /* pArcTo */
    emfpathdrv_BeginPath,               /* pBeginPath */
    NULL,                               /* pBlendImage */
    NULL,                               /* pChord */
    emfpathdrv_CloseFigure,             /* pCloseFigure */
    NULL,                               /* pCreateCompatibleDC */
    emfpathdrv_CreateDC,                /* pCreateDC */
    emfpathdrv_DeleteDC,                /* pDeleteDC */
    NULL,                               /* pDeleteObject */
    NULL,                               /* pDeviceCapabilities */
    NULL,                               /* pEllipse */
    NULL,                               /* pEndDoc */
    NULL,                               /* pEndPage */
    emfpathdrv_EndPath,                 /* pEndPath */
    NULL,                               /* pEnumFonts */
    NULL,                               /* pEnumICMProfiles */
    NULL,                               /* pExcludeClipRect */
    NULL,                               /* pExtDeviceMode */
    NULL,                               /* pExtEscape */
    NULL,                               /* pExtFloodFill */
    NULL,                               /* pExtSelectClipRgn */
    emfpathdrv_ExtTextOut,              /* pExtTextOut */
    NULL,                               /* pFillPath */
    NULL,                               /* pFillRgn */
    NULL,                               /* pFlattenPath */
    NULL,                               /* pFontIsLinked */
    NULL,                               /* pFrameRgn */
    NULL,                               /* pGdiComment */
    NULL,                               /* pGetBoundsRect */
    NULL,                               /* pGetCharABCWidths */
    NULL,                               /* pGetCharABCWidthsI */
    NULL,                               /* pGetCharWidth */
    NULL,                               /* pGetCharWidthInfo */
    NULL,                               /* pGetDeviceCaps */
    NULL,                               /* pGetDeviceGammaRamp */
    NULL,                               /* pGetFontData */
    NULL,                               /* pGetFontRealizationInfo */
    NULL,                               /* pGetFontUnicodeRanges */
    NULL,                               /* pGetGlyphIndices */
    NULL,                               /* pGetGlyphOutline */
    NULL,                               /* pGetICMProfile */
    NULL,                               /* pGetImage */
    NULL,                               /* pGetKerningPairs */
    NULL,                               /* pGetNearestColor */
    NULL,                               /* pGetOutlineTextMetrics */
    NULL,                               /* pGetPixel */
    NULL,                               /* pGetSystemPaletteEntries */
    NULL,                               /* pGetTextCharsetInfo */
    NULL,                               /* pGetTextExtentExPoint */
    NULL,                               /* pGetTextExtentExPointI */
    NULL,                               /* pGetTextFace */
    NULL,                               /* pGetTextMetrics */
    NULL,                               /* pGradientFill */
    NULL,                               /* pIntersectClipRect */
    NULL,                               /* pInvertRgn */
    NULL,                               /* pLineTo */
    NULL,                               /* pModifyWorldTransform */
    NULL,                               /* pMoveTo */
    NULL,                               /* pOffsetClipRgn */
    NULL,                               /* pOffsetViewportOrg */
    NULL,                               /* pOffsetWindowOrg */
    NULL,                               /* pPaintRgn */
    NULL,                               /* pPatBlt */
    NULL,                               /* pPie */
    NULL,                               /* pPolyBezier */
    NULL,                               /* pPolyBezierTo */
    emfpathdrv_PolyDraw,                /* pPolyDraw */
    NULL,                               /* pPolyPolygon */
    NULL,                               /* pPolyPolyline */
    NULL,                               /* pPolylineTo */
    NULL,                               /* pPutImage */
    NULL,                               /* pRealizeDefaultPalette */
    NULL,                               /* pRealizePalette */
    NULL,                               /* pRectangle */
    NULL,                               /* pResetDC */
    NULL,                               /* pRestoreDC */
    NULL,                               /* pRoundRect */
    NULL,                               /* pSaveDC */
    NULL,                               /* pScaleViewportExt */
    NULL,                               /* pScaleWindowExt */
    NULL,                               /* pSelectBitmap */
    NULL,                               /* pSelectBrush */
    NULL,                               /* pSelectClipPath */
    NULL,                               /* pSelectFont */
    NULL,                               /* pSelectPalette */
    NULL,                               /* pSelectPen */
    NULL,                               /* pSetArcDirection */
    NULL,                               /* pSetBkColor */
    NULL,                               /* pSetBkMode */
    NULL,                               /* pSetBoundsRect */
    NULL,                               /* pSetDCBrushColor */
    NULL,                               /* pSetDCPenColor */
    NULL,                               /* pSetDIBitsToDevice */
    NULL,                               /* pSetDeviceClipping */
    NULL,                               /* pSetDeviceGammaRamp */
    NULL,                               /* pSetLayout */
    NULL,                               /* pSetMapMode */
    NULL,                               /* pSetMapperFlags */
    NULL,                               /* pSetPixel */
    NULL,                               /* pSetPolyFillMode */
    NULL,                               /* pSetROP2 */
    NULL,                               /* pSetRelAbs */
    NULL,                               /* pSetStretchBltMode */
    NULL,                               /* pSetTextAlign */
    NULL,                               /* pSetTextCharacterExtra */
    NULL,                               /* pSetTextColor */
    NULL,                               /* pSetTextJustification */
    NULL,                               /* pSetViewportExt */
    NULL,                               /* pSetViewportOrg */
    NULL,                               /* pSetWindowExt */
    NULL,                               /* pSetWindowOrg */
    NULL,                               /* pSetWorldTransform */
    NULL,                               /* pStartDoc */
    NULL,                               /* pStartPage */
    NULL,                               /* pStretchBlt */
    NULL,                               /* pStretchDIBits */
    NULL,                               /* pStrokeAndFillPath */
    NULL,                               /* pStrokePath */
    NULL,                               /* pUnrealizePalette */
    NULL,                               /* pWidenPath */
    NULL,                               /* pD3DKMTCheckVidPnExclusiveOwnership */
    NULL,                               /* pD3DKMTSetVidPnSourceOwner */
    NULL,                               /* wine_get_wgl_driver */
    NULL,                               /* wine_get_vulkan_driver */
    GDI_PRIORITY_PATH_DRV + 1           /* priority */
};
