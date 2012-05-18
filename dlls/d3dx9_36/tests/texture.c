/*
 * Tests for the D3DX9 texture functions
 *
 * Copyright 2009 Tony Wasserka
 * Copyright 2010 Owen Rudge for CodeWeavers
 * Copyright 2010 Matteo Bruni for CodeWeavers
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

#define COBJMACROS
#include "wine/test.h"
#include "d3dx9tex.h"
#include "resources.h"

/* 2x2 16-bit dds, no mipmaps */
static const unsigned char dds_16bit[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x08,0x00,0x02,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x7c,0x00,0x00,
0xe0,0x03,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0x7f,0xff,0x7f,0xff,0x7f,0xff,0x7f
};

/* 2x2 24-bit dds, 2 mipmaps */
static const unsigned char dds_24bit[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x0a,0x00,0x02,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0xff,0x00,
0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x40,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

/* 4x4 cube map dds */
static const unsigned char dds_cube_map[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x08,0x00,0x04,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x44,0x58,0x54,0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x00,0x00,
0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50
};

/* 4x4x2 volume map dds, 2 mipmaps */
static const unsigned char dds_volume_map[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x8a,0x00,0x04,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x44,0x58,0x54,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x40,0x00,
0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x2f,0x7e,0xcf,0x79,0x01,0x54,0x5c,0x5c,
0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x84,0xef,0x7b,0xaa,0xab,0xab,0xab
};

static BOOL is_autogenmipmap_supported(IDirect3DDevice9 *device, D3DRESOURCETYPE resource_type)
{
    HRESULT hr;
    D3DCAPS9 caps;
    IDirect3D9 *d3d9;
    D3DDISPLAYMODE mode;
    D3DDEVICE_CREATION_PARAMETERS params;

    IDirect3DDevice9_GetDeviceCaps(device, &caps);
    IDirect3DDevice9_GetDirect3D(device, &d3d9);
    IDirect3DDevice9_GetCreationParameters(device, &params);
    IDirect3DDevice9_GetDisplayMode(device, 0, &mode);

    if (!(caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP))
        return FALSE;

    hr = IDirect3D9_CheckDeviceFormat(d3d9, params.AdapterOrdinal, params.DeviceType,
        mode.Format, D3DUSAGE_AUTOGENMIPMAP, resource_type, D3DFMT_A8R8G8B8);

    IDirect3D9_Release(d3d9);
    return SUCCEEDED(hr);
}

static void test_D3DXCheckTextureRequirements(IDirect3DDevice9 *device)
{
    UINT width, height, mipmaps;
    D3DFORMAT format, expected;
    D3DCAPS9 caps;
    HRESULT hr;
    IDirect3D9 *d3d;
    D3DDEVICE_CREATION_PARAMETERS params;
    D3DDISPLAYMODE mode;

    IDirect3DDevice9_GetDeviceCaps(device, &caps);

    /* general tests */
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, D3DX_DEFAULT, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXCheckTextureRequirements(NULL, NULL, NULL, NULL, D3DX_DEFAULT, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    /* width & height */
    width = height = D3DX_DEFAULT;
    hr = D3DXCheckTextureRequirements(device, &width, &height, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 256, "Returned width %d, expected %d\n", width, 256);
    ok(height == 256, "Returned height %d, expected %d\n", height, 256);

    width = D3DX_DEFAULT;
    hr = D3DXCheckTextureRequirements(device, &width, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 256, "Returned width %d, expected %d\n", width, 256);

    if (caps.TextureCaps & D3DPTEXTURECAPS_POW2)
        skip("Hardware only supports pow2 textures\n");
    else
    {
        width = 62;
        hr = D3DXCheckTextureRequirements(device, &width, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(width == 62, "Returned width %d, expected %d\n", width, 62);

        width = D3DX_DEFAULT; height = 63;
        hr = D3DXCheckTextureRequirements(device, &width, &height, NULL, 0, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(width == height, "Returned width %d, expected %d\n", width, height);
        ok(height == 63, "Returned height %d, expected %d\n", height, 63);
    }

    width = D3DX_DEFAULT; height = 0;
    hr = D3DXCheckTextureRequirements(device, &width, &height, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 1, "Returned width %d, expected %d\n", width, 1);
    ok(height == 1, "Returned height %d, expected %d\n", height, 1);

    width = 0; height = 0;
    hr = D3DXCheckTextureRequirements(device, &width, &height, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 1, "Returned width %d, expected %d\n", width, 1);
    ok(height == 1, "Returned height %d, expected %d\n", height, 1);

    width = 0;
    hr = D3DXCheckTextureRequirements(device, &width, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 1, "Returned width %d, expected %d\n", width, 1);

    width = 0xFFFFFFFE;
    hr = D3DXCheckTextureRequirements(device, &width, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == caps.MaxTextureWidth, "Returned width %d, expected %d\n", width, caps.MaxTextureWidth);

    width = caps.MaxTextureWidth-1;
    hr = D3DXCheckTextureRequirements(device, &width, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    if (caps.TextureCaps & D3DPTEXTURECAPS_POW2)
        ok(width == caps.MaxTextureWidth, "Returned width %d, expected %d\n", width, caps.MaxTextureWidth);
    else
        ok(width == caps.MaxTextureWidth-1, "Returned width %d, expected %d\n", width, caps.MaxTextureWidth-1);

    /* mipmaps */
    width = 64; height = 63;
    mipmaps = 9;
    hr = D3DXCheckTextureRequirements(device, &width, &height, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 7, "Returned mipmaps %d, expected %d\n", mipmaps, 7);

    if (!(caps.TextureCaps & D3DPTEXTURECAPS_POW2))
    {
        width = 284; height = 137;
        mipmaps = 20;
        hr = D3DXCheckTextureRequirements(device, &width, &height, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(mipmaps == 9, "Returned mipmaps %d, expected %d\n", mipmaps, 9);

        width = height = 63;
        mipmaps = 9;
        hr = D3DXCheckTextureRequirements(device, &width, &height, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(mipmaps == 6, "Returned mipmaps %d, expected %d\n", mipmaps, 6);
    }
    else
        skip("Skipping some tests, npot2 textures unsupported\n");

    mipmaps = 20;
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 9, "Returned mipmaps %d, expected %d\n", mipmaps, 9);

    mipmaps = 0;
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 9, "Returned mipmaps %d, expected %d\n", mipmaps, 9);

    /* mipmaps when D3DUSAGE_AUTOGENMIPMAP is set */
    if (is_autogenmipmap_supported(device, D3DRTYPE_TEXTURE))
    {
        mipmaps = 0;
        hr = D3DXCheckTextureRequirements(device, NULL, NULL, &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(mipmaps == 0, "Returned mipmaps %d, expected %d\n", mipmaps, 0);
        mipmaps = 1;
        hr = D3DXCheckTextureRequirements(device, NULL, NULL, &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(mipmaps == 1, "Returned mipmaps %d, expected %d\n", mipmaps, 1);
        mipmaps = 2;
        hr = D3DXCheckTextureRequirements(device, NULL, NULL, &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(mipmaps == 0, "Returned mipmaps %d, expected %d\n", mipmaps, 0);
        mipmaps = 6;
        hr = D3DXCheckTextureRequirements(device, NULL, NULL, &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(mipmaps == 0, "Returned mipmaps %d, expected %d\n", mipmaps, 0);
    }
    else
        skip("No D3DUSAGE_AUTOGENMIPMAP support for textures\n");

    /* usage */
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, D3DUSAGE_DONOTCLIP, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, D3DUSAGE_POINTS, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, D3DUSAGE_RTPATCHES, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, D3DUSAGE_NPATCHES, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckTextureRequirements succeeded, but should've failed.\n");

    /* format */
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    format = D3DFMT_UNKNOWN;
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_A8R8G8B8);

    format = D3DX_DEFAULT;
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_A8R8G8B8);

    format = D3DFMT_R8G8B8;
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_X8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_X8R8G8B8);

    IDirect3DDevice9_GetDirect3D(device, &d3d);
    IDirect3DDevice9_GetCreationParameters(device, &params);
    IDirect3DDevice9_GetDisplayMode(device, 0, &mode);

    if (SUCCEEDED(IDirect3D9_CheckDeviceFormat(d3d, params.AdapterOrdinal, params.DeviceType,
                                               mode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_R3G3B2)))
        expected = D3DFMT_R3G3B2;
    else
    {
        if (SUCCEEDED(IDirect3D9_CheckDeviceFormat(d3d, params.AdapterOrdinal, params.DeviceType,
                                                   mode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_X4R4G4B4)))
            expected = D3DFMT_X4R4G4B4;
        else
            expected = D3DFMT_X1R5G5B5;
    }

    format = D3DFMT_R3G3B2;
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == expected, "Returned format %u, expected %u\n", format, expected);

    if(SUCCEEDED(IDirect3D9_CheckDeviceFormat(d3d, params.AdapterOrdinal, params.DeviceType,
                                              mode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8R3G3B2)))
        expected = D3DFMT_A8R3G3B2;
    else
        expected = D3DFMT_A8R8G8B8;

    format = D3DFMT_A8R3G3B2;
    hr = D3DXCheckTextureRequirements(device, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == expected, "Returned format %u, expected %u\n", format, expected);

    IDirect3D9_Release(d3d);
}

static void test_D3DXCheckCubeTextureRequirements(IDirect3DDevice9 *device)
{
    UINT size, mipmaps, expected;
    D3DFORMAT format;
    D3DCAPS9 caps;
    HRESULT hr;

    IDirect3DDevice9_GetDeviceCaps(device, &caps);

    if (!(caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
    {
        skip("No cube textures support\n");
        return;
    }

    /* general tests */
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, D3DX_DEFAULT, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXCheckCubeTextureRequirements(NULL, NULL, NULL, D3DX_DEFAULT, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    /* size */
    size = D3DX_DEFAULT;
    hr = D3DXCheckCubeTextureRequirements(device, &size, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(size == 256, "Returned size %d, expected %d\n", size, 256);

    /* mipmaps */
    size = 64;
    mipmaps = 9;
    hr = D3DXCheckCubeTextureRequirements(device, &size, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 7, "Returned mipmaps %d, expected %d\n", mipmaps, 7);

    size = 284;
    mipmaps = 20;
    expected = caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2 ? 10 : 9;
    hr = D3DXCheckCubeTextureRequirements(device, &size, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == expected, "Returned mipmaps %d, expected %d\n", mipmaps, expected);

    size = 63;
    mipmaps = 9;
    expected = caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2 ? 7 : 6;
    hr = D3DXCheckCubeTextureRequirements(device, &size, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == expected, "Returned mipmaps %d, expected %d\n", mipmaps, expected);

    mipmaps = 0;
    hr = D3DXCheckCubeTextureRequirements(device, NULL, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 9, "Returned mipmaps %d, expected %d\n", mipmaps, 9);

    if (is_autogenmipmap_supported(device, D3DRTYPE_CUBETEXTURE))
    {
        mipmaps = 3;
        hr = D3DXCheckCubeTextureRequirements(device, NULL,  &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
        ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
        ok(mipmaps == 0, "Returned mipmaps %d, expected %d\n", mipmaps, 0);
    }
    else
        skip("No D3DUSAGE_AUTOGENMIPMAP support for cube textures\n");

    /* usage */
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckCubeTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, D3DUSAGE_DONOTCLIP, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckCubeTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, D3DUSAGE_POINTS, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckCubeTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, D3DUSAGE_RTPATCHES, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckCubeTextureRequirements succeeded, but should've failed.\n");
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, D3DUSAGE_NPATCHES, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckCubeTextureRequirements succeeded, but should've failed.\n");

    /* format */
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    format = D3DFMT_UNKNOWN;
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_A8R8G8B8);

    format = D3DX_DEFAULT;
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_A8R8G8B8);

    format = D3DFMT_R8G8B8;
    hr = D3DXCheckCubeTextureRequirements(device, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckCubeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_X8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_X8R8G8B8);
}

static void test_D3DXCheckVolumeTextureRequirements(IDirect3DDevice9 *device)
{
    UINT width, height, depth, mipmaps, expected;
    D3DFORMAT format;
    D3DCAPS9 caps;
    HRESULT hr;

    IDirect3DDevice9_GetDeviceCaps(device, &caps);

    if (!(caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
    {
        skip("No volume textures support\n");
        return;
    }

    /* general tests */
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL, NULL, NULL, D3DX_DEFAULT, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXCheckVolumeTextureRequirements(NULL, NULL, NULL, NULL, NULL, D3DX_DEFAULT, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    /* width, height, depth */
    width = height = depth = D3DX_DEFAULT;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, &height, &depth, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 256, "Returned width %d, expected %d\n", width, 256);
    ok(height == 256, "Returned height %d, expected %d\n", height, 256);
    ok(depth == 1, "Returned depth %d, expected %d\n", depth, 1);

    width = D3DX_DEFAULT;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, NULL, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 256, "Returned width %d, expected %d\n", width, 256);

    width = D3DX_DEFAULT; height = 0; depth = 0;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, &height, &depth, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 1, "Returned width %d, expected %d\n", width, 1);
    ok(height == 1, "Returned height %d, expected %d\n", height, 1);
    ok(depth == 1, "Returned height %d, expected %d\n", depth, 1);

    width = 0; height = 0; depth = 0;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, &height, &depth, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 1, "Returned width %d, expected %d\n", width, 1);
    ok(height == 1, "Returned height %d, expected %d\n", height, 1);
    ok(depth == 1, "Returned height %d, expected %d\n", depth, 1);

    width = 0;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, NULL, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == 1, "Returned width %d, expected %d\n", width, 1);

    width = 0xFFFFFFFE;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, NULL, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(width == caps.MaxVolumeExtent, "Returned width %d, expected %d\n", width, caps.MaxVolumeExtent);

    /* format */
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL, NULL, NULL, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);

    format = D3DFMT_UNKNOWN;
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_A8R8G8B8);

    format = D3DX_DEFAULT;
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_A8R8G8B8);

    format = D3DFMT_R8G8B8;
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL, NULL, NULL, 0, &format, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(format == D3DFMT_X8R8G8B8, "Returned format %u, expected %u\n", format, D3DFMT_X8R8G8B8);

    /* mipmaps */
    if (!(caps.TextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP))
    {
        skip("No volume textures mipmapping support\n");
        return;
    }

    width = height = depth = 64;
    mipmaps = 9;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, &height, &depth, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 7, "Returned mipmaps %d, expected %d\n", mipmaps, 7);

    width = 284;
    height = 143;
    depth = 55;
    mipmaps = 20;
    expected = caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2 ? 10 : 9;
    hr = D3DXCheckVolumeTextureRequirements(device, &width, &height, &depth, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == expected, "Returned mipmaps %d, expected %d\n", mipmaps, expected);

    mipmaps = 0;
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL, NULL, &mipmaps, 0, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 9, "Returned mipmaps %d, expected %d\n", mipmaps, 9);

    if (!is_autogenmipmap_supported(device, D3DRTYPE_VOLUMETEXTURE))
    {
        skip("No D3DUSAGE_AUTOGENMIPMAP support for volume textures\n");
        return;
    }

    /* mipmaps when D3DUSAGE_AUTOGENMIPMAP is set */
    mipmaps = 0;
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL,NULL, &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 0, "Returned mipmaps %d, expected %d\n", mipmaps, 0);
    mipmaps = 1;
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL,NULL, &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 1, "Returned mipmaps %d, expected %d\n", mipmaps, 1);
    mipmaps = 3;
    hr = D3DXCheckVolumeTextureRequirements(device, NULL, NULL,NULL, &mipmaps, D3DUSAGE_AUTOGENMIPMAP, NULL, D3DPOOL_DEFAULT);
    ok(hr == D3D_OK, "D3DXCheckVolumeTextureRequirements returned %#x, expected %#x\n", hr, D3D_OK);
    ok(mipmaps == 0, "Returned mipmaps %d, expected %d\n", mipmaps, 0);
}

static void test_D3DXCreateTexture(IDirect3DDevice9 *device)
{
    IDirect3DTexture9 *texture;
    D3DSURFACE_DESC desc;
    D3DCAPS9 caps;
    UINT mipmaps;
    HRESULT hr;

    IDirect3DDevice9_GetDeviceCaps(device, &caps);

    hr = D3DXCreateTexture(NULL, 0, 0, 0, 0, D3DX_DEFAULT, D3DPOOL_DEFAULT, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    /* width and height tests */

    hr = D3DXCreateTexture(device, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        hr = IDirect3DTexture9_GetLevelDesc(texture, 0, &desc);
        ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
        ok(desc.Format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", desc.Format, D3DFMT_A8R8G8B8);

        ok(desc.Width == 256, "Returned width %d, expected %d\n", desc.Width, 256);
        ok(desc.Height == 256, "Returned height %d, expected %d\n", desc.Height, 256);

        IDirect3DTexture9_Release(texture);
    }


    hr = D3DXCreateTexture(device, 0, 0, 0, 0, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        hr = IDirect3DTexture9_GetLevelDesc(texture, 0, &desc);
        ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
        ok(desc.Format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", desc.Format, D3DFMT_A8R8G8B8);

        ok(desc.Width == 1, "Returned width %d, expected %d\n", desc.Width, 1);
        ok(desc.Height == 1, "Returned height %d, expected %d\n", desc.Height, 1);

        IDirect3DTexture9_Release(texture);
    }


    if (caps.TextureCaps & D3DPTEXTURECAPS_POW2)
        skip("Hardware only supports pow2 textures\n");
    else
    {
        hr = D3DXCreateTexture(device, D3DX_DEFAULT, 63, 0, 0, 0, D3DPOOL_DEFAULT, &texture);
        ok((hr == D3D_OK) ||
           /* may not work with conditional NPOT */
           ((hr != D3D_OK) && (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)),
           "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

        if (texture)
        {
            hr = IDirect3DTexture9_GetLevelDesc(texture, 0, &desc);
            ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
            ok(desc.Format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", desc.Format, D3DFMT_A8R8G8B8);

            /* Conditional NPOT may create a texture with different dimensions, so allow those
               situations instead of returning a fail */

            ok(desc.Width == 63 ||
               (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL),
               "Returned width %d, expected %d\n", desc.Width, 63);

            ok(desc.Height == 63 ||
               (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL),
               "Returned height %d, expected %d\n", desc.Height, 63);

            IDirect3DTexture9_Release(texture);
        }
    }

    /* mipmaps */

    hr = D3DXCreateTexture(device, 64, 63, 9, 0, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        mipmaps = IDirect3DTexture9_GetLevelCount(texture);
        ok(mipmaps == 7, "Returned mipmaps %d, expected %d\n", mipmaps, 7);

        IDirect3DTexture9_Release(texture);
    }


    hr = D3DXCreateTexture(device, 284, 137, 9, 0, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        mipmaps = IDirect3DTexture9_GetLevelCount(texture);
        ok(mipmaps == 9, "Returned mipmaps %d, expected %d\n", mipmaps, 9);

        IDirect3DTexture9_Release(texture);
    }


    hr = D3DXCreateTexture(device, 0, 0, 20, 0, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        mipmaps = IDirect3DTexture9_GetLevelCount(texture);
        ok(mipmaps == 1, "Returned mipmaps %d, expected %d\n", mipmaps, 1);

        IDirect3DTexture9_Release(texture);
    }


    hr = D3DXCreateTexture(device, 64, 64, 1, 0, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        mipmaps = IDirect3DTexture9_GetLevelCount(texture);
        ok(mipmaps == 1, "Returned mipmaps %d, expected %d\n", mipmaps, 1);

        IDirect3DTexture9_Release(texture);
    }

    /* usage */

    hr = D3DXCreateTexture(device, 0, 0, 0, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTexture succeeded, but should have failed.\n");
    hr = D3DXCreateTexture(device, 0, 0, 0, D3DUSAGE_DONOTCLIP, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTexture succeeded, but should have failed.\n");
    hr = D3DXCreateTexture(device, 0, 0, 0, D3DUSAGE_POINTS, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTexture succeeded, but should have failed.\n");
    hr = D3DXCreateTexture(device, 0, 0, 0, D3DUSAGE_RTPATCHES, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTexture succeeded, but should have failed.\n");
    hr = D3DXCreateTexture(device, 0, 0, 0, D3DUSAGE_NPATCHES, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTexture succeeded, but should have failed.\n");

    /* format */

    hr = D3DXCreateTexture(device, 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        hr = IDirect3DTexture9_GetLevelDesc(texture, 0, &desc);
        ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
        ok(desc.Format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", desc.Format, D3DFMT_A8R8G8B8);

        IDirect3DTexture9_Release(texture);
    }


    hr = D3DXCreateTexture(device, 0, 0, 0, 0, 0, D3DPOOL_DEFAULT, &texture);
    ok(hr == D3D_OK, "D3DXCreateTexture returned %#x, expected %#x\n", hr, D3D_OK);

    if (texture)
    {
        hr = IDirect3DTexture9_GetLevelDesc(texture, 0, &desc);
        ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
        ok(desc.Format == D3DFMT_A8R8G8B8, "Returned format %u, expected %u\n", desc.Format, D3DFMT_A8R8G8B8);

        IDirect3DTexture9_Release(texture);
    }

    /* D3DXCreateTextureFromResource */
    todo_wine {
        hr = D3DXCreateTextureFromResourceA(device, NULL, MAKEINTRESOURCEA(IDB_BITMAP_1x1), &texture);
        ok(hr == D3D_OK, "D3DXCreateTextureFromResource returned %#x, expected %#x\n", hr, D3D_OK);
        if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);
    }
    hr = D3DXCreateTextureFromResourceA(device, NULL, MAKEINTRESOURCEA(IDD_BITMAPDATA_1x1), &texture);
    ok(hr == D3D_OK, "D3DXCreateTextureFromResource returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);

    hr = D3DXCreateTextureFromResourceA(device, NULL, MAKEINTRESOURCEA(IDS_STRING), &texture);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXCreateTextureFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXCreateTextureFromResourceA(NULL, NULL, MAKEINTRESOURCEA(IDD_BITMAPDATA_1x1), &texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTextureFromResource returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXCreateTextureFromResourceA(device, NULL, NULL, &texture);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXCreateTextureFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXCreateTextureFromResourceA(device, NULL, MAKEINTRESOURCEA(IDD_BITMAPDATA_1x1), NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTextureFromResource returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);


    /* D3DXCreateTextureFromResourceEx */
    hr = D3DXCreateTextureFromResourceExA(device, NULL, MAKEINTRESOURCEA(IDD_BITMAPDATA_1x1), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
    ok(hr == D3D_OK, "D3DXCreateTextureFromResourceEx returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);

    hr = D3DXCreateTextureFromResourceExA(device, NULL, MAKEINTRESOURCEA(IDS_STRING), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXCreateTextureFromResourceEx returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXCreateTextureFromResourceExA(NULL, NULL, MAKEINTRESOURCEA(IDD_BITMAPDATA_1x1), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTextureFromResourceEx returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXCreateTextureFromResourceExA(device, NULL, NULL, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXCreateTextureFromResourceEx returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXCreateTextureFromResourceExA(device, NULL, MAKEINTRESOURCEA(IDD_BITMAPDATA_1x1), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateTextureFromResourceEx returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);
}

static void test_D3DXFilterTexture(IDirect3DDevice9 *device)
{
    IDirect3DTexture9 *tex;
    IDirect3DCubeTexture9 *cubetex;
    HRESULT hr;

    hr = IDirect3DDevice9_CreateTexture(device, 256, 256, 5, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) tex, NULL, 0, D3DX_FILTER_NONE);
        ok(hr == D3D_OK, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3D_OK);

        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) tex, NULL, 0, D3DX_FILTER_BOX + 1); /* Invalid filter */
        ok(hr == D3DERR_INVALIDCALL, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) tex, NULL, 5, D3DX_FILTER_NONE); /* Invalid miplevel */
        ok(hr == D3DERR_INVALIDCALL, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);
    }
    else
        skip("Failed to create texture\n");

    IDirect3DTexture9_Release(tex);

    hr = D3DXFilterTexture(NULL, NULL, 0, D3DX_FILTER_NONE);
    ok(hr == D3DERR_INVALIDCALL, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    /* Test different pools */
    hr = IDirect3DDevice9_CreateTexture(device, 256, 256, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) tex, NULL, 0, D3DX_FILTER_NONE);
        ok(hr == D3D_OK, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DTexture9_Release(tex);
    }
    else
        skip("Failed to create texture\n");

    hr = IDirect3DDevice9_CreateTexture(device, 256, 256, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) tex, NULL, 0, D3DX_FILTER_NONE);
        ok(hr == D3D_OK, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DTexture9_Release(tex);
    }
    else
        skip("Failed to create texture\n");

    /* Cube texture test */
    hr = IDirect3DDevice9_CreateCubeTexture(device, 256, 5, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &cubetex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) cubetex, NULL, 0, D3DX_FILTER_NONE);
        ok(hr == D3D_OK, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3D_OK);

        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) cubetex, NULL, 0, D3DX_FILTER_BOX + 1); /* Invalid filter */
        ok(hr == D3DERR_INVALIDCALL, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXFilterTexture((IDirect3DBaseTexture9*) cubetex, NULL, 5, D3DX_FILTER_NONE); /* Invalid miplevel */
        ok(hr == D3DERR_INVALIDCALL, "D3DXFilterTexture returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);
        IDirect3DCubeTexture9_Release(cubetex);
    }
    else
        skip("Failed to create texture\n");
}

static BOOL color_match(const DWORD *value, const DWORD *expected)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        DWORD diff = value[i] > expected[i] ? value[i] - expected[i] : expected[i] - value[i];
        if (diff > 1) return FALSE;
    }
    return TRUE;
}

static void WINAPI fillfunc(D3DXVECTOR4 *value, const D3DXVECTOR2 *texcoord,
                            const D3DXVECTOR2 *texelsize, void *data)
{
    value->x = texcoord->x;
    value->y = texcoord->y;
    value->z = texelsize->x;
    value->w = 1.0f;
}

static void test_D3DXFillTexture(IDirect3DDevice9 *device)
{
    IDirect3DTexture9 *tex;
    HRESULT hr;
    D3DLOCKED_RECT lock_rect;
    DWORD x, y, m;
    DWORD v[4], e[4];
    DWORD value, expected, size, pitch;

    size = 4;
    hr = IDirect3DDevice9_CreateTexture(device, size, size, 0, 0, D3DFMT_A8R8G8B8,
                                        D3DPOOL_MANAGED, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFillTexture(tex, fillfunc, NULL);
        ok(hr == D3D_OK, "D3DXFillTexture returned %#x, expected %#x\n", hr, D3D_OK);

        for (m = 0; m < 3; m++)
        {
            hr = IDirect3DTexture9_LockRect(tex, m, &lock_rect, NULL, D3DLOCK_READONLY);
            ok(hr == D3D_OK, "Couldn't lock the texture, error %#x\n", hr);
            if (SUCCEEDED(hr))
            {
                pitch = lock_rect.Pitch / sizeof(DWORD);
                for (y = 0; y < size; y++)
                {
                    for (x = 0; x < size; x++)
                    {
                        value = ((DWORD *)lock_rect.pBits)[y * pitch + x];
                        v[0] = (value >> 24) & 0xff;
                        v[1] = (value >> 16) & 0xff;
                        v[2] = (value >> 8) & 0xff;
                        v[3] = value & 0xff;

                        e[0] = 0xff;
                        e[1] = (x + 0.5f) / size * 255.0f + 0.5f;
                        e[2] = (y + 0.5f) / size * 255.0f + 0.5f;
                        e[3] = 255.0f / size + 0.5f;
                        expected = e[0] << 24 | e[1] << 16 | e[2] << 8 | e[3];

                        ok(color_match(v, e),
                           "Texel at (%u, %u) doesn't match: %#x, expected %#x\n",
                           x, y, value, expected);
                    }
                }
                IDirect3DTexture9_UnlockRect(tex, m);
            }
            size >>= 1;
        }
    }
    else
        skip("Failed to create texture\n");

    IDirect3DTexture9_Release(tex);

    hr = IDirect3DDevice9_CreateTexture(device, 4, 4, 1, 0, D3DFMT_A1R5G5B5,
                                        D3DPOOL_MANAGED, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFillTexture(tex, fillfunc, NULL);
        ok(hr == D3D_OK, "D3DXFillTexture returned %#x, expected %#x\n", hr, D3D_OK);

        hr = IDirect3DTexture9_LockRect(tex, 0, &lock_rect, NULL, D3DLOCK_READONLY);
        ok(hr == D3D_OK, "Couldn't lock the texture, error %#x\n", hr);
        if (SUCCEEDED(hr))
        {
            pitch = lock_rect.Pitch / sizeof(WORD);
            for (y = 0; y < 4; y++)
            {
                for (x = 0; x < 4; x++)
                {
                    value = ((WORD *)lock_rect.pBits)[y * pitch + x];
                    v[0] = value >> 15;
                    v[1] = value >> 10 & 0x1f;
                    v[2] = value >> 5 & 0x1f;
                    v[3] = value & 0x1f;

                    e[0] = 1;
                    e[1] = (x + 0.5f) / 4.0f * 31.0f + 0.5f;
                    e[2] = (y + 0.5f) / 4.0f * 31.0f + 0.5f;
                    e[3] = 8;
                    expected = e[0] << 15 | e[1] << 10 | e[2] << 5 | e[3];

                    ok(color_match(v, e),
                       "Texel at (%u, %u) doesn't match: %#x, expected %#x\n",
                       x, y, value, expected);
                }
            }
            IDirect3DTexture9_UnlockRect(tex, 0);
        }
    }
    else
        skip("Failed to create texture\n");

    IDirect3DTexture9_Release(tex);
}

static void WINAPI fillfunc_cube(D3DXVECTOR4 *value, const D3DXVECTOR3 *texcoord,
                                 const D3DXVECTOR3 *texelsize, void *data)
{
    value->x = (texcoord->x + 1.0f) / 2.0f;
    value->y = (texcoord->y + 1.0f) / 2.0f;
    value->z = (texcoord->z + 1.0f) / 2.0f;
    value->w = texelsize->x;
}

enum cube_coord
{
    XCOORD = 0,
    XCOORDINV = 1,
    YCOORD = 2,
    YCOORDINV = 3,
    ZERO = 4,
    ONE = 5
};

static float get_cube_coord(enum cube_coord coord, unsigned int x, unsigned int y, unsigned int size)
{
    switch (coord)
    {
        case XCOORD:
            return x + 0.5f;
        case XCOORDINV:
            return size - x - 0.5f;
        case YCOORD:
            return y + 0.5f;
        case YCOORDINV:
            return size - y - 0.5f;
        case ZERO:
            return 0.0f;
        case ONE:
            return size;
        default:
           trace("Unexpected coordinate value\n");
           return 0.0f;
    }
}

static void test_D3DXFillCubeTexture(IDirect3DDevice9 *device)
{
    IDirect3DCubeTexture9 *tex;
    HRESULT hr;
    D3DLOCKED_RECT lock_rect;
    DWORD x, y, f, m;
    DWORD v[4], e[4];
    DWORD value, expected, size, pitch;
    enum cube_coord coordmap[6][3] =
        {
            {ONE, YCOORDINV, XCOORDINV},
            {ZERO, YCOORDINV, XCOORD},
            {XCOORD, ONE, YCOORD},
            {XCOORD, ZERO, YCOORDINV},
            {XCOORD, YCOORDINV, ONE},
            {XCOORDINV, YCOORDINV, ZERO}
        };

    size = 4;
    hr = IDirect3DDevice9_CreateCubeTexture(device, size, 0, 0, D3DFMT_A8R8G8B8,
                                            D3DPOOL_MANAGED, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFillCubeTexture(tex, fillfunc_cube, NULL);
        ok(hr == D3D_OK, "D3DXFillCubeTexture returned %#x, expected %#x\n", hr, D3D_OK);

        for (m = 0; m < 3; m++)
        {
            for (f = 0; f < 6; f++)
            {
                hr = IDirect3DCubeTexture9_LockRect(tex, f, m, &lock_rect, NULL, D3DLOCK_READONLY);
                ok(hr == D3D_OK, "Couldn't lock the texture, error %#x\n", hr);
                if (SUCCEEDED(hr))
                {
                    pitch = lock_rect.Pitch / sizeof(DWORD);
                    for (y = 0; y < size; y++)
                    {
                        for (x = 0; x < size; x++)
                        {
                            value = ((DWORD *)lock_rect.pBits)[y * pitch + x];
                            v[0] = (value >> 24) & 0xff;
                            v[1] = (value >> 16) & 0xff;
                            v[2] = (value >> 8) & 0xff;
                            v[3] = value & 0xff;

                            e[0] = (f == 0) || (f == 1) ?
                                0 : (BYTE)(255.0f / size * 2.0f + 0.5f);
                            e[1] = get_cube_coord(coordmap[f][0], x, y, size) / size * 255.0f + 0.5f;
                            e[2] = get_cube_coord(coordmap[f][1], x, y, size) / size * 255.0f + 0.5f;
                            e[3] = get_cube_coord(coordmap[f][2], x, y, size) / size * 255.0f + 0.5f;
                            expected = e[0] << 24 | e[1] << 16 | e[2] << 8 | e[3];

                            ok(color_match(v, e),
                               "Texel at face %u (%u, %u) doesn't match: %#x, expected %#x\n",
                               f, x, y, value, expected);
                        }
                    }
                    IDirect3DCubeTexture9_UnlockRect(tex, f, m);
                }
            }
            size >>= 1;
        }
    }
    else
        skip("Failed to create texture\n");

    IDirect3DCubeTexture9_Release(tex);

    hr = IDirect3DDevice9_CreateCubeTexture(device, 4, 1, 0, D3DFMT_A1R5G5B5,
                                            D3DPOOL_MANAGED, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFillCubeTexture(tex, fillfunc_cube, NULL);
        ok(hr == D3D_OK, "D3DXFillTexture returned %#x, expected %#x\n", hr, D3D_OK);
        for (f = 0; f < 6; f++)
        {
            hr = IDirect3DCubeTexture9_LockRect(tex, f, 0, &lock_rect, NULL, D3DLOCK_READONLY);
            ok(hr == D3D_OK, "Couldn't lock the texture, error %#x\n", hr);
            if (SUCCEEDED(hr))
            {
                pitch = lock_rect.Pitch / sizeof(WORD);
                for (y = 0; y < 4; y++)
                {
                    for (x = 0; x < 4; x++)
                    {
                        value = ((WORD *)lock_rect.pBits)[y * pitch + x];
                        v[0] = value >> 15;
                        v[1] = value >> 10 & 0x1f;
                        v[2] = value >> 5 & 0x1f;
                        v[3] = value & 0x1f;

                        e[0] = (f == 0) || (f == 1) ?
                            0 : (BYTE)(1.0f / size * 2.0f + 0.5f);
                        e[1] = get_cube_coord(coordmap[f][0], x, y, 4) / 4 * 31.0f + 0.5f;
                        e[2] = get_cube_coord(coordmap[f][1], x, y, 4) / 4 * 31.0f + 0.5f;
                        e[3] = get_cube_coord(coordmap[f][2], x, y, 4) / 4 * 31.0f + 0.5f;
                        expected = e[0] << 15 | e[1] << 10 | e[2] << 5 | e[3];

                        ok(color_match(v, e),
                           "Texel at face %u (%u, %u) doesn't match: %#x, expected %#x\n",
                           f, x, y, value, expected);
                    }
                }
                IDirect3DCubeTexture9_UnlockRect(tex, f, 0);
            }
        }
    }
    else
        skip("Failed to create texture\n");

    IDirect3DCubeTexture9_Release(tex);
}

static void WINAPI fillfunc_volume(D3DXVECTOR4 *value, const D3DXVECTOR3 *texcoord,
                                   const D3DXVECTOR3 *texelsize, void *data)
{
    value->x = texcoord->x;
    value->y = texcoord->y;
    value->z = texcoord->z;
    value->w = texelsize->x;
}

static void test_D3DXFillVolumeTexture(IDirect3DDevice9 *device)
{
    IDirect3DVolumeTexture9 *tex;
    HRESULT hr;
    D3DLOCKED_BOX lock_box;
    DWORD x, y, z, m;
    DWORD v[4], e[4];
    DWORD value, expected, size, row_pitch, slice_pitch;

    size = 4;
    hr = IDirect3DDevice9_CreateVolumeTexture(device, size, size, size, 0, 0, D3DFMT_A8R8G8B8,
                                              D3DPOOL_MANAGED, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFillVolumeTexture(tex, fillfunc_volume, NULL);
        ok(hr == D3D_OK, "D3DXFillVolumeTexture returned %#x, expected %#x\n", hr, D3D_OK);

        for (m = 0; m < 3; m++)
        {
            hr = IDirect3DVolumeTexture9_LockBox(tex, m, &lock_box, NULL, D3DLOCK_READONLY);
            ok(hr == D3D_OK, "Couldn't lock the texture, error %#x\n", hr);
            if (SUCCEEDED(hr))
            {
                row_pitch = lock_box.RowPitch / sizeof(DWORD);
                slice_pitch = lock_box.SlicePitch / sizeof(DWORD);
                for (z = 0; z < size; z++)
                {
                    for (y = 0; y < size; y++)
                    {
                        for (x = 0; x < size; x++)
                        {
                            value = ((DWORD *)lock_box.pBits)[z * slice_pitch + y * row_pitch + x];
                            v[0] = (value >> 24) & 0xff;
                            v[1] = (value >> 16) & 0xff;
                            v[2] = (value >> 8) & 0xff;
                            v[3] = value & 0xff;

                            e[0] = 255.0f / size + 0.5f;
                            e[1] = (x + 0.5f) / size * 255.0f + 0.5f;
                            e[2] = (y + 0.5f) / size * 255.0f + 0.5f;
                            e[3] = (z + 0.5f) / size * 255.0f + 0.5f;
                            expected = e[0] << 24 | e[1] << 16 | e[2] << 8 | e[3];

                            ok(color_match(v, e),
                               "Texel at (%u, %u, %u) doesn't match: %#x, expected %#x\n",
                               x, y, z, value, expected);
                        }
                    }
                }
                IDirect3DVolumeTexture9_UnlockBox(tex, m);
            }
            size >>= 1;
        }
    }
    else
        skip("Failed to create texture\n");

    IDirect3DVolumeTexture9_Release(tex);

    hr = IDirect3DDevice9_CreateVolumeTexture(device, 4, 4, 4, 1, 0, D3DFMT_A1R5G5B5,
                                              D3DPOOL_MANAGED, &tex, NULL);

    if (SUCCEEDED(hr))
    {
        hr = D3DXFillVolumeTexture(tex, fillfunc_volume, NULL);
        ok(hr == D3D_OK, "D3DXFillTexture returned %#x, expected %#x\n", hr, D3D_OK);
        hr = IDirect3DVolumeTexture9_LockBox(tex, 0, &lock_box, NULL, D3DLOCK_READONLY);
        ok(hr == D3D_OK, "Couldn't lock the texture, error %#x\n", hr);
        if (SUCCEEDED(hr))
        {
            row_pitch = lock_box.RowPitch / sizeof(WORD);
            slice_pitch = lock_box.SlicePitch / sizeof(WORD);
            for (z = 0; z < 4; z++)
            {
                for (y = 0; y < 4; y++)
                {
                    for (x = 0; x < 4; x++)
                    {
                        value = ((WORD *)lock_box.pBits)[z * slice_pitch + y * row_pitch + x];
                        v[0] = value >> 15;
                        v[1] = value >> 10 & 0x1f;
                        v[2] = value >> 5 & 0x1f;
                        v[3] = value & 0x1f;

                        e[0] = 1;
                        e[1] = (x + 0.5f) / 4 * 31.0f + 0.5f;
                        e[2] = (y + 0.5f) / 4 * 31.0f + 0.5f;
                        e[3] = (z + 0.5f) / 4 * 31.0f + 0.5f;
                        expected = e[0] << 15 | e[1] << 10 | e[2] << 5 | e[3];

                        ok(color_match(v, e),
                           "Texel at (%u, %u, %u) doesn't match: %#x, expected %#x\n",
                           x, y, z, value, expected);
                    }
                }
            }
            IDirect3DVolumeTexture9_UnlockBox(tex, 0);
        }
    }
    else
        skip("Failed to create texture\n");

    IDirect3DVolumeTexture9_Release(tex);
}

static void test_D3DXCreateTextureFromFileInMemory(IDirect3DDevice9 *device)
{
    HRESULT hr;
    IDirect3DTexture9 *texture;

    hr = D3DXCreateTextureFromFileInMemory(device, dds_16bit, sizeof(dds_16bit), &texture);
    ok(hr == D3D_OK, "D3DXCreateTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);

    hr = D3DXCreateTextureFromFileInMemory(device, dds_24bit, sizeof(dds_24bit), &texture);
    ok(hr == D3D_OK, "D3DXCreateTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);

    hr = D3DXCreateTextureFromFileInMemory(device, dds_24bit, sizeof(dds_24bit) - 1, &texture);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXCreateTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);
}

static void test_D3DXCreateTextureFromFileInMemoryEx(IDirect3DDevice9 *device)
{
    HRESULT hr;
    IDirect3DTexture9 *texture;

    hr = D3DXCreateTextureFromFileInMemoryEx(device, dds_16bit, sizeof(dds_16bit), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
        0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
    ok(hr == D3D_OK, "D3DXCreateTextureFromFileInMemoryEx returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);

    hr = D3DXCreateTextureFromFileInMemoryEx(device, dds_16bit, sizeof(dds_16bit), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
        D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
    ok(hr == D3D_OK, "D3DXCreateTextureFromFileInMemoryEx returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);

    if (!is_autogenmipmap_supported(device, D3DRTYPE_TEXTURE))
    {
        skip("No D3DUSAGE_AUTOGENMIPMAP support for textures\n");
        return;
    }

    hr = D3DXCreateTextureFromFileInMemoryEx(device, dds_16bit, sizeof(dds_16bit), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
        D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);
    ok(hr == D3D_OK, "D3DXCreateTextureFromFileInMemoryEx returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DTexture9_Release(texture);
}

static void test_D3DXCreateCubeTextureFromFileInMemory(IDirect3DDevice9 *device)
{
    HRESULT hr;
    ULONG ref;
    DWORD levelcount;
    IDirect3DCubeTexture9 *cube_texture;
    D3DSURFACE_DESC surface_desc;

    hr = D3DXCreateCubeTextureFromFileInMemory(NULL, dds_cube_map, sizeof(dds_cube_map), &cube_texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXCreateCubeTextureFromFileInMemory(device, NULL, sizeof(dds_cube_map), &cube_texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXCreateCubeTextureFromFileInMemory(device, dds_cube_map, 0, &cube_texture);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXCreateCubeTextureFromFileInMemory(device, dds_cube_map, sizeof(dds_cube_map), NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXCreateCubeTextureFromFileInMemory(device, dds_cube_map, sizeof(dds_cube_map), &cube_texture);
    if (SUCCEEDED(hr))
    {
        levelcount = IDirect3DCubeTexture9_GetLevelCount(cube_texture);
        todo_wine ok(levelcount == 3, "GetLevelCount returned %u, expected 3\n", levelcount);

        hr = IDirect3DCubeTexture9_GetLevelDesc(cube_texture, 0, &surface_desc);
        ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
        ok(surface_desc.Width == 4, "Got width %u, expected 4\n", surface_desc.Width);
        ok(surface_desc.Height == 4, "Got height %u, expected 4\n", surface_desc.Height);

        hr = IDirect3DCubeTexture9_GetLevelDesc(cube_texture, 3, &surface_desc);
        todo_wine ok(hr == D3DERR_INVALIDCALL, "GetLevelDesc returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        ref = IDirect3DCubeTexture9_Release(cube_texture);
        ok(ref == 0, "Invalid reference count. Got %u, expected 0\n", ref);
    } else skip("Couldn't create cube texture\n");
}

static void test_D3DXCreateCubeTextureFromFileInMemoryEx(IDirect3DDevice9 *device)
{
    HRESULT hr;
    IDirect3DCubeTexture9 *cube_texture;

    if (!is_autogenmipmap_supported(device, D3DRTYPE_CUBETEXTURE))
    {
        skip("No D3DUSAGE_AUTOGENMIPMAP support for cube textures\n");
        return;
    }

    hr = D3DXCreateCubeTextureFromFileInMemoryEx(device, dds_cube_map, sizeof(dds_cube_map), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &cube_texture);
    ok(hr == D3D_OK, "D3DXCreateCubeTextureFromFileInMemoryEx returned %#x, expected %#x\n", hr, D3D_OK);
    if (SUCCEEDED(hr)) IDirect3DCubeTexture9_Release(cube_texture);
}

static void test_D3DXCreateVolumeTextureFromFileInMemory(IDirect3DDevice9 *device)
{
    HRESULT hr;
    ULONG ref;
    DWORD levelcount;
    IDirect3DVolumeTexture9 *volume_texture;
    D3DVOLUME_DESC volume_desc;

    todo_wine {
        hr = D3DXCreateVolumeTextureFromFileInMemory(NULL, dds_volume_map, sizeof(dds_volume_map), &volume_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, NULL, sizeof(dds_volume_map), &volume_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, dds_volume_map, 0, &volume_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, dds_volume_map, sizeof(dds_volume_map), NULL);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, dds_volume_map, sizeof(dds_volume_map), &volume_texture);
        if (SUCCEEDED(hr))
        {
            levelcount = IDirect3DVolumeTexture9_GetLevelCount(volume_texture);
            ok(levelcount == 3, "GetLevelCount returned %u, expected 3\n", levelcount);

            hr = IDirect3DVolumeTexture9_GetLevelDesc(volume_texture, 0, &volume_desc);
            ok(hr == D3D_OK, "GetLevelDesc returend %#x, expected %#x\n", hr, D3D_OK);
            ok(volume_desc.Width == 4, "Got width %u, expected 4\n", volume_desc.Width);
            ok(volume_desc.Height == 4, "Got height %u, expected 4\n", volume_desc.Height);
            ok(volume_desc.Depth == 2, "Got depth %u, expected 2\n", volume_desc.Depth);

            hr = IDirect3DVolumeTexture9_GetLevelDesc(volume_texture, 1, &volume_desc);
            ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
            ok(volume_desc.Width == 2, "Got width %u, expected 2\n", volume_desc.Width);
            ok(volume_desc.Height == 2, "Got height %u, expected 2\n", volume_desc.Height);
            ok(volume_desc.Depth == 1, "Got depth %u, expected 1\n", volume_desc.Depth);

            ref = IDirect3DVolumeTexture9_Release(volume_texture);
            ok(ref == 0, "Invalid reference count. Got %u, expected 0\n", ref);
        }
    }
}

START_TEST(texture)
{
    HWND wnd;
    IDirect3D9 *d3d;
    IDirect3DDevice9 *device;
    D3DPRESENT_PARAMETERS d3dpp;
    HRESULT hr;

    wnd = CreateWindow("static", "d3dx9_test", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if (!wnd) {
        skip("Couldn't create application window\n");
        return;
    }
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) {
        skip("Couldn't create IDirect3D9 object\n");
        DestroyWindow(wnd);
        return;
    }

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed   = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    hr = IDirect3D9_CreateDevice(d3d, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &device);
    if (FAILED(hr)) {
        skip("Failed to create IDirect3DDevice9 object %#x\n", hr);
        IDirect3D9_Release(d3d);
        DestroyWindow(wnd);
        return;
    }

    test_D3DXCheckTextureRequirements(device);
    test_D3DXCheckCubeTextureRequirements(device);
    test_D3DXCheckVolumeTextureRequirements(device);
    test_D3DXCreateTexture(device);
    test_D3DXFilterTexture(device);
    test_D3DXFillTexture(device);
    test_D3DXFillCubeTexture(device);
    test_D3DXFillVolumeTexture(device);
    test_D3DXCreateTextureFromFileInMemory(device);
    test_D3DXCreateTextureFromFileInMemoryEx(device);
    test_D3DXCreateCubeTextureFromFileInMemory(device);
    test_D3DXCreateCubeTextureFromFileInMemoryEx(device);
    test_D3DXCreateVolumeTextureFromFileInMemory(device);

    IDirect3DDevice9_Release(device);
    IDirect3D9_Release(d3d);
    DestroyWindow(wnd);
}
