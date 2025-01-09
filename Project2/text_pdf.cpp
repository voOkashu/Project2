#include<iostream>
#include<vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"

jmp_buf env;

#define FONT_PATH "../simkai.ttf"          //楷体文件目录
#define START_CHAR (000000)                // Unicode编码 起始
#define END_CHAR (65536)                   // Unicode编码 结束
#define CHARACTERS_PER_LINE 15             //每行成15个文字
#define LINES_PER_PAGE 20                  // 每页20行
#define X_SPACEING 30                      // 每一行每个字符之间的间隔
#define Y_SPACEING 40                     


        //将Unicode编码 转为utf-8
std::vector<uint8_t> unicode_code_point_to_utf8(uint32_t code_point)
{
    std::vector<uint8_t> utf8_bytes;

    if (code_point <= 0x7F)
    {
        utf8_bytes.push_back(code_point);
    }
    else if (code_point <= 0x7FF)
    {
        utf8_bytes.push_back(0xC0 | ((code_point >> 6) & 0x1F));
        utf8_bytes.push_back(0x80 | (code_point & 0x3F));
    }
    else if (code_point <= 0xFFFF)
    {
        utf8_bytes.push_back(0xE0 | ((code_point >> 12) & 0x0F));
        utf8_bytes.push_back(0x80 | ((code_point >> 6) & 0x3F));
        utf8_bytes.push_back(0x80 | (code_point & 0x3F));
    }
    else if (code_point <= 0x10FFFF) 
    {
        utf8_bytes.push_back(0xF0 | ((code_point >> 18) & 0x07));
        utf8_bytes.push_back(0x80 | ((code_point >> 12) & 0x3F));
        utf8_bytes.push_back(0x80 | ((code_point >> 6) & 0x3F));
        utf8_bytes.push_back(0x80 | (code_point & 0x3F));     
    }
    else
    {
        std::cerr << "Invalid Unicode code point." << std::endl;
    }

    return utf8_bytes;
}

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS   error_no,
    HPDF_STATUS   detail_no,
    void* user_data)
{
    printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
        (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

int main(int argc, char** argv)
{
    HPDF_Doc  pdf;
    HPDF_Font font;
    HPDF_Page page;


    char fname[256];
    unsigned char buf[8]; // 一个字符
    unsigned int i;

    strcpy(fname, argv[0]);
    strcat(fname, ".pdf");

    pdf = HPDF_New(error_handler, NULL);
    if (!pdf) {
        printf("不能创造pdf文件\n");
        return 1;
    }

    if (setjmp(env)) {
        HPDF_Free(pdf);
        return 1;
    }
    // 使用 UTF-8 编码
    HPDF_UseUTFEncodings(pdf);
    HPDF_SetCurrentEncoder(pdf, "UTF-8");
    

    // 加载 simkai 字体
    const char* _font = HPDF_LoadTTFontFromFile(pdf, FONT_PATH, HPDF_TRUE);
    font = HPDF_GetFont(pdf, _font, "UTF-8");

    // 添加 页
    page = HPDF_AddPage(pdf);
    // 设置页的大小
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    HPDF_Page_BeginText(page);
    // 设置字体大小
    HPDF_Page_SetFontAndSize(page, font, 12);
    // 获取页高
    float height = HPDF_Page_GetHeight(page);

    float x = X_SPACEING * 16, y = height;

    HPDF_Page_MoveTextPos(page, x, y);
    int char_count = 0;
    int idx = 0;
    for (i = START_CHAR; i <= END_CHAR; i++)
    {
        if (char_count % CHARACTERS_PER_LINE == 0)//15个换行
        {
            HPDF_Page_MoveTextPos(page, -X_SPACEING * 15, -40);
           
        }
        memset(buf, 0, sizeof(buf));
        // unicode 转 utf-8
        std::vector<uint8_t> tmp = unicode_code_point_to_utf8(i);
        for (int j = 0; j < tmp.size(); j++)
        {
            buf[j] = tmp[j];
        }
       
        HPDF_Page_ShowText(page, (const char*)buf); //放文字
        HPDF_Page_MoveTextPos(page, X_SPACEING, 0); //移动x坐标

        x += X_SPACEING;
        char_count++;
        if (char_count % 300 == 0) // 超过 300 进行分页
        {
            HPDF_Page_EndText(page);
            char_count = 0;
            x = X_SPACEING * 16;
            y = height;
            page = HPDF_AddPage(pdf);
            HPDF_Page_BeginText(page);
            HPDF_Page_SetFontAndSize(page, font, 12);
            HPDF_Page_MoveTextPos(page, x, y);

        }
    }

    /* save the document to a file */
    HPDF_SaveToFile(pdf, fname);

    /* clean up */
    HPDF_Free(pdf);

    system("pause");
    return 0;
}
