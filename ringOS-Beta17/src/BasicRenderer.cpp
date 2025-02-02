#include "BasicRenderer.hpp"

namespace Renderer
{
    BasicRenderer* GlobalRenderer;

    BasicRenderer::BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font, BMPImage* bmpImage)
    {
        TargetFramebuffer = targetFramebuffer;
        PSF1_Font = psf1_Font;
        BMPimage = bmpImage;
        Colour = 0xffffffff;
        CursorPosition = {0, 0};
    }

    void BasicRenderer::PutPix(uint32_t x, uint32_t y, uint32_t colour)
    {
        *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4)) = colour;
    }

    uint32_t BasicRenderer::GetPix(uint32_t x, uint32_t y){
        return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4));
    }

    void BasicRenderer::ClearMouseCursor(uint8_t* cursor, Point position)
    {
        if (!MouseDrawn) return;

        int xMax = 16;
        int yMax = 16;
        int differenceX = TargetFramebuffer->Width - position.X;
        int differenceY = TargetFramebuffer->Height - position.Y;

        if (differenceX < 16) xMax = differenceX;
        if (differenceY < 16) yMax = differenceY;

        for (int y = 0; y < yMax; y++){
            for (int x = 0; x < xMax; x++){
                int bit = y * 16 + x;
                int byte = bit / 8;
                if ((cursor[byte] & (0b10000000 >> (x % 8))))
                {
                    if (GetPix(position.X + x, position.Y + y) == MouseCursorBufferAfter[x + y *16]){
                        PutPix(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
                    }
                }
            }
        }
    }

    void BasicRenderer::DrawOverlayMouseCursor(uint8_t* cursor, Point position, uint32_t colour)
    {

        int xMax = 16;
        int yMax = 16;
        int differenceX = TargetFramebuffer->Width - position.X;
        int differenceY = TargetFramebuffer->Height - position.Y;

        if (differenceX < 16) xMax = differenceX;
        if (differenceY < 16) yMax = differenceY;

        for (int y = 0; y < yMax; y++)
        {
            for (int x = 0; x < xMax; x++)
            {
                int bit = y * 16 + x;
                int byte = bit / 8;
                if ((cursor[byte] & (0b10000000 >> (x % 8))))
                {
                    MouseCursorBuffer[x + y * 16] = GetPix(position.X + x, position.Y + y);
                    PutPix(position.X + x, position.Y + y, colour);
                    MouseCursorBufferAfter[x + y * 16] = GetPix(position.X + x, position.Y + y);

                }
            }
        }

        MouseDrawn = true;
    }

    void BasicRenderer::Clear()
    {
        uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
        uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
        uint64_t fbHeight = TargetFramebuffer->Height;
        uint64_t fbSize = TargetFramebuffer->BufferSize;

        for (int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline ++)
        {
            uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
            for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++)
            {
                *pixPtr = ClearColour;
            }
        }
    }

    void BasicRenderer::ClearChar()
    {

        if (CursorPosition.X == 0){
            CursorPosition.X = TargetFramebuffer->Width;
            CursorPosition.Y -= 16;
            if (CursorPosition.Y < 0) CursorPosition.Y = 0;
        }

        unsigned int xOff = CursorPosition.X;
        unsigned int yOff = CursorPosition.Y;

        unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
        for (unsigned long y = yOff; y < yOff + 16; y++)
        {
            for (unsigned long x = xOff - 8; x < xOff; x++)
            {
                *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = ClearColour;
            }
        }

        CursorPosition.X -= 8;

        if (CursorPosition.X < 0){
            CursorPosition.X = TargetFramebuffer->Width;
            CursorPosition.Y -= 16;
            if (CursorPosition.Y < 0) CursorPosition.Y = 0;
        }

    }

    void BasicRenderer::Next()
    {
        CursorPosition.X = 0;
        CursorPosition.Y += 16;
    }

    void BasicRenderer::Next(int type)
    {
        switch (type)
        {
            case 1:
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
                break;

            case 2:
                CursorPosition2.X = 0;
                CursorPosition2.Y += 16;
                break;
            
            default:
                break;
        }
    }

    void BasicRenderer::Print(const char* str)
    {
        char* chr = (char*)str;
        while(*chr != 0)
        {
            if (*chr == '\n')
            {
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
                chr++;
                continue;
            }
            PutChar(*chr, CursorPosition.X, CursorPosition.Y);
            CursorPosition.X+=8;
            if(CursorPosition.X + 8 > TargetFramebuffer->Width)
            {
                CursorPosition.X = 0;
                CursorPosition.Y += 16;
            }
            chr++;
        }
    }

    void BasicRenderer::Print(const char* str, int type)
    {  
        switch (type)
        {
            case 1:
            {
                char* chr = (char*)str;
                while(*chr != 0)
                {
                    if (*chr == '\n')
                    {
                        CursorPosition.X = 0;
                        CursorPosition.Y += 16;
                        chr++;
                        continue;
                    }
                    PutChar(*chr, CursorPosition.X, CursorPosition.Y);
                    CursorPosition.X+=8;
                    if(CursorPosition.X + 8 > TargetFramebuffer->Width)
                    {
                        CursorPosition.X = 0;
                        CursorPosition.Y += 16;
                    }
                    chr++;
                }
                break;
            }

            case 2:
            {
                char* chr = (char*)str;
                while(*chr != 0)
                {
                    if (*chr == '\n')
                    {
                        CursorPosition2.X = 0;
                        CursorPosition2.Y += 16;
                        chr++;
                        continue;
                    }
                    PutChar(*chr, CursorPosition2.X, CursorPosition2.Y);
                    CursorPosition2.X+=8;
                    if(CursorPosition2.X + 8 > TargetFramebuffer->Width)
                    {
                        CursorPosition2.X = 0;
                        CursorPosition2.Y += 16;
                    }
                    chr++;
                }
                break;
            }

            default:
                break;
        }
    }

    void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
    {
        unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
        char* fontPtr = (char*)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
        for (unsigned long y = yOff; y < yOff + 16; y++)
        {
            for (unsigned long x = xOff; x < xOff+8; x++)
            {
                if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
                {
                    *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = Colour;
                }
            }
            fontPtr++;
        }
    }

    void BasicRenderer::PutChar(char chr)
    {
        PutChar(chr, CursorPosition.X, CursorPosition.Y);
        CursorPosition.X += 8;
        if (CursorPosition.X + 8 > TargetFramebuffer->Width)
        {
            CursorPosition.X = 0; 
            CursorPosition.Y += 16;
        }
    }

    typedef __builtin_va_list va_list;
    #define va_start(ap, X) __builtin_va_start(ap, X)
    #define va_arg(ap, type) __builtin_va_arg(ap, type)
    #define va_end(ap) __builtin_va_end(ap)

    void _putc(unsigned char c) { GlobalRenderer->PutChar((char)c); }

    void BasicRenderer::printf(const char* fmt, ...)
    {
        const char digits[] = "0123456789abcdef";
        va_list ap;
        char buf[10];
        char* s;
        unsigned char u;
        int c, i, pad;

        va_start(ap, fmt);
        while ((c = *fmt++) != 0)
        {
            if (c == '%') 
            {
                c = *fmt++;
                if (c == 'l')
                    c = *fmt++;
                switch (c)
                {
                    case 'c':
                        _putc(va_arg(ap, int));
                        continue;
                    case 's':
                        s = va_arg(ap, char*);
                        if (s == (void*)0)
                            s = (char*)"<NULL>";
                        for (; *s; s++)
                            _putc((int)*s);
                        continue;
                    case '%':
                        _putc('%');
                        continue;
                    case 'd':
                    case 'i':
                        c = 'u';
                    case 'u':
                    case 'x':
                    case 'X':
                        u = va_arg(ap, unsigned);
                        s = buf;
                        if (c == 'u') 
                        {
                            do
                                *s++ = digits[u % 10U];
                            while (u /= 10U);
                        } 
                        else
                        {
                            pad = 0;
                            for (i = 0; i < 8; i++) 
                            {
                                if (pad)
                                    *s++ = '0';
                                else 
                                {
                                    *s++ = digits[u % 16U];
                                    if ((u /= 16U) == false)
                                        pad = 1;
                                }
                            }
                        }

                    while (--s >= buf)
                        _putc((int)*s);
                    continue;
                }
            }
            else if(c == '\n')
            {
                CursorPosition.X = 0; 
                CursorPosition.Y += 16;
                c++;
                continue;
            }
            _putc((int)c);
        }
        va_end(ap);
    }

    void BasicRenderer::TaskBar()
    {
        uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
        uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
        uint64_t fbHeight = TargetFramebuffer->Height;
        uint64_t fbSize = TargetFramebuffer->BufferSize;

        int verticalScanline = TargetFramebuffer->Height - 80;

        uint32_t colour = TASKBAR_COLOR;
        WindowStuff->taskbar_width = verticalScanline;
        WindowStuff->taskbar_height = 80;

        for (verticalScanline; verticalScanline < fbHeight; verticalScanline++)
        {
            uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
            for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++)
            {
                *pixPtr = colour;
            }
        }
    }

    void BasicRenderer::BMPPicture()
    {
        GlobalRenderer->CursorPosition = {-20, -20};

        if (BMPimage->height != TargetFramebuffer->Height || BMPimage->width != TargetFramebuffer->Width)
        {
            uint32_t previousColour = GlobalRenderer->Colour;
            GlobalRenderer->Colour = 0xffff0000;
            printf("BMP image is not the same resolution as the screen resolution!");
            GlobalRenderer->Next();
            GlobalRenderer->Colour= previousColour;
        }

        // Bottom-up rendering
        for (unsigned int y = 0; y < BMPimage->height; ++y)
        {
            for (unsigned int x = 0; x < BMPimage->width; ++x)
            {
                unsigned int* framebufferPtr = TargetFramebuffer->BaseAddress + BMPimage->width * y + x;
                unsigned int* pixPtr = BMPimage->bitmapBuffer + (BMPimage->height - 1 - y) * BMPimage->width + x;
                *framebufferPtr = *pixPtr;
            }
        }
    }
}